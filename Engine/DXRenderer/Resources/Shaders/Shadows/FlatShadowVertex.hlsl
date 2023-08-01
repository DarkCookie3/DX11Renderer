#include "../Globals/ConstantBuffers.hlsli"

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
    float2 texCoordScale : TEX_COORD_SCALE;
    float creationTime : CREATION_TIME;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VShader(VSInput input)
{
    VSOutput output;
    output.position = mul(mul(input.position, input.instanceMatrix), viewProj);
    return output;
}