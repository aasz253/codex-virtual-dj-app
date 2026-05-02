# 🎧 Codex Virtual DJ App

A professional-grade virtual DJ application built with JUCE framework, featuring dual-deck mixing, real-time audio processing, and a sleek modern interface.

![Codex Virtual DJ Interface](DJ-App/Screenshot%20from%202026-05-02%2008-40-43.png)

##  Features

- **Dual Deck Mixing** — Independent playback and control for two decks with full transport controls
- **Jog Wheel** — Click and drag for precise track scratching and seeking
- **Crossfader** — Smooth transitions between decks with adjustable curve
- **VU Meters** — Real-time audio level monitoring for both channels
- **EQ & Filter Controls** — Shape your sound with 3-band EQ and filter knobs
- **Playlist Management** — Load, organize, and queue tracks with drag-and-drop support
- **Waveform Display** — Visual track representation for accurate beat matching
- **Master Volume & Cue** — Dedicated controls for monitoring and output

## ️ Tech Stack

- **Framework**: JUCE 7
- **Language**: C++17
- **Build System**: CMake / Xcode
- **Architecture**: MVC (Model-View-Controller)

## 📦 Quick Start

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- JUCE framework (included as submodule)

### Build

```bash
cd DJ-App
cmake -S . -B build
cmake --build build -j$(nproc)
```

### Run

```bash
./build/DJApp_artefacts/DJApp
```

## 📂 Project Structure

```
DJ-App/
├── Source/
│   ├── Model/          # Track, AudioEngine, PlaylistManager
│   ├── View/           # MainView, DeckView, MixerView, PlaylistView
│   ├── Controller/     # DJController business logic
│   ├── Components/     # JogWheel, ModernButton, VUMeter, WaveformView
│   └── Main.cpp        # Entry point
├── Builds/             # Platform-specific IDE projects
├── JUCE/               # JUCE framework submodule
── CMakeLists.txt      # Build configuration
```

## 📄 License

MIT

---

Built with ❤️ using JUCE
