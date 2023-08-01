#include "../LightSources/LightSources.hlsli"

Texture2DArray<float4> g_diffuseTextureArray : register(t0);
Texture2DArray<float4> g_normalTextureArray : register(t1);
Texture2DArray<float4> g_roughnessTextureArray : register(t2);
Texture2DArray<float4> g_metalnessTextureArray : register(t3);
Texture2DArray<float4> g_emissionTextureArray : register(t4);
Texture2DArray<float4> g_heightTextureArray : register(t5);
Texture2DArray<float4> g_opacityTextureArray : register(t6);
Texture2DArray<float4> g_ambientOcclusionTextureArray : register(t7);

//8-11 slots for light sources

TextureCube<float4> g_skycubeDiffuseIrradiance : register(t12);
TextureCube<float4> g_skycubeSpecularIrradiance : register(t13);
Texture2D<float2> g_skycubeSpecularBRDF : register(t14);

//15-17 slots for shadowmaps

Texture2D<float1> g_noiseMap : register(t18);