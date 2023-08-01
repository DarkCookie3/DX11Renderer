#include "ParticleShaderResources.hlsli"
#include "../LightSources/LightSourceTextureMask.hlsli"
#include "../Utils/ConstantValues.hlsli"
#include "../Utils/SelectCubeFace.hlsli"


static const float smoothClippingFactor = 10.0f;

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 pixelWorldPos : PIXEL_WORLD_POS;
    float2 texCoord : TEXCOORD;
    float4 ColorRGBA : COLOR_RGBA;
    float4 emissionColor : COLOR_EMISSION;
    float animationState : ANIMATION_STATE;
};

float3 CalculateParticleColor(float3 RLU, float3 DBF, float3 lightDir, float3 lightColor, float irradiance, float3 frontDir, float3 upDir, float3 rightDir)
{
    float3 resultColor = float3(0, 0, 0);
    
    for (int dbfSide = 0; dbfSide < 3; dbfSide++)
    {
        float3 basis = ((dbfSide != 0) * upDir - upDir) + (frontDir - 2 * frontDir * (dbfSide == 1));
        resultColor += irradiance * lightColor * DBF[dbfSide] * max(0, dot(basis, lightDir));
    }
    for (int rluSide = 0; rluSide < 3; rluSide++)
    {
        float3 basis = (upDir - (rluSide != 2) * upDir) + (rightDir - 2 * rightDir * (rluSide == 1));
        resultColor += irradiance * lightColor * RLU[rluSide] * max(0, dot(basis, lightDir));
    }
    
    return resultColor;
}

float4 PShader(PSInput input) : SV_TARGET
{
    float4 resultColor = float4(0, 0, 0, 0);
    
    int currentAnimationFrameNum = trunc(input.animationState * 64);
    int nextAnimationFrameNum = currentAnimationFrameNum + 1;
    float time = fmod(input.animationState, 1.0 / 64) * 64;
    
    float2 uvThis = (float2((currentAnimationFrameNum % 8), (currentAnimationFrameNum / 8)) + input.texCoord) / 8.0;
    float2 uvNext = (float2((nextAnimationFrameNum % 8), (nextAnimationFrameNum / 8)) + input.texCoord) / 8.0;
    
    float2 mv0 = 2.0 * g_smokeEMVA.Sample(g_pointWrap, uvThis).gb - 1.0;
    float2 mv1 = 2.0 * g_smokeEMVA.Sample(g_pointWrap, uvNext).gb - 1.0;
    
    mv0.y = -mv0.y;
    mv1.y = -mv1.y;
    
    static const float MV_SCALE = 0.0015;
    float2 uv0 = uvThis;
    uv0 -= mv0 * MV_SCALE * time;

    float2 uv1 = uvNext;
    uv1 -= mv1 * MV_SCALE * (time - 1.f);
    
    float2 emissionAlpha0 = g_smokeEMVA.Sample(g_pointWrap, uv0).ra;
    float2 emissionAlpha1 = g_smokeEMVA.Sample(g_pointWrap, uv1).ra;

    float3 lightmapRLU0 = g_smokeRLU.Sample(g_pointWrap, uv0).rgb;
    float3 lightmapRLU1 = g_smokeRLU.Sample(g_pointWrap, uv1).rgb;

    float3 lightmapDBF0 = g_smokeDBF.Sample(g_pointWrap, uv0).rgb;
    float3 lightmapDBF1 = g_smokeDBF.Sample(g_pointWrap, uv1).rgb;

    float2 emissionAlpha = lerp(emissionAlpha0, emissionAlpha1, time);
    float3 lightmapRLU = lerp(lightmapRLU0, lightmapRLU1, time);
    float3 lightmapDBF = lerp(lightmapDBF0, lightmapDBF1, time);
    
    float3 rightDir = -viewInv[0].xyz;
    float3 upDir = viewInv[1].xyz;
    float3 frontDir = -viewInv[2].xyz;
    
    for (int i = 0; i < pointLightNum; i++)
    {
        float3 lightDirection = normalize(g_pointLights[i].position - input.pixelWorldPos);
        float dist = distance(g_pointLights[i].position, input.pixelWorldPos);
        float solidAngle = 2 * PI * (1 - sqrt(1 - pow(min(g_pointLights[i].radius / dist, 1), 2)));
        
        uint cubeSideIndex = selectCubeFace(-lightDirection);
        float4 lightSpacePos = mul(input.pixelWorldPos, g_pointLights[i].lightViewProj[cubeSideIndex]);
        lightSpacePos /= lightSpacePos.w;
        float shadowed = g_pointShadowMaps.SampleCmp(g_shadowSampler, float4(-lightDirection, g_pointLights[i].shadowMapIndex), lightSpacePos.z + 0.001);
        
        resultColor.xyz += shadowed * CalculateParticleColor(lightmapRLU, lightmapDBF, lightDirection, g_pointLights[i].color, g_pointLights[i].intensity * solidAngle, frontDir, upDir, rightDir);
    }
    for (int j = 0; j < spotLightNum; j++)
    {
        float3 lightDirection = normalize(g_spotLights[j].position - input.pixelWorldPos);
        
        float coneAngle = dot(-lightDirection, normalize(g_spotLights[j].direction));
        float3 color = float3(0, 0, 0);
        float outerAngle = g_spotLights[j].lightAngleRad + g_spotLights[j].fadeAngleRad;
        if (coneAngle > cos(outerAngle))
        {
            float dist = distance(g_spotLights[j].position, input.pixelWorldPos);
            float solidAngle = 2 * PI * (1 - sqrt(1 - pow(min(g_spotLights[j].radius / dist, 1), 2)));
            
            float4 lightSpacePos = mul(input.pixelWorldPos, g_spotLights[j].lightViewProj);
            lightSpacePos /= lightSpacePos.w;
            float shadowed = g_spotShadowMaps.SampleCmp(g_shadowSampler, float3((float2(lightSpacePos.x, -lightSpacePos.y) + 1) / 2, g_spotLights[j].shadowMapIndex), lightSpacePos.z);
            
            resultColor.xyz += shadowed * CalculateParticleColor(lightmapRLU, lightmapDBF, lightDirection, CalculateTextureMaskTransmittance(g_spotLights[j], input.pixelWorldPos), g_spotLights[j].intensity * solidAngle, frontDir, upDir, rightDir);
        }
    }
    for (int k = 0; k < dirLightNum; k++)
    {
        float3 lightDirection = g_dirLights[k].direction;
        
        float4 lightSpacePos = mul(input.pixelWorldPos, g_dirLights[k].lightViewProj);
        lightSpacePos /= lightSpacePos.w;
        float shadowed = g_dirShadowMaps.SampleCmp(g_shadowSampler, float3((float2(lightSpacePos.x, -lightSpacePos.y) + 1) / 2, g_dirLights[k].shadowMapIndex), lightSpacePos.z);
        
        resultColor.xyz += shadowed * CalculateParticleColor(lightmapRLU, lightmapDBF, lightDirection, g_dirLights[k].color, g_dirLights[k].intensity, frontDir, upDir, rightDir);
    }
    
    resultColor.xyz += emissionAlpha.r * input.emissionColor.xyz;
    resultColor.a = input.ColorRGBA.a;
    resultColor.a *= saturate((input.Position.z - g_depthTexture.Load(input.Position)) / (smoothClippingFactor * input.Position.z * input.Position.z));
    resultColor.a *= emissionAlpha.g;
    return resultColor;
}
