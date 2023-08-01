#include "GBufferResources.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "../Utils/OctahedronPacking.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 texCoord : TEXCOORD;
};

float4 PShader(PSInput input) : SV_TARGET
{
    float4 albedo = g_albedo.Load(input.position); 
    float4 packedNormals = g_normals.Load(input.position);
    float3 normal = unpackOctahedron(packedNormals.rg);
    float3 emission = g_emission.Load(input.position);
    float depth = g_depthStencil.Load(input.position).r;
    
    float4 clipSpaceCoord = float4(2 * input.texCoord.xy - 1, depth, 1);
    float4 pixelWorldPos = mul(clipSpaceCoord, viewProjInv);
    pixelWorldPos /= pixelWorldPos.w;
    
    float3 normedEmission = emission / max(emission.x,
	max(emission.y, max(emission.z, 1.0)));

    float3 cameraDir = normalize(camera_pos - pixelWorldPos);

    float NoV = dot(cameraDir, normalize(normal));
    float3 returnColor = lerp(normedEmission,
		emission, pow(max(0.1, NoV), 2));
    
    return float4(returnColor, 1);
}