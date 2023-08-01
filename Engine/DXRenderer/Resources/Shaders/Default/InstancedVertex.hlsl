#include "Hologram.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    nointerpolation float4 emission : EMISSION;
    row_major float4x4 instanceMatrix : INSTANCE_MODEL_MATRIX;
    row_major float4x4 instanceMatrixInversed : INSTANCE_MODEL_MATRIX_INV;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale: TEX_COORD_SCALE;
    float creationTime : CREATION_TIME;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 pixelWorldPos : PIXEL_WORLD_POS;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    nointerpolation float4 emission : EMISSION;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
    float creationTime : CREATION_TIME;
};

VSOutput VShader(VSInput input)
{
    VSOutput output;
    output.pixelWorldPos = mul(input.position, input.instanceMatrix);
    output.position = mul(output.pixelWorldPos, viewProj);
    float3x3 normalMat = (float3x3) (transpose(input.instanceMatrixInversed));
    output.normal = float4(mul(input.normal, normalMat), 0.0f);
    output.tangent = float4(mul(input.tangent, normalMat), 0.0f);
    output.bitangent = float4(mul(input.bitangent, normalMat), 0.0f);
    output.color = input.color;
    output.emission = input.emission;
    output.texArrIndex = input.texArrIndex;
    output.texCoordScale = input.texCoordScale;
    output.texCoord = input.texCoord;
    output.creationTime = input.creationTime;
    return output;
}