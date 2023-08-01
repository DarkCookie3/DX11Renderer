
Texture2D<float2> g_depthTexture : register(t0);

struct PSInput
{
    float4 Position : SV_POSITION;
    float4 texCoord : TEXCOORD;
};

float4 PShader(PSInput input) : SV_TARGET
{
    float2 color = g_depthTexture.Load(input.Position);
    return float4(color.x, 0, 0, 1);
}