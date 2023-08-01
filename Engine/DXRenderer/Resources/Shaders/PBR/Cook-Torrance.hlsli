#include "../Utils/ConstantValues.hlsli"

float3 fresnel(float NorHdotL, float3 F0)
{
    return F0 + (1 - F0) * pow(1 - NorHdotL, 5);
}

float smith(float rough2, float NdotV, float NdotL)
{
    NdotV *= NdotV;
    NdotL *= NdotL;
    return 2.0 / (sqrt(1 + rough2 * (1 - NdotV) / NdotV) + sqrt(1 + rough2 * (1 - NdotL) / NdotL));
}

float ggx(float rough2, float NdotH)
{
    float denom = NdotH * NdotH * (rough2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    return rough2 / denom;
}