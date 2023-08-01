
#include "../Globals/Samplers.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "PostProcessShaderResources.hlsli"
#include "FXAA.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 texCoord : TEXCOORD;
};

float3 aces_tonemap(float3 color)
{
    float3x3 m1 = float3x3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
    float3x3 m2 = float3x3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602
	);
    float3 v = mul(color, m1);
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return saturate(mul((a / b), m2));
}


float4 PShader(PSInput input) : SV_Target
{
    float4 color = g_diffuseTexture.Load(input.position);
    
    uint width, height, levels;
    g_diffuseTexture.GetDimensions(0, width, height, levels);
    
    float EV_I_MAX = pow(2.0f, EV100) * 78.0f / 65.0f;
    color = color / EV_I_MAX;
    color = float4(aces_tonemap(color), 1);
    color = pow(color, 1.0f / 2.2f);
    
    return color;
}