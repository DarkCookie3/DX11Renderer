#include "../Globals/Samplers.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "../Skybox/SkyboxShaderResources.hlsli"
#include "../PBR/Cook-Torrance.hlsli"
#include "../Utils/FibbonaciHemisphere.hlsli"
#include "../Utils/BasisFromDir.hlsli"

static const uint NumSamples = 1024 * 16;

struct PSInput
{
    float4 position : SV_POSITION;
    float4 direction : Direction;
};

float4 PShader(PSInput input) : SV_Target
{
    float3 irradiance = float3(0, 0, 0);

    float3 dir = normalize(input.direction.xyz);
    float3 up;
    float3 right;
    basisFromDir(right, up, dir);
    float3x3 hemisphereBasisMatrix = float3x3(right, up, dir);
    
    for (int i = 0; i < NumSamples; i++)
    {
        float NdotV = 0;
        float3 sampleDir = normalize(mul(randomHemisphere(NdotV, i, NumSamples), hemisphereBasisMatrix));
        float mipLevel = max(0.5 * log2(3.0 * inputWidth * inputHeight / NumSamples), 0.0);
        irradiance += skycubeTexture.SampleLevel(g_linearWrap, sampleDir, mipLevel) * NdotV * (float3(1, 1, 1) - fresnel(NdotV, float3(0.04, 0.04, 0.04)));
    }
    irradiance = 2 * irradiance / float(NumSamples);
    return float4(irradiance, 1);
}