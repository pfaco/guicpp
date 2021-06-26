cmake_minimum_required(VERSION 3.20)

project(
  guicpp 
  
  LANGUAGES
    C CXX
)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(ResourceGenerator src/resources_generator.cpp)
target_link_libraries(ResourceGenerator PRIVATE fmt::fmt)
target_compile_features(ResourceGenerator PUBLIC cxx_std_17)
add_executable(Eletra::ResourceGenerator ALIAS ResourceGenerator)

add_library(
  ${PROJECT_NAME}

  src/gui.cpp
  src/backend_win32.cpp
)
add_library(pfaco::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
  ${PROJECT_NAME}
  
  PUBLIC 
    fmt::fmt
    imgui::imgui
)

target_compile_features(
  ${PROJECT_NAME}

  PUBLIC
    cxx_std_17
)

## Install headers
install(
    FILES
        include/gui/gui.h
        include/gui/backend.h
     DESTINATION
        include/gui
)

install(
    TARGETS
        ${PROJECT_NAME}
        ResourceGenerator
    EXPORT
        ${PROJECT_NAME}Config
    LIBRARY 
        DESTINATION 
            lib
    RUNTIME 
        DESTINATION 
            tools
)

install(
    EXPORT
        ${PROJECT_NAME}Config
    NAMESPACE
        pfaco::
    DESTINATION
        share/${PROJECT_NAME}/
)
