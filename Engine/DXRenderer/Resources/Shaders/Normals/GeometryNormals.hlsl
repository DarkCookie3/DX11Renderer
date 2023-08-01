#include "../Globals/ConstantBuffers.hlsli"

struct GS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    row_major float4x4 instanceMatrix : INSTANCE_MODEL_MATRIX;
    row_major float4x4 instanceMatrixInversed : INSTANCE_MODEL_MATRIX_INV;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 Color : COLOR;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

[maxvertexcount(2)]
void GShader(triangle GS_INPUT input[3], inout LineStream<GS_OUTPUT> output)
{
    [unroll] 
    for (int k = 0; k < 3; k++)
    {
        GS_OUTPUT outputLine;
        outputLine.texCoordScale = input[k].texCoordScale;
        outputLine.texArrIndex = input[k].texArrIndex;
        outputLine.texCoord = input[k].texCoord;
        outputLine.normal = input[k].normal;
        outputLine.tangent = input[k].tangent;
        outputLine.bitangent = input[k].bitangent;
        outputLine.Color = outputLine.normal / 2 + 0.5;
        
        float4 pointIn = input[k].position;
        float4 pointOut = pointIn + outputLine.normal/10;

        outputLine.position = mul(pointIn, viewProj);
        output.Append(outputLine);
        outputLine.position = mul(pointOut, viewProj);
        output.Append(outputLine);
        output.RestartStrip();
    }
}
