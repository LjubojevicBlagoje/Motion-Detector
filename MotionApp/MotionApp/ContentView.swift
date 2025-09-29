//
//  ContentView.swift
//  MotionApp
//
//  Created by Blagoje Ljubojevic on 29/9/2025.
//

import SwiftUI
import AppKit

struct ContentView: View {
  @StateObject private var runner = LiveDiffRunner()

  var body: some View {
    VStack(spacing: 12) {
      // Preview of the exact current.jpg frame
      Group {
        if let img = runner.previewImage {
          Image(nsImage: img)
            .resizable()
            .aspectRatio(contentMode: .fit)
            .frame(minHeight: 240)
            .cornerRadius(12)
            .overlay(RoundedRectangle(cornerRadius: 12).stroke(.secondary.opacity(0.3)))
        } else {
          Rectangle()
            .fill(.secondary.opacity(0.08))
            .overlay(Text("Camera preview").foregroundColor(.secondary))
            .frame(height: 240)
            .cornerRadius(12)
        }
      }
      .overlay(alignment: .topLeading) {
        if runner.isMotionActive {
          Text(runner.bannerText)
            .font(.system(size: 18, weight: .heavy, design: .rounded))
            .padding(10)
            .background(Color.red.opacity(0.92))
            .foregroundColor(.white)
            .cornerRadius(8)
            .transition(.opacity)
        }
      }

      // Live stats + controls
      HStack(spacing: 16) {
        Text(String(format: "Motion: %.2f%%", runner.motionPct))
          .font(.system(size: 20, weight: .semibold, design: .rounded))
        Text(String(format: "Score: %.4f", runner.motionScore))
          .font(.system(size: 14, weight: .regular, design: .monospaced))
          .foregroundColor(.secondary)

        if let t = runner.lastDetection {
          Text("Last: \(t.formatted(date: .omitted, time: .standard))")
            .font(.system(size: 13, weight: .regular, design: .monospaced))
            .foregroundColor(.secondary)
        }

        Spacer()

        Button(runner.isRunning ? "Stop" : "Start") {
          runner.isRunning ? runner.stop() : runner.start()
        }
        .keyboardShortcut(.space, modifiers: [])
      }

      if let err = runner.lastError, !err.isEmpty {
        Text(err)
          .font(.system(size: 12, weight: .regular, design: .monospaced))
          .foregroundColor(.red)
          .frame(maxWidth: .infinity, alignment: .leading)
      }
    }
    .padding(18)
    .onAppear { if !runner.isRunning { runner.start() } }
  }
}
