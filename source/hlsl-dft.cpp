#include "hlsl-dft.h"

#include <d3d11.h>
#include <fstream>

[[nodiscard]] static std::vector<char> LoadShader(const std::wstring& filename)
{
    std::ifstream file{ filename, std::ios::binary };

    if (!file)
        throw std::runtime_error{ "Failed to open shader file" };

    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

[[nodiscard]] std::vector<std::complex<float>> hlslDFT(const std::vector<float>& frequencies,
                                                       float sampleRate,
                                                       const std::vector<std::complex<float>>& input)
{
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
    auto shaderData = LoadShader(L"hlsl-dft.cso");
    device->CreateComputeShader(shaderData.data(), shaderData.size(), nullptr, &computeShader);
    context->CSSetShader(computeShader.Get(), nullptr, 0);
    context->Dispatch(1, 1, 1); // Adjust thread groups as needed
}
