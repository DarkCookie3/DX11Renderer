#include "../Globals/ShaderResources.hlsli"
#include "../Globals/Samplers.hlsli"
#include "../Globals/ConstantBuffers.hlsli"

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

float4 PShader(PSInput input) : SV_TARGET
{
    float4 normedEmission = input.emission / max(input.emission.x,
	max(input.emission.y, max(input.emission.z, 1.0)));

    float3 cameraDir = normalize(camera_pos - input.pixelWorldPos);

    float NoV = dot(cameraDir, normalize(input.normal));
    return lerp(normedEmission,
		input.emission, pow(max(0.1, NoV), 2));
}