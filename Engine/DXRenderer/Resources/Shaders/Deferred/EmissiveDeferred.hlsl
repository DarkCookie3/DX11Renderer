#include "../Globals/ConstantBuffers.hlsli"
#include "../Globals/Samplers.hlsli"
#include "../Globals/ShaderResources.hlsli"
#include "../PBR/LoadTextureValues.hlsli"
#include "../Utils/OctahedronPacking.hlsli"

struct PSInput
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

struct PSOutput
{
    float4 albedo : SV_TARGET0;
    float2 roughness_metallness : SV_TARGET1;
    float4 normals : SV_TARGET2;
    float3 emission : SV_TARGET3;
};


PSOutput PShader(PSInput input)
{
    PSOutput output;
    output.albedo = input.color;
    output.emission = float3(LoadEmission(input.texCoord, input.texCoordScale, input.emission, input.texArrIndex[4]));

    output.normals.rg = packOctahedron(normalize(input.normal));
    
    return output;
}