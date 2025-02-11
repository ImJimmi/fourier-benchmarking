#include "metal-dft.h"

#import <Metal/Metal.h>
#include <iostream>

id<MTLDevice> device = MTLCreateSystemDefaultDevice();
id<MTLCommandQueue> commandQueue = [device newCommandQueue];

[[nodiscard]] static auto getShaderLib()
{
    NSError* error = nullptr;
    NSString* shaderPath = [[NSBundle mainBundle] pathForResource:@"metal-dft"
                                                           ofType:@"metallib"];
    id<MTLLibrary> library = [device newLibraryWithFile:shaderPath error:&error];

    if (!library)
    {
        std::cerr << "Failed to load Metal library: "
                  << error.localizedDescription.UTF8String << std::endl;
    }

    return library;
}

[[nodiscard]] static auto getPipelineState()
{
    NSError* error = nullptr;
    auto library = getShaderLib();
    auto function = [library newFunctionWithName:@"zTransform"];
    auto pipelineState = [device newComputePipelineStateWithFunction:function
                                                               error:&error];
    return pipelineState;
}

std::vector<std::complex<float>> metalDFT(const std::vector<float>& frequencies,
                                          float sampleRate,
                                          const std::vector<std::complex<float>>& input)
{
    std::vector<std::complex<float>> output(std::size(input));
    const auto numInputs = static_cast<uint32_t>(std::size(input));
    const auto numFrequencies = static_cast<uint32_t>(std::size(frequencies));

    auto pipelineState = getPipelineState();
    auto commandBuffer = [commandQueue commandBuffer];
    auto encoder = [commandBuffer computeCommandEncoder];

    auto inputBuffer = [device newBufferWithBytes:std::data(input)
                                           length:std::size(input) * sizeof(decltype(*std::begin(input)))
                                          options:MTLResourceStorageModeShared];
    auto frequenciesBuffer = [device newBufferWithBytes:std::data(frequencies)
                                                 length:std::size(frequencies) * sizeof(decltype(*std::begin(frequencies)))
                                                options:MTLResourceStorageModeShared];
    auto outputBuffer = [device newBufferWithLength:std::size(output) * sizeof(decltype(*std::begin(output)))
                                            options:MTLResourceStorageModeShared];
    auto sampleRateBuffer = [device newBufferWithBytes:&sampleRate
                                                length:sizeof(sampleRate)
                                               options:MTLResourceStorageModeShared];
    auto numInputsBuffer = [device newBufferWithBytes:&numInputs
                                               length:sizeof(numInputs)
                                              options:MTLResourceStorageModeShared];
    [encoder setComputePipelineState:pipelineState];
    [encoder setBuffer:inputBuffer offset:0 atIndex:0];
    [encoder setBuffer:frequenciesBuffer offset:0 atIndex:1];
    [encoder setBuffer:outputBuffer offset:0 atIndex:2];
    [encoder setBuffer:sampleRateBuffer offset:0 atIndex:3];
    [encoder setBuffer:numInputsBuffer offset:0 atIndex:4];

    NSUInteger threadGroupSize = pipelineState.maxTotalThreadsPerThreadgroup;
    MTLSize gridSize = MTLSizeMake(std::size(frequencies), 1, 1);
    MTLSize threadGroupSize3D = MTLSizeMake(threadGroupSize, 1, 1);

    [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadGroupSize3D];
    [encoder endEncoding];

    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

    memcpy(output.data(), outputBuffer.contents, output.size() * sizeof(decltype(*std::begin(output))));
    return output;
}
