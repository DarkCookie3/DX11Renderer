#include "../LightSources/LightSourceTextureMask.hlsli"

float CalculateAttenuation(float distance, float attenuationConstant, float attenuationLinear, float attenuationQuadratic)
{
    return 1.0 / (attenuationConstant + attenuationLinear * distance + attenuationQuadratic * distance * distance);
}

float3 CalculateBlinnPhongLighting(float3 lightDirection, float3 viewDirection, float3 surfaceNormal, 
                                    float3 surfaceDiffColor, float3 surfaceSpecColor, float shininess, float ambientFactor)
{
    float3 halfVector = normalize(lightDirection + viewDirection);
    float diffuse = saturate(dot(surfaceNormal, lightDirection));
    float specular = pow(saturate(dot(surfaceNormal, halfVector)), shininess);

    return (ambientFactor * surfaceDiffColor) + (surfaceDiffColor * diffuse) + (specular * surfaceSpecColor);
}

float3 CalculatePointLightBlinnPhong(PointLight light, float3 surfacePosition, float3 surfaceNormal,
                                        float3 surfaceDiffColor, float3 surfaceSpecColor, float shininess)
{
    float3 lightDirection = normalize(light.position - surfacePosition);
    float3 viewDirection = normalize(camera_pos - surfacePosition);
    return light.color * light.intensity * CalculateAttenuation(light.position - surfacePosition, 1.0, 0.1, 0.01) * CalculateBlinnPhongLighting(lightDirection, viewDirection, surfaceNormal,
                                                        surfaceDiffColor, surfaceSpecColor, shininess, light.ambientFactor);
}

float3 CalculateDirLightBlinnPhong(DirLight light, float3 surfacePosition, float3 surfaceNormal, 
                                            float3 surfaceDiffColor, float3 surfaceSpecColor, float shininess)
{
    float3 lightDirection = normalize(-light.direction);
    float3 viewDirection = normalize(camera_pos - surfacePosition);
    return light.color * light.intensity * CalculateBlinnPhongLighting(lightDirection, viewDirection, surfaceNormal,
                                                        surfaceDiffColor, surfaceSpecColor, shininess, light.ambientFactor);
}

float3 CalculateSpotLightBlinnPhong(SpotLight light, float3 surfacePosition, float3 surfaceNormal,
                                            float3 surfaceDiffColor, float3 surfaceSpecColor, float shininess)
{
    float3 lightDirection = normalize(light.position - surfacePosition);
    float3 viewDirection = normalize(camera_pos - surfacePosition);
    float coneAngle = dot(-lightDirection, normalize(light.direction));
    float3 color = float3(0, 0, 0);
    float outerAngle = light.lightAngleRad + light.fadeAngleRad;
    if (coneAngle > cos(outerAngle))
    {
        float coneFactor = saturate((coneAngle - cos(outerAngle)) / (cos(light.lightAngleRad) - cos(outerAngle)));
        color = light.color * light.intensity * coneFactor;
    }
    return color * CalculateTextureMaskTransmittance(light, surfacePosition) * CalculateAttenuation(camera_pos - surfacePosition, 1.0, 0.1, 0.01) * CalculateBlinnPhongLighting(lightDirection, viewDirection, surfaceNormal,
                                                        surfaceDiffColor, surfaceSpecColor, shininess, light.ambientFactor);
}

float3 CalculateTotalBlinnPhongColor(float3 surfacePosition, float3 surfaceNormal,
                                            float3 surfaceDiffColor, float3 surfaceSpecColor, float shininess)
{
    float3 color = float3(0, 0, 0);

    for (int i = 0; i < pointLightNum; i++)
    {
        color += CalculatePointLightBlinnPhong(g_pointLights[i], surfacePosition, surfaceNormal, surfaceDiffColor, surfaceSpecColor, shininess);
    }

    for (int j = 0; j < spotLightNum; j++)
    {
        color += CalculateSpotLightBlinnPhong(g_spotLights[j], surfacePosition, surfaceNormal, surfaceDiffColor, surfaceSpecColor, shininess);
    }

    for (int k = 0; k < dirLightNum; k++)
    {
        color += CalculateDirLightBlinnPhong(g_dirLights[k], surfacePosition, surfaceNormal, surfaceDiffColor, surfaceSpecColor, shininess);
    }
    return color;
}