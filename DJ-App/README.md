# DJ Desktop App

A modern, cross-platform DJ application built with JUCE framework, featuring a clean MVC architecture and professional audio capabilities.

<img width="1395" height="922" alt="image" src="https://github.com/user-attachments/assets/b702edc9-b21b-4721-b1b1-29badeefbe40" />

## Features

🎵 **Professional DJ Tools**
- Dual deck mixing interface
- Real-time waveform display
- Jog wheel controls for precise track manipulation
- VU meters for audio level monitoring
- Modern slider and button components

🎛️ **Audio Engine**
- High-quality audio processing
- Low-latency audio playback
- Support for multiple audio formats
- Professional mixer controls

📁 **Playlist Management**
- Track library organization
- Playlist creation and management
- Audio file metadata extraction
- Drag-and-drop functionality

## Architecture

This application follows a clean **Model-View-Controller (MVC)** pattern:

```
Source/
├── Model/           # Data layer
│   ├── Track.cpp/h         # Track metadata and file handling
│   ├── AudioEngine.cpp/h   # Audio processing engine
│   └── PlaylistManager.cpp/h # Playlist management
├── View/            # UI layer
│   ├── MainView.cpp/h      # Main application window
│   ├── DeckView.cpp/h      # Individual deck interface
│   ├── MixerView.cpp/h     # Mixer controls
│   └── PlaylistView.cpp/h  # Playlist interface
├── Controller/      # Business logic
│   └── DJController.cpp/h  # Main application controller
├── Components/      # Reusable UI components
│   ├── JogWheel.cpp/h      # Jog wheel control
│   ├── ModernButton.cpp/h  # Custom button component
│   ├── ModernSlider.cpp/h  # Custom slider component
│   ├── VUMeter.cpp/h       # Audio level meter
│   └── WaveformView.cpp/h  # Waveform display
└── Main.cpp         # Application entry point
```

## Requirements

### System Requirements
- **macOS**: 10.13 or later
- **Windows**: Windows 10 or later
- **Linux**: Ubuntu 18.04+ or equivalent

### Development Requirements
- JUCE Framework (included)
- C++17 compatible compiler
- CMake 3.15+ or Xcode (macOS)
- Audio interface (recommended for professional use)

## Building the Project

### macOS (Xcode)

1. Navigate to the build directory:
   ```bash
   cd Builds/MacOSX
   ```

2. Build the project:
   ```bash
   xcodebuild -project thisMightWork.xcodeproj -scheme "thisMightWork - App" -configuration Release
   ```

3. Run the application:
   ```bash
   open build/Release/NewProject.app
   ```

### Cross-Platform (CMake)

1. Create build directory:
   ```bash
   mkdir build && cd build
   ```

2. Configure and build:
   ```bash
   cmake ..
   make
   ```

## Project Structure

- **`Source/`** - Main application source code
- **`Builds/`** - Platform-specific build configurations
- **`JuceLibraryCode/`** - JUCE framework integration
- **`Archive/`** - Legacy components (archived during refactoring)
- **`CMakeLists.txt`** - CMake build configuration
- **`DJ GUI_New.jucer`** - JUCE project file

## Usage

1. **Loading Tracks**: Use the playlist view to browse and load audio files
2. **Deck Control**: Each deck provides play/pause, cue, and tempo controls
3. **Mixing**: Use the mixer section to blend between tracks
4. **Jog Wheels**: Click and drag for precise track positioning
5. **Waveform**: Visual representation helps with beat matching

## Development

### Code Style
- Follow JUCE coding conventions
- Use meaningful variable and function names
- Maintain separation of concerns (MVC pattern)
- Document complex algorithms and audio processing code

### Adding New Features
1. Determine the appropriate layer (Model/View/Controller)
2. Create new components in the `Components/` directory if reusable
3. Update the controller to handle new business logic
4. Ensure proper memory management for audio buffers

## Audio Considerations

- **Latency**: Optimized for low-latency audio processing
- **Buffer Sizes**: Configurable audio buffer sizes for different hardware
- **Sample Rates**: Supports standard sample rates (44.1kHz, 48kHz, 96kHz)
- **Bit Depth**: 16-bit and 24-bit audio support

## Troubleshooting

### Build Issues
- Ensure JUCE modules are properly linked
- Check that all archived files are excluded from build
- Verify audio driver compatibility

### Audio Issues
- Check audio device settings in system preferences
- Ensure exclusive audio device access
- Adjust buffer sizes for your hardware

## Contributing

1. Fork the repository
2. Create a feature branch
3. Follow the existing code style
4. Test thoroughly with different audio formats
5. Submit a pull request

## Acknowledgments

- Built with [JUCE Framework](https://juce.com/)
- Modern UI components inspired by professional DJ software
- Audio processing optimized for real-time performance

---

**Note**: This application is designed for professional DJ use. For optimal performance, use dedicated audio interfaces and ensure your system meets the minimum requirements for real-time audio processing.
# codex-virtual-dj-app
