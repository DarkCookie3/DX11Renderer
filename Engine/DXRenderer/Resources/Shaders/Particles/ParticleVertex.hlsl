
#include "../Globals/ConstantBuffers.hlsli"

struct VSInput
{
    row_major float4x4 instanceMatrix : INSTANCE_MODEL_MATRIX;
    float4 ColorRGBA : COLOR_RGBA;
    float4 emissionColor : COLOR_EMISSION;
    float animationState : ANIMATION_STATE;
    uint VertexID : SV_VertexID;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 pixelWorldPos : PIXEL_WORLD_POS;
    float2 texCoord : TEXCOORD;
    float4 ColorRGBA : COLOR_RGBA;
    float4 emissionColor : COLOR_EMISSION;
    float animationState : ANIMATION_STATE;
};

VSOutput VShader(VSInput input)
{
    VSOutput output;
    output.animationState = input.animationState;
    output.ColorRGBA = input.ColorRGBA;
    output.emissionColor = input.emissionColor;

    float4 position = float4(-1.0f + 2 * ((input.VertexID & 2) != 0), 1.0f - 2 * (input.VertexID % 2 == 0), 0.0f, 1.0f);
    output.texCoord = float2(position.x, -position.y) / 2 + 0.5;
    
    position = float4(mul(mul(position, (float3x3)input.instanceMatrix), (float3x3)viewInv), 1); 
    position += float4(input.instanceMatrix[3].xyz, 0);

    output.pixelWorldPos = position;
    output.Position = mul(position, viewProj);
    
    return output;
}