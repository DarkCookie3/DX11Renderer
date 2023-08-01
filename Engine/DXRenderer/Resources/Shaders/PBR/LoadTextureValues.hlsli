
float3 LoadAlbedo(float4 texCoord, float2 texCoordScale, float3 defaultValue, int texArrIndex)
{
    float3 result;
    if (texArrIndex >= 0)
    {
        result = g_diffuseTextureArray.Sample(g_anisotropicWrap, float3(texCoord.x * texCoordScale.x, texCoord.y * texCoordScale.y, texArrIndex));
    }
    else
    {
        result = defaultValue;
    }
    return result;
}

float3 LoadNormal(float4 texCoord, float2 texCoordScale, float3 defaultValue, int texArrIndex)
{
    float3 result;
    if (texArrIndex >= 0)
    {
        result = g_normalTextureArray.Sample(g_anisotropicWrap, float3(texCoord.x * texCoordScale.x, texCoord.y * texCoordScale.y, texArrIndex));
        result.xy = result.xy * float2(2.0f, 2.0f) - float2(1.0f, 1.0f);
        result.z = sqrt(saturate(1.0f - dot(result.xy, result.xy)));
    }
    else
    {
        result = defaultValue;
    }
    return result;
}

float LoadRoughness(float4 texCoord, float2 texCoordScale, float defaultValue, int texArrIndex)
{
    float3 result;
    if (texArrIndex >= 0)
    {
        result = g_roughnessTextureArray.Sample(g_anisotropicWrap, float3(texCoord.x * texCoordScale.x, texCoord.y * texCoordScale.y, texArrIndex));
    }
    else
    {
        result = defaultValue;
    }
    return result;
}

float LoadMetalness(float4 texCoord, float2 texCoordScale, float defaultValue, int texArrIndex)
{
    float result;
    if (texArrIndex >= 0)
    {
        result = g_metalnessTextureArray.Sample(g_anisotropicWrap, float3(texCoord.x * texCoordScale.x, texCoord.y * texCoordScale.y, texArrIndex));
    }
    else
    {
        result = defaultValue;
    }
    return result;
}

float3 LoadEmission(float4 texCoord, float2 texCoordScale, float3 defaultValue, int texArrIndex)
{
    float3 result;
    if (texArrIndex >= 0)
    {
        result = g_emissionTextureArray.Sample(g_anisotropicWrap, float3(texCoord.x * texCoordScale.x, texCoord.y * texCoordScale.y, texArrIndex));
    }
    else
    {
        result = defaultValue;
    }
    return result;
}