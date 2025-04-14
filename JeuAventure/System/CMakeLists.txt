project(System)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
${HEADER_DIR}/Game.h
${HEADER_DIR}/StateManager.h
${HEADER_DIR}/Application.h
)
set(SOURCES
${SOURCE_DIR}/Game.cpp
${SOURCE_DIR}/StateManager.cpp
${SOURCE_DIR}/Application.cpp
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

target_include_directories(System PUBLIC ${SFML_INCLUDE_DIR}) 

link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Utils
        States
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
        
)

add_custom_command(TARGET System POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:System>
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "System")