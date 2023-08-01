#include "../LightSources/LightSources.hlsli"

Texture2D<float4> g_smokeEMVA : register(t0);
Texture2D<float4> g_smokeDBF : register(t1);
Texture2D<float4> g_smokeRLU : register(t2);

Texture2D<float> g_depthTexture : register(t3);