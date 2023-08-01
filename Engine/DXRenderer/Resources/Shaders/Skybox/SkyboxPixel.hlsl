#include "../Globals/Samplers.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "SkyboxShaderResources.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 texCoord : TEXCOORD;
};

float4 PShader(PSInput input) : SV_Target
{
    float4 color = skycubeTexture.Sample(g_anisotropicWrap, input.texCoord);
    return color;
}