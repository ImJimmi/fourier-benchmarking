cmake_minimum_required(VERSION 3.30)

project("Fourier Benchmarking" VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cpm)
cpmaddpackage("gh:juce-framework/JUCE#develop")

juce_add_console_app(fourier-benchmarking PRODUCT_NAME ${PROJECT_NAME})

target_sources(
  fourier-benchmarking PRIVATE source/main.cpp source/benchmarking.h
                               source/fft.h source/dft.h source/math.h)
target_link_libraries(fourier-benchmarking PRIVATE juce::juce_dsp)
target_compile_features(fourier-benchmarking PRIVATE cxx_std_20)
install(TARGETS fourier-benchmarking DESTINATION .)
