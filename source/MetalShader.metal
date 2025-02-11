#include <metal_stdlib>
using namespace metal;

inline float2 complex_mul(float2 a, float2 b) {
    return float2(a.x * b.x - a.y * b.y,
                   a.x * b.y + a.y * b.x);
}

inline float2 complex_div(float2 a, float2 b) {
    float denominator = dot(b, b);
    return float2((a.x * b.x + a.y * b.y) / denominator,
                   (a.y * b.x - a.x * b.y) / denominator);
}

// calculates e^(j*theta)
inline float2 complex_exp(float theta) {
    return float2(cos(theta), sin(theta));
}

kernel void zTransform(device const float2* input [[buffer(0)]],
                       device const float* frequencies [[buffer(1)]],
                       device float2* output [[buffer(2)]],
                       device const float* sampleRate [[buffer(3)]],
                       device const uint* numInputs [[buffer(4)]],
                       uint id [[thread_position_in_grid]]) {
    const float2 z = complex_exp(frequencies[id] * 2.0f * M_PI_F / sampleRate[0]);

    output[id] = float2(0.0, 0.0);
    float2 zFactor = z;

    for (uint i = 0; i < numInputs[0]; i++) {
        zFactor = complex_div(zFactor, z);
        output[id] += complex_mul(input[i],  zFactor);
    }
}
