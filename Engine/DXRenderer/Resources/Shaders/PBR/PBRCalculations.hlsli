
float3 PBR(float3 lightDir, float3 viewDir, float3 lightColor, float3 baseReflectivity, float3 albedo, float3 normal, float roughness, float metalness, float radiusSin)
{
    float3 H = normalize(lightDir + viewDir);
    float NdotH = dot(normal, H);
    float NdotL = dot(normal, lightDir);
    float VdotL = dot(viewDir, lightDir);
    float VdotH = dot(viewDir, H);
    float HdotL = dot(H, lightDir);
    float NdotV = dot(normal, viewDir);
    float rough2 = pow(roughness, 2);
    
    NdotH = max(NdotH, 0.0);
    NdotV = max(NdotV, 0.0);
    NdotL = max(NdotL, 0.0);
    
    float3 Ks = fresnel(HdotL, baseReflectivity);
    float3 Kd = (float3(1.0, 1.0, 1.0) - fresnel(NdotL, baseReflectivity)) * (1.0f - metalness);
    
    float3 lambert = albedo / PI;
    
    float cookTorranceNumerator = ggx(rough2, NdotH) * smith(rough2, NdotV, NdotL);
    float cookTorranceDenominator = max(4 * NdotL * NdotV, 0.0001);
    float cookTorrance = cookTorranceNumerator / cookTorranceDenominator;
    
    float3 BRDF = Kd * lambert + Ks * cookTorrance;
    float3 outgoingLight = BRDF * lightColor * max(NdotL, 0.005f);

    return outgoingLight;
}

uint querySpecularTextureLevels()
{
    uint width, height, levels;
    g_skycubeSpecularIrradiance.GetDimensions(0, width, height, levels);
    return levels;
}

struct PBRInput
{
    float3 albedo;
    float3 macronormal;
    float3 micronormal;
    float roughness;
    float metalness;
    float4 pixelWorldPos;
};

