TextureCube<float4> g_skycubeDiffuseIrradiance : register(t12);
TextureCube<float4> g_skycubeSpecularIrradiance : register(t13);
Texture2D<float2> g_skycubeSpecularBRDF : register(t14);

#include "GBufferResources.hlsli"
#include "../Utils/OctahedronPacking.hlsli"
#include "../LightSources/LightSources.hlsli"
#include "../LightSources/LightSourceTextureMask.hlsli"
#include "../PBR/Cook-Torrance.hlsli"
#include "../Utils/ClosestPointSpherelight.hlsli"
#include "../Utils/SelectCubeFace.hlsli"
#include "../PBR/PBRCalculations.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 texCoord : TEXCOORD;
};

float4 PShader(PSInput input) : SV_TARGET
{
    float4 albedo = g_albedo.Load(input.position);
    
    float4 packedNormals = g_normals.Load(input.position);
    float3 macronormal = unpackOctahedron(packedNormals.rg);
    float3 micronormal = unpackOctahedron(packedNormals.ba);
    
    float4 emission = g_emission.Load(input.position);
    
    float2 roughness_metalness = g_roughness_metalness.Load(input.position);
    float roughness = roughness_metalness.r;
    float metalness = roughness_metalness.g;
    
    float depth = g_depthStencil.Load(input.position).r;
    
    float4 clipSpaceCoord = float4(2 * input.texCoord.xy - 1, depth, 1);
    float4 pixelWorldPos = mul(clipSpaceCoord, viewProjInv);
    pixelWorldPos /= pixelWorldPos.w;

    PBRInput pbr_input;
    pbr_input.albedo = albedo;
    pbr_input.micronormal = micronormal;
    pbr_input.macronormal = macronormal;
    pbr_input.roughness = roughness;
    pbr_input.metalness = metalness;
    pbr_input.pixelWorldPos = pixelWorldPos;
    return CalculatePBR(pbr_input);
}