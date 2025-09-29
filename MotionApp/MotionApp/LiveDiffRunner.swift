//
//  LiveDiffRunner.swift
//  MotionApp
//
//  Created by Blagoje Ljubojevic on 29/9/2025.
//

import Foundation
import AVFoundation
import CoreImage
import Combine
import AppKit

final class LiveDiffRunner: NSObject, ObservableObject, AVCaptureVideoDataOutputSampleBufferDelegate {
  // Public model the UI reads
  @Published var motionPct: Double = 0.0          // 0–100%
  @Published var motionScore: Double = 0.0        // 0.0–1.0 (from CLI)
  @Published var isRunning = false
  @Published var lastError: String?
  @Published var previewImage: NSImage?           // literal current.jpg frame
  @Published var lastDetection: Date?
  @Published private(set) var isMotionActive = false
  @Published private(set) var bannerText: String = ""

  // Detection thresholds (tweak to taste)
  private let percentThreshold: Double = 1.0      // trigger if ≥ 1%
  private let scoreThreshold: Double = 0.02       // or if ≥ 0.02
  private let bannerHoldSeconds: TimeInterval = 1.5

  // Capture + IO
  private let session = AVCaptureSession()
  private let captureQueue = DispatchQueue(label: "cam.queue")
  private let ioQueue = DispatchQueue(label: "io.queue")
  private let context = CIContext(options: nil)

  // Temp files and helper path
  private var tempDir: URL!
  private var prevURL: URL!
  private var currURL: URL!
  private var helperURL: URL!

  // Flow control
  private var busy = false
  private var lastSent = CFAbsoluteTimeGetCurrent()

  func start() {
    lastError = nil
    guard !isRunning else { return }
    isRunning = true

    // Prepare temp files reused every frame
    tempDir = URL(fileURLWithPath: NSTemporaryDirectory()).appendingPathComponent("MotionTemp", isDirectory: true)
    try? FileManager.default.createDirectory(at: tempDir, withIntermediateDirectories: true)
    prevURL = tempDir.appendingPathComponent("prev.jpg")
    currURL = tempDir.appendingPathComponent("current.jpg")

    // Find embedded helper
    guard let p = Bundle.main.path(forResource: "Helpers/DiffTool", ofType: nil) else {
      lastError = "Helper not found in bundle"
      isRunning = false
      return
    }
    helperURL = URL(fileURLWithPath: p)

    // Prepare camera devices before session config
    guard let cam = AVCaptureDevice.default(for: .video) else {
      lastError = "No camera device"
      isRunning = false
      return
    }
    guard let input = try? AVCaptureDeviceInput(device: cam) else {
      lastError = "Failed to create camera input (check Camera entitlement)"
      isRunning = false
      return
    }
    let output = AVCaptureVideoDataOutput()
    output.videoSettings = [kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32BGRA]
    output.alwaysDiscardsLateVideoFrames = true
    output.setSampleBufferDelegate(self, queue: captureQueue)

    // Session configuration
    session.beginConfiguration()
    session.sessionPreset = .low
    guard session.canAddInput(input) else {
      lastError = "Cannot add camera input"; session.commitConfiguration(); isRunning = false; return
    }
    session.addInput(input)
    guard session.canAddOutput(output) else {
      lastError = "Cannot add camera output"; session.commitConfiguration(); isRunning = false; return
    }
    session.addOutput(output)
    session.commitConfiguration()
    session.startRunning()
  }

  func stop() {
    if session.isRunning { session.stopRunning() }
    isRunning = false
    isMotionActive = false
    try? FileManager.default.removeItem(at: prevURL)
    try? FileManager.default.removeItem(at: currURL)
  }

  // Save CVPixelBuffer → JPEG
  private func writeJPEG(_ pb: CVPixelBuffer, to url: URL) {
    let ci = CIImage(cvPixelBuffer: pb)
    let opts: [CIImageRepresentationOption: Any] = [kCGImageDestinationLossyCompressionQuality as CIImageRepresentationOption: 0.7]
    try? context.writeJPEGRepresentation(of: ci, to: url,
                                         colorSpace: CGColorSpaceCreateDeviceRGB(),
                                         options: opts)
  }