float4 CalculatePBR(PBRInput input)
{
    float3 baseReflectivity = lerp(0.04, input.albedo, input.metalness);
    
    float3 viewDir = normalize(camera_pos - input.pixelWorldPos);
    
    float3 resultColor = float3(0.0f, 0.0f, 0.0f);
    
    float coordOffset = 1.0f / 1024.0f;
    
    [loop]
    for (int i = 0; i < pointLightNum; i++)
    {
        float3 lightColor = g_pointLights[i].color;
        float3 lightDir = normalize(g_pointLights[i].position - input.pixelWorldPos);
        
        float dist = distance(g_pointLights[i].position, input.pixelWorldPos);
        float solidAngle = 2 * PI * (1 - sqrt(1 - pow(min(g_pointLights[i].radius / dist, 1), 2)));
        
        float radiusSin = g_pointLights[i].radius / sqrt(g_pointLights[i].radius * g_pointLights[i].radius + dist * dist);
        float HforFalloffMicro = dot(input.micronormal, g_pointLights[i].position - input.pixelWorldPos);
        float HforFalloffMacro = dot(input.macronormal, g_pointLights[i].position - input.pixelWorldPos);
        float falloffMicro = min(1, (HforFalloffMicro + g_pointLights[i].radius) / (2 * g_pointLights[i].radius));
        float falloffMacro = min(1, (HforFalloffMacro + g_pointLights[i].radius) / (2 * g_pointLights[i].radius));
        
        uint cubeSideIndex = selectCubeFace(-lightDir);
        float4 lightSpacePos = mul(input.pixelWorldPos, g_pointLights[i].lightViewProj[cubeSideIndex]);
        lightSpacePos /= lightSpacePos.w;
        float shadowed = g_pointShadowMaps.SampleCmp(g_shadowSampler, float4(-lightDir, g_pointLights[i].shadowMapIndex), lightSpacePos.z + 0.001);
        
        lightDir = ClosestSphereDir(viewDir, input.micronormal, lightDir, sqrt(1 - radiusSin * radiusSin), radiusSin);
        
        resultColor += shadowed * g_pointLights[i].intensity * min(falloffMacro, falloffMicro) * solidAngle * PBR(lightDir, viewDir, lightColor, baseReflectivity, input.albedo, input.micronormal, input.roughness, input.metalness, radiusSin);
    }
    
    [loop]
    for (int j = 0; j < spotLightNum; j++)
    {
        float3 lightDir = normalize(g_spotLights[j].position - input.pixelWorldPos);
        float coneAngle = dot(-lightDir, normalize(g_spotLights[j].direction));
        float3 color = float3(0, 0, 0);
        float outerAngle = g_spotLights[j].lightAngleRad + g_spotLights[j].fadeAngleRad;
        if (coneAngle > cos(outerAngle))
        {
            float coneFactor = saturate((coneAngle - cos(outerAngle)) / (cos(g_spotLights[j].lightAngleRad) - cos(outerAngle)));
            float dist = distance(g_spotLights[j].position, input.pixelWorldPos);
            float solidAngle = 2 * PI * (1 - sqrt(1 - pow(min(g_spotLights[j].radius / dist, 1), 2)));
            
            float radiusSin = g_spotLights[j].radius / sqrt(g_spotLights[j].radius * g_spotLights[j].radius + dist * dist);
            float HforFalloffMicro = dot(input.micronormal, g_spotLights[j].position - input.pixelWorldPos);
            float HforFalloffMacro = dot(input.macronormal, g_spotLights[j].position - input.pixelWorldPos);
            float falloffMicro = min(1, (HforFalloffMicro + g_spotLights[j].radius) / (2 * g_spotLights[j].radius));
            float falloffMacro = min(1, (HforFalloffMacro + g_spotLights[j].radius) / (2 * g_spotLights[j].radius));
            
            float4 lightSpacePos = mul(input.pixelWorldPos, g_spotLights[j].lightViewProj);
            lightSpacePos /= lightSpacePos.w;
            float shadowed = g_spotShadowMaps.SampleCmp(g_shadowSampler, float3((float2(lightSpacePos.x, -lightSpacePos.y) + 1) / 2, g_spotLights[j].shadowMapIndex), lightSpacePos.z);
            
            lightDir = ClosestSphereDir(viewDir, input.micronormal, lightDir, sqrt(1 - radiusSin * radiusSin), radiusSin);
            
            float3 lightColor = coneFactor * g_spotLights[j].color * CalculateTextureMaskTransmittance(g_spotLights[j], input.pixelWorldPos);
            resultColor += shadowed * g_spotLights[j].intensity * min(falloffMacro, falloffMicro) * solidAngle * PBR(lightDir, viewDir, lightColor, baseReflectivity, input.albedo, input.micronormal, input.roughness, input.metalness, radiusSin);
        }
    }
    
    [loop]
    for (int k = 0; k < dirLightNum; k++)
    {
        float3 lightColor = g_dirLights[k].color;
        float3 lightDir = normalize(-g_dirLights[k].direction);

        float4 lightSpacePos = mul(input.pixelWorldPos, g_dirLights[k].lightViewProj);
        lightSpacePos /= lightSpacePos.w;
        float shadowed = 0;
        
        for (int m = -1; m <= 1; m++)
        {
            for (int n = -1; n <= 1; n++)
            {
                shadowed += g_dirShadowMaps.SampleCmp(g_shadowSampler, float3((float2(lightSpacePos.x + m * coordOffset, -lightSpacePos.y + n * coordOffset) + 1) / 2, g_dirLights[k].shadowMapIndex), lightSpacePos.z);
            }
        }
        shadowed /= 9;
        shadowed = smoothstep(0.33, 1, shadowed);

        resultColor += shadowed * g_dirLights[k].intensity * PBR(lightDir, viewDir, lightColor, baseReflectivity, input.albedo, input.micronormal, input.roughness, input.metalness, 0.0);
    }
    
    float NdotV = max(dot(input.micronormal, viewDir), 0.001);
    
    float3 ambientLighting = float3(0.0, 0.0, 0.0);
	{
        if (enableDiffuseIrradiance)
        {
            float3 irradiance = g_skycubeDiffuseIrradiance.Sample(g_anisotropicWrap, input.micronormal).rgb;

            float3 kd = (float3(1.0, 1.0, 1.0) - fresnel(NdotV, baseReflectivity)) * (1.0f - input.metalness);
            ambientLighting += kd * input.albedo * irradiance;
        }
        if (enableSpecularIrradiance)
        {
            float cosLo = max(0.001, dot(input.micronormal, viewDir));
            float3 Lr = 2.0 * cosLo * input.micronormal - viewDir;
        
            uint specularTextureLevels = querySpecularTextureLevels();
            float3 specularIrradiance = g_skycubeSpecularIrradiance.SampleLevel(g_anisotropicWrap, Lr, input.roughness * specularTextureLevels).rgb;

            float2 specularBRDF = g_skycubeSpecularBRDF.Sample(g_pointWrap, float2(cosLo, 1.0 - input.roughness));

            ambientLighting += (baseReflectivity * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        }
    }
    
    return float4(resultColor + ambientLighting, 1.0f);
}