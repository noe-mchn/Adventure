project(Audio)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
${HEADER_DIR}/AudioManager.h
${HEADER_DIR}/SoundEffect.h
${HEADER_DIR}/MusicPlayer.h
)
set(SOURCES
${SOURCE_DIR}/AudioManager.cpp
${SOURCE_DIR}/SoundEffect.cpp
${SOURCE_DIR}/MusicPlayer.cpp
)

add_library(${PROJECT_NAME}

${SOURCES}
${HEADERS}
)


target_include_directories(${PROJECT_NAME}
PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${HEADER_DIR}>
)

target_include_directories(Audio PUBLIC ${SFML_INCLUDE_DIR}) 

link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Features
        Graphics
        States
        System
        UI
        Utils
        World
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
)

add_custom_command(TARGET Audio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:Audio>
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Audio")