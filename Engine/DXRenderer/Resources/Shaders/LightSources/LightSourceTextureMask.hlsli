#include "../Globals/Samplers.hlsli"
#include "../Globals/ConstantBuffers.hlsli"
#include "../Utils/Map.hlsli"

float3 CalculateTextureMaskTransmittance(SpotLight light, float3 pixelPos)
{
    float3 resultColor;
    if(light.texArraySliceIndex >= 0)
    {
        float3 pixelPosLightSpace = mul(float4(pixelPos, 1.0f), light.worldToLocal);
        float radialLength = pixelPosLightSpace.z * tan(light.lightAngleRad + light.fadeAngleRad);

        float u = map(pixelPosLightSpace.x, -radialLength, radialLength, 0.0f, 1.0f);
        float v = map(pixelPosLightSpace.y, -radialLength, radialLength, 0.0f, 1.0f);
        resultColor = g_spotLightTextureMasks.Sample(g_pointWrap, float3(u * light.texCoordScale.x, -v * light.texCoordScale.y, light.texArraySliceIndex));
    }
    else
    {
        resultColor = float3(1, 1, 1);
    }
    return resultColor;
}