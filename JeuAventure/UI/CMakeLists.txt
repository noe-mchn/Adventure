project(UI)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
${HEADER_DIR}/UIManager.h
${HEADER_DIR}/HUD.h
${HEADER_DIR}/Menu.h
${HEADER_DIR}/DialogSystem.h
${HEADER_DIR}/UIElements.h
${HEADER_DIR}/Widget.h
)
set(SOURCES
${SOURCE_DIR}/UIManager.cpp
${SOURCE_DIR}/HUD.cpp
${SOURCE_DIR}/Menu.cpp
${SOURCE_DIR}/DialogSystem.cpp
${SOURCE_DIR}/UIElements.cpp
${SOURCE_DIR}/Widget.cpp
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

target_include_directories(UI PUBLIC ${SFML_INCLUDE_DIR}) 

link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Features
        Graphics
        System
        Utils
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
        
)

add_custom_command(TARGET UI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:UI>
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "UI")