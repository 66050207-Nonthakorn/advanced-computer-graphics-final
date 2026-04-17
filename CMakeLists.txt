cmake_minimum_required(VERSION 3.20)
project(advanced_computer_graphics_final LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/**/*.cpp
)

add_executable(advanced_computer_graphics_final ${SOURCE_FILES})

target_include_directories(advanced_computer_graphics_final PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include
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

# Copy shaders next to the binary
add_custom_command(TARGET advanced_computer_graphics_final POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/src/shaders
          $<TARGET_FILE_DIR:advanced_computer_graphics_final>/shaders
)