  // Frame callback
  func captureOutput(_ output: AVCaptureOutput, didOutput sb: CMSampleBuffer, from connection: AVCaptureConnection) {
    guard isRunning, let pb = CMSampleBufferGetImageBuffer(sb) else { return }

    // throttle CLI invocations
    let now = CFAbsoluteTimeGetCurrent()
    if now - lastSent < 0.03 { return }
    lastSent = now

    ioQueue.async {
      // first frame -> prev.jpg
      if !FileManager.default.fileExists(atPath: self.prevURL.path) {
        CVPixelBufferLockBaseAddress(pb, .readOnly)
        self.writeJPEG(pb, to: self.prevURL)
        CVPixelBufferUnlockBaseAddress(pb, .readOnly)
        return
      }

      if self.busy { return }
      self.busy = true

      // write current.jpg
      CVPixelBufferLockBaseAddress(pb, .readOnly)
      self.writeJPEG(pb, to: self.currURL)
      CVPixelBufferUnlockBaseAddress(pb, .readOnly)

      // Update preview with the exact file the CLI reads
      if let data = try? Data(contentsOf: self.currURL, options: [.mappedIfSafe]),
         let img = NSImage(data: data) {
        DispatchQueue.main.async { self.previewImage = img }
      }

      // Run CLI and parse both percent and score
      self.runDiff(frame: self.currURL, mask: self.prevURL) { pct, score in
        // publish stats
        DispatchQueue.main.async {
          self.motionPct = pct
          self.motionScore = score
        }

        // motion decision using either metric
        if pct >= self.percentThreshold || score >= self.scoreThreshold {
          DispatchQueue.main.async {
            self.lastDetection = Date()
            self.isMotionActive = true
            self.bannerText = String(
              format: "MOTION DETECTED  •  %0.2f%%  •  score %0.4f  •  %@",
              pct, score,
              DateFormatter.localizedString(from: self.lastDetection!,
                                            dateStyle: .none,
                                            timeStyle: .medium)
            )
          }
          // auto-hide after a short hold
          DispatchQueue.main.asyncAfter(deadline: .now() + self.bannerHoldSeconds) {
            self.isMotionActive = false
          }
        }

        // slide window
        try? FileManager.default.removeItem(at: self.prevURL)
        try? FileManager.default.copyItem(at: self.currURL, to: self.prevURL)

        self.busy = false
      }
    }
  }

  /// Launch CLI and parse: "percentage of pixels changed: NN" and trailing "(0.xxxx)" score if present
  private func runDiff(frame: URL, mask: URL, completion: @escaping (Double, Double) -> Void) {
    guard let helperURL else { completion(0, 0); return }

    let task = Process()
    task.executableURL = helperURL
    task.arguments = [frame.path, mask.path]

    let outPipe = Pipe()
    let errPipe = Pipe()
    task.standardOutput = outPipe
    task.standardError = errPipe

    task.terminationHandler = { _ in
      let out = outPipe.fileHandleForReading.readDataToEndOfFile()
      let s = String(data: out, encoding: .utf8) ?? ""

      // Parse percent
      var pct: Double = 0
      if let r = s.range(of: #"percentage of pixels changed:\s+(\d+)"#, options: .regularExpression) {
        let n = String(s[r]).components(separatedBy: CharacterSet.decimalDigits.inverted).joined()
        pct = Double(n) ?? 0
      }

      // Parse score inside parentheses e.g. "(0.0123)"
      var score: Double = 0
      if let r2 = s.range(of: #"\((0\.\d+)\)"#, options: .regularExpression) {
        let f = String(s[r2]).trimmingCharacters(in: CharacterSet(charactersIn: "()"))
        score = Double(f) ?? 0
      }

      completion(pct, score)
    }

    do { try task.run() } catch {
      DispatchQueue.main.async { self.lastError = "Failed to run DiffTool: \(error)" }
      completion(0, 0)
    }
  }
}
