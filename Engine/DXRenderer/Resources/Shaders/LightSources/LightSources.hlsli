
struct PointLight
{
    row_major float4x4 lightViewProj[6];
    float4 position;
    float4 color;
    float radius;
    float intensity;
    int shadowMapIndex;
    float padding;
};

struct SpotLight
{
    row_major float4x4 lightViewProj;
    row_major float4x4 localToWorld;
    row_major float4x4 worldToLocal;
    float4 position;
    float4 direction;
    float4 color;
    float lightAngleRad;
    float fadeAngleRad;
    float radius;
    float intensity;
    int texArraySliceIndex;
    float2 texCoordScale;
    int shadowMapIndex;
};

struct DirLight
{
    row_major float4x4 lightViewProj;
    float4 direction;
    float4 color;
    float intensity;
    int shadowMapIndex;
    float2 padding;
};

StructuredBuffer<PointLight> g_pointLights : register(t8);
StructuredBuffer<SpotLight> g_spotLights : register(t9);
StructuredBuffer<DirLight> g_dirLights : register(t10);

Texture2DArray<float4> g_spotLightTextureMasks : register(t11);

Texture2DArray<float1> g_dirShadowMaps : register(t15);
TextureCubeArray<float1> g_pointShadowMaps : register(t16);
Texture2DArray<float1> g_spotShadowMaps : register(t17);