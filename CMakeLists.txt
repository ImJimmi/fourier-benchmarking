cmake_minimum_required(VERSION 3.30)

project("Fourier Benchmarking" VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cpm)
cpmaddpackage("gh:juce-framework/JUCE#develop")

juce_add_console_app(fourier-benchmarking PRODUCT_NAME ${PROJECT_NAME})

target_sources(
  fourier-benchmarking
  PRIVATE source/main.cpp source/benchmarking.h source/fft.h source/dft.h
          source/math.h source/metal-dft.h)

target_link_libraries(fourier-benchmarking PRIVATE juce::juce_dsp)

if(APPLE)
  find_library(METAL_FRAMEWORK Metal)
  find_library(METALKIT_FRAMEWORK MetalKit)
  find_library(FOUNDATION_FRAMEWORK Foundation)

  get_target_property(ARTEFACTS_DIR fourier-benchmarking
                      RUNTIME_OUTPUT_DIRECTORY)

  add_custom_command(
    OUTPUT ${ARTEFACTS_DIR}/MetalShader.air
    COMMAND
      xcrun -sdk macosx metal -c
      ${CMAKE_CURRENT_SOURCE_DIR}/source/MetalShader.metal -o
      ${ARTEFACTS_DIR}/MetalShader.air
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/MetalShader.metal
    COMMENT "Compiling Metal shader")

  add_custom_command(
    OUTPUT ${ARTEFACTS_DIR}/MetalShader.metallib
    COMMAND xcrun -sdk macosx metallib ${ARTEFACTS_DIR}/MetalShader.air -o
            ${ARTEFACTS_DIR}/MetalShader.metallib
    DEPENDS ${ARTEFACTS_DIR}/MetalShader.air
    COMMENT "Creating Metal shader library")

  add_custom_target(MetalShaders ALL
                    DEPENDS ${ARTEFACTS_DIR}/MetalShader.metallib)

  add_dependencies(fourier-benchmarking MetalShaders)

  target_sources(
    fourier-benchmarking PRIVATE ${ARTEFACTS_DIR}/MetalShader.metallib
                                 source/metal-dft.mm)
  install(FILES ${ARTEFACTS_DIR}/MetalShader.metallib DESTINATION .)

  target_link_libraries(
    fourier-benchmarking PRIVATE ${METAL_FRAMEWORK} ${METALKIT_FRAMEWORK}
                                 ${FOUNDATION_FRAMEWORK})
endif()

target_compile_features(fourier-benchmarking PRIVATE cxx_std_20)
install(TARGETS fourier-benchmarking DESTINATION .)
