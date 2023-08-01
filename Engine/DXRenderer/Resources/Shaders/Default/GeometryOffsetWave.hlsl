#include "Hologram.hlsli"

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
    float4 pixelWorldPos : PIXEL_WORLD_POS;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 Color : COLOR;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

[maxvertexcount(3)]
void GShader(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> output)
{
    float3 v0 = input[0].position;
    float3 v1 = input[1].position;
    float3 v2 = input[2].position;

    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;

    float4 normal = float4(normalize(cross(edge1, edge2)), 0.0f);
       
    [unroll] 
    for (int k = 0; k < 3; k++)
    {
        GS_OUTPUT outputTriangle;
        outputTriangle.texCoordScale = input[k].texCoordScale;
        outputTriangle.texArrIndex = input[k].texArrIndex;
        outputTriangle.texCoord = input[k].texCoord;
        outputTriangle.pixelWorldPos = input[k].position;
        outputTriangle.position = input[k].position + float4(vertexDistortion(input[k].position, normal), 0.0f);
        outputTriangle.position = mul(outputTriangle.position, viewProj);
        
        outputTriangle.normal = input[k].normal;
        outputTriangle.tangent = input[k].tangent;
        outputTriangle.bitangent = input[k].bitangent;
        outputTriangle.Color = input[k].color;
        output.Append(outputTriangle);
    }
    output.RestartStrip();
}