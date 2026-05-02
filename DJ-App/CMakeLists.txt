cmake_minimum_required(VERSION 3.15)

project(DJApp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(JUCE)

# Create the JUCE application
juce_add_gui_app(DJApp
    COMPANY_NAME "DJ App"
    BUNDLE_ID "com.djapp"
    VERSION "1.0.0"
)

# Add source files
target_sources(DJApp PRIVATE
    Source/Main.cpp
    Source/Model/Track.cpp
    Source/Model/PlaylistManager.cpp
    Source/Model/AudioEngine.cpp
    Source/Controller/DJController.cpp
    Source/Components/ModernButton.cpp
    Source/Components/ModernSlider.cpp
    Source/Components/WaveformView.cpp
    Source/Components/VUMeter.cpp
    Source/Components/JogWheel.cpp
    Source/View/MainView.cpp
    Source/View/DeckView.cpp
    Source/View/MixerView.cpp
    Source/View/PlaylistView.cpp
)

# Link JUCE modules
target_link_libraries(DJApp PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Disable JUCE modules we don't need
target_compile_definitions(DJApp PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_WIN_WEBVIEW2=0
)

# Include directories
target_include_directories(DJApp PRIVATE
    Source
    Source/Model
    Source/Controller
    Source/Components
    Source/View
    JuceLibraryCode
)