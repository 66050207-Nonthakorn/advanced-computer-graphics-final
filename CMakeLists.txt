cmake_minimum_required(VERSION 3.20)
project(advanced_computer_graphics_final LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/Libs/*.cpp
  ${CMAKE_SOURCE_DIR}/src/main.cpp
)

add_executable(advanced_computer_graphics_final ${SOURCE_FILES})

target_include_directories(advanced_computer_graphics_final PRIVATE
  ${CMAKE_SOURCE_DIR}/src/Libs
)

target_link_libraries(advanced_computer_graphics_final PRIVATE
  OpenGL::GL
  glfw
  GLEW::GLEW
  glm::glm
)

if(APPLE)
  target_compile_definitions(advanced_computer_graphics_final PRIVATE GL_SILENCE_DEPRECATION)
  target_link_libraries(advanced_computer_graphics_final PRIVATE
    "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
  )
endif()

target_compile_options(advanced_computer_graphics_final PRIVATE $<$<CONFIG:Release>:/O2>)

# Copy shaders next to the binary (runs on every build)
add_custom_target(copy_shaders ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Shaders
          $<TARGET_FILE_DIR:advanced_computer_graphics_final>/Shaders
  COMMENT "Copying shaders to build directory"
)

# Copy models next to the binary (runs on every build)
add_custom_target(copy_models ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Models
          $<TARGET_FILE_DIR:advanced_computer_graphics_final>/Models
  COMMENT "Copying models to build directory"
)

# Copy textures next to the binary (runs on every build)
add_custom_target(copy_textures ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Textures
          $<TARGET_FILE_DIR:advanced_computer_graphics_final>/Textures
  COMMENT "Copying textures to build directory"
)
