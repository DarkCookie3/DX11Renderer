#include "../Globals/ShaderResources.hlsli"
#include "../LightSources/LightSourceTextureMask.hlsli"
#include "../PBR/LoadTextureValues.hlsli"
#include "../PBR/Cook-Torrance.hlsli"
#include "../Utils/ClosestPointSpherelight.hlsli"
#include "../Utils/SelectCubeFace.hlsli"

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

#include "../PBR/PBRCalculations.hlsli"

float4 PShader(PSInput input) : SV_TARGET
{
    float4 resultColor;
    float noiseValue = g_noiseMap.Sample(g_linearWrap, input.texCoord);
    float creationTimeNoiseDelta = (g_time - input.creationTime) - noiseFactor * noiseValue - delay;
    if (creationTimeNoiseDelta <= 0.1)
    {
        if(creationTimeNoiseDelta <= 0.0)
        {
            discard;
        }
        else
        {
            resultColor = input.color;
        }
    }
    else
    {
        input.normal = normalize(input.normal);
        input.tangent = -normalize(input.tangent);
        input.bitangent = normalize(input.bitangent);
        float3x3 TBN = float3x3(
        input.tangent.x, input.tangent.y, input.tangent.z,
        input.bitangent.x, input.bitangent.y, input.bitangent.z,
        input.normal.x, input.normal.y, input.normal.z
        );
    
        float3 albedo = LoadAlbedo(input.texCoord, input.texCoordScale, input.color, input.texArrIndex[0]);
        float3 normal = LoadNormal(input.texCoord, input.texCoordScale, float3(0, 0, 1), input.texArrIndex[1]);
        float roughness;
        float metalness;
        if (!overwriteRoughness)
        {
            roughness = LoadRoughness(input.texCoord, input.texCoordScale, 0.5f, input.texArrIndex[2]);
        }
        else
        {
            roughness = roughnessOverwriteValue;
        }
        if (!overwriteMetalness)
        {
            metalness = LoadMetalness(input.texCoord, input.texCoordScale, 0.2f, input.texArrIndex[3]);
        }
        else
        {
            metalness = metalnessOverwriteValue;
        }
        normal = mul(normalize(normal), TBN);
    
        PBRInput pbr_input;
        pbr_input.albedo = albedo;
        pbr_input.micronormal = normal;
        pbr_input.macronormal = input.normal;
        pbr_input.roughness = roughness;
        pbr_input.metalness = metalness;
        pbr_input.pixelWorldPos = input.pixelWorldPos;
        resultColor = CalculatePBR(pbr_input);
    }
    return resultColor;
}