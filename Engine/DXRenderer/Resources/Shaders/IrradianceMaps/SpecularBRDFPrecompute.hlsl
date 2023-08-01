#include "../PBR/Cook-Torrance.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "../Globals/Samplers.hlsli"
#include "../Skybox/SkyboxShaderResources.hlsli"
#include "../Utils/ImportanceSampling.hlsli"
#include "../Utils/BasisFromDir.hlsli"

static const uint NumSamples = 4096;

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
    float r = roughness;
    float k = (r * r) / 2.0; 
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 direction : Direction;
};

float2 calculateBRDFIntegration(PSInput input)
{
    float cosLo = max(input.Position.x / float(inputWidth), 0.0001);
    float roughness = (inputHeight - input.Position.y) / float(inputHeight);
    
    float3 Lo = float3(sqrt(1.0 - cosLo * cosLo), 0.0, cosLo);
    
    roughness = max(roughness, 0.035);
    float DFG1 = 0.0;
    float DFG2 = 0.0;

    for (uint i = 0; i < NumSamples; ++i)
    {
        float2 u = randomHammersley(i, NumSamples);
        float3 Lh = randomGGX(u, roughness);
        float3 Li = reflect(-Lo, Lh);

        float cosLi = saturate(Li.z);
        float cosLh = saturate(Lh.z);
        float cosLoLh = dot(Lo, Lh);
        if (cosLi > 0.0)
        {
            float G = gaSchlickGGX_IBL(cosLi, cosLo, roughness);
            float Gv = G * cosLoLh / (cosLh * cosLo);
            float Fc = pow(1.0 - cosLoLh, 5);

            DFG1 += (1 - Fc) * Gv;
            DFG2 += Fc * Gv;
        }
    }

    float2 result;
    result.x = DFG1 / float(NumSamples);
    result.y = DFG2 / float(NumSamples);
    return result;
}

float2 PShader(PSInput input) : SV_TARGET
{
    float2 integratedValue = calculateBRDFIntegration(input);
    return integratedValue;
}