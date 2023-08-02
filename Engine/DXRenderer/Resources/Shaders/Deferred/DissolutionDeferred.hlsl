#include "../Globals/ConstantBuffers.hlsli"
#include "../Globals/Samplers.hlsli"
#include "../Globals/ShaderResources.hlsli"
#include "../PBR/LoadTextureValues.hlsli"
#include "../Utils/OctahedronPacking.hlsli"

static const float delay = 0.5f;
static const float noiseFactor = 2.0f;

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
    
    float noiseValue = g_noiseMap.Sample(g_linearWrap, input.texCoord);
    float creationTimeNoiseDelta = (g_time - input.creationTime) - noiseFactor * noiseValue - delay;
    if (creationTimeNoiseDelta <= 0.1)
    {
        if (creationTimeNoiseDelta <= 0.0)
        {
            discard;
        }
        else
        {
            output.albedo = input.color;
            output.emission = input.color;
        }
    }
    else
    {
        output.albedo = float4(LoadAlbedo(input.texCoord, input.texCoordScale, input.color, input.texArrIndex[0]), 1);
        output.emission = float3(LoadEmission(input.texCoord, input.texCoordScale, input.emission, input.texArrIndex[4]));
    }

    output.roughness_metallness.r = overwriteRoughness ? roughnessOverwriteValue : LoadRoughness(input.texCoord, input.texCoordScale, 0.5f, input.texArrIndex[2]);
    output.roughness_metallness.g = overwriteMetalness ? metalnessOverwriteValue : LoadMetalness(input.texCoord, input.texCoordScale, 0.2f, input.texArrIndex[3]);
    
    input.normal = normalize(input.normal);
    input.tangent = -normalize(input.tangent);
    input.bitangent = normalize(input.bitangent);
    float3x3 TBN = float3x3(
        input.tangent.x, input.tangent.y, input.tangent.z,
        input.bitangent.x, input.bitangent.y, input.bitangent.z,
        input.normal.x, input.normal.y, input.normal.z
        );
    
    float3 micronormal = LoadNormal(input.texCoord, input.texCoordScale, float3(0, 0, 1), input.texArrIndex[1]);
    micronormal = mul(normalize(micronormal), TBN);
    
    output.normals.rg = packOctahedron(input.normal);
    output.normals.ba = packOctahedron(micronormal);
    
    return output;
}