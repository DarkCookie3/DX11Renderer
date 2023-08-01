#include "DecalShaderResources.hlsli"
#include "../../Globals/ConstantBuffers.hlsli"
#include "../../Globals/Samplers.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 screenSpacePos : SCREEN_SPACE_POS;
    row_major float4x4 worldToLocal : INSTANCE_MODEL_MATRIX_INV;
};

float4 PShader(PSInput input) : SV_TARGET
{
    float sceneDepth = m_depthStencil.Load(input.position);
    
    float4 decalSpacePos = mul(float4(input.screenSpacePos, sceneDepth, 1), viewProjInv);
    decalSpacePos /= decalSpacePos.w;
    decalSpacePos = mul(decalSpacePos, input.worldToLocal);
    
    if (abs(decalSpacePos.x) > 1 || abs(decalSpacePos.y) > 1 || abs(decalSpacePos.z) > 1)
    {
        discard;
    }

    float4 color = m_albedo.Sample(g_pointWrap, decalSpacePos.xy / 2 + 0.5);
    if (color.a <= 0.1)
    {
        discard;
    }
    return color;
}