
#include "../PBR/Cook-Torrance.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "../Globals/Samplers.hlsli"
#include "../Skybox/SkyboxShaderResources.hlsli"
#include "../Utils/ImportanceSampling.hlsli"
#include "../Utils/BasisFromDir.hlsli"

static const uint NumSamples = 1024;

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 direction : Direction;
};

float4 PShader(PSInput input) : SV_TARGET
{	
    float3 N = normalize(input.direction);
    float3 Lo = N;
	
    float3 S, T;
    basisFromDir(S, T, N);
    float3x3 hemisphereBasisMatrix = float3x3(S, T, N);

    float3 color = 0;
    float weight = 0;

    float rough2 = roughness_gen * roughness_gen;
    for (uint i = 0; i < NumSamples; ++i)
    {
        float2 u = randomHammersley(i, NumSamples);
        float3 Lh = normalize(mul(randomGGX(u, roughness_gen), hemisphereBasisMatrix));

        float3 Li = reflect(-Lo, Lh);
        float cosLi = dot(N, Li);
        
        if(cosLi > 0)
        {
            float mipLevel;
            if (roughness_gen >= 0.01)
            {
                float cosLh = max(dot(N, Lh), 0.0);
                float pdf = smith(rough2, cosLh, cosLh);
                mipLevel = max(0.5 * log2(3.0 * 2.0 * inputWidth * inputHeight / (PI * pdf * NumSamples)), 0.0);
            }
            else
            {
                mipLevel = 0;
            }
 
            color += skycubeTexture.SampleLevel(g_linearWrap, Li, mipLevel).rgb * cosLi;
            weight += cosLi;
        }
    }
    color /= weight;

    return float4(color, 1.0);
}
