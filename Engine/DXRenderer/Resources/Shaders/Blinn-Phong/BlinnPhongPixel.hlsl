#include "BlinnPhong.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 pixelWorldPos : PIXEL_WORLD_POS;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

float4 PShader(PSInput input) : SV_TARGET
{
    float4 resultColor;
    if (input.texArrIndex[0] >= 0)
    {
        resultColor = g_diffuseTextureArray.Sample(g_anisotropicWrap, float3(input.texCoord.x * input.texCoordScale.x, input.texCoord.y * input.texCoordScale.y, input.texArrIndex[0]));
    }
    else
    {
        resultColor = input.color;
    }
    float3 blinnPhongColor = CalculateTotalBlinnPhongColor(input.pixelWorldPos, input.normal, resultColor, float3(1, 1, 1), 32);
    return float4(blinnPhongColor, 1);
}