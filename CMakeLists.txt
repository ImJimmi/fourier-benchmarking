cmake_minimum_required(VERSION 3.30)

project("Fourier Benchmarking" VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cpm)
cpmaddpackage("gh:juce-framework/JUCE#develop")

juce_add_console_app(fourier-benchmarking PRODUCT_NAME ${PROJECT_NAME})

target_sources(
  fourier-benchmarking
  PRIVATE source/main.cpp
          source/benchmarking.h
          source/fft.h
          source/dft.h
          source/math.h
          source/metal-dft.h
          source/hlsl-dft.h)

target_link_libraries(fourier-benchmarking PRIVATE juce::juce_dsp)

if(APPLE)
  find_library(METAL_FRAMEWORK Metal)
  find_library(METALKIT_FRAMEWORK MetalKit)
  find_library(FOUNDATION_FRAMEWORK Foundation)

  get_target_property(ARTEFACTS_DIR fourier-benchmarking
                      RUNTIME_OUTPUT_DIRECTORY)

  add_custom_command(
    OUTPUT ${ARTEFACTS_DIR}/metal-dft.air
    COMMAND
      xcrun -sdk macosx metal -c
      ${CMAKE_CURRENT_SOURCE_DIR}/source/metal-dft.metal -o
      ${ARTEFACTS_DIR}/metal-dft.air
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/metal-dft.metal
    COMMENT "Compiling Metal shader")
  add_custom_command(
    OUTPUT ${ARTEFACTS_DIR}/metal-dft.metallib
    COMMAND xcrun -sdk macosx metallib ${ARTEFACTS_DIR}/metal-dft.air -o
            ${ARTEFACTS_DIR}/metal-dft.metallib
    DEPENDS ${ARTEFACTS_DIR}/metal-dft.air
    COMMENT "Creating Metal shader library")

  add_custom_target(metal-dft ALL DEPENDS ${ARTEFACTS_DIR}/metal-dft.metallib)
  add_dependencies(fourier-benchmarking metal-dft)

  target_sources(
    fourier-benchmarking PRIVATE ${ARTEFACTS_DIR}/metal-dft.metallib
                                 source/metal-dft.mm)
  install(FILES ${ARTEFACTS_DIR}/metal-dft.metallib DESTINATION .)

  target_link_libraries(
    fourier-benchmarking PRIVATE ${METAL_FRAMEWORK} ${METALKIT_FRAMEWORK}
                                 ${FOUNDATION_FRAMEWORK})
endif()

if(WIN32)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hlsl-dft.cso
    COMMAND
      "dxc" "/T" "cs_6_0" "/E" "CSMain" "/Fo"
      "${CMAKE_CURRENT_BINARY_DIR}/hlsl-dft.cso"
      "${CMAKE_SOURCE_DIR}/source/hlsl-dft.hlsl"
    DEPENDS ${CMAKE_SOURCE_DIR}/source/hlsl-dft.hlsl)

  add_custom_target(CompileShaders
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hlsl-dft.cso)
  add_dependencies(fourier-benchmarking CompileShaders)
  target_link_libraries(fourier-benchmarking PRIVATE d3d11 dxgi d3dcompiler)

  target_sources(fourier-benchmarking PRIVATE source/hlsl-dft.hlsl
                                              source/hlsl-dft.cpp)
endif()

target_compile_features(fourier-benchmarking PRIVATE cxx_std_20)
install(TARGETS fourier-benchmarking DESTINATION .)
