#include "../Globals/ConstantBuffers.hlsli"

struct VSInput
{
    uint VertexID : SV_VertexID;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 direction : Direction;
};

VSOutput VShader(VSInput input)
{
    VSOutput output;

    output.Position = float4(3.0f - 4 * (input.VertexID != 2), -1.0f + 4 * (input.VertexID & 1), 0.0f, 1.0f);
    output.direction = float4(mul(float3(output.Position.xy, 1), (float3x3) viewInv), 0);
    
    return output;
}