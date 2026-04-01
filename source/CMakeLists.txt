juce_add_gui_app(OreginaldProStudio
    PRODUCT_NAME "Oreginald Pro Studio"
)

juce_generate_juce_header(OreginaldProStudio)

target_sources(OreginaldProStudio
    PRIVATE
        Main.cpp
        MainComponent.cpp

        TransportBar.cpp
        BrowserPanel.cpp
        InspectorPanel.cpp
        ArrangementView.cpp
        StatusBar.cpp

        TrackEngine.cpp

        ClipComponent.cpp
)

target_include_directories(OreginaldProStudio
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(OreginaldProStudio
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(OreginaldProStudio
    PRIVATE
        juce::juce_audio_utils
        juce::juce_gui_extra

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)