project(Features)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
${HEADER_DIR}/Ability.h
${HEADER_DIR}/DamageSystem.h
${HEADER_DIR}/Hitbox.h
${HEADER_DIR}/CombatManager.h
${HEADER_DIR}/PhysicsEngine.h
${HEADER_DIR}/Collider.h
${HEADER_DIR}/CollisionManager.h
)
set(SOURCES
${SOURCE_DIR}/Ability.cpp
${SOURCE_DIR}/DamageSystem.cpp
${SOURCE_DIR}/Hitbox.cpp
${SOURCE_DIR}/CombatManager.cpp
${SOURCE_DIR}/PhysicsEngine.cpp
${SOURCE_DIR}/Collider.cpp
${SOURCE_DIR}/CollisionManager.cpp
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

target_include_directories(Features PUBLIC ${SFML_INCLUDE_DIR}) 

link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Utils
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
        
)

add_custom_command(TARGET Features POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:Features>
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Features")