#include "../../Globals/ConstantBuffers.hlsli"


struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    row_major float4x4 localToWorld : INSTANCE_MODEL_MATRIX;
    row_major float4x4 worldToLocal : INSTANCE_MODEL_MATRIX_INV;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 screenSpacePos : SCREEN_SPACE_POS;
    row_major float4x4 worldToLocal : INSTANCE_MODEL_MATRIX_INV;
};

VSOutput VShader(VSInput input)
{
    VSOutput output;
    output.worldToLocal = input.worldToLocal;
    output.position = mul(mul(input.position, input.localToWorld), viewProj);
    output.screenSpacePos = (output.position.xyz / output.position.w).xy;
    return output;
}