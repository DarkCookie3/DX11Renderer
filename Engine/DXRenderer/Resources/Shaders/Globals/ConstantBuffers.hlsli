
cbuffer CONSTANT_BUFFER_PER_TEXTURE_STRUCT : register(b3)
{
    int inputWidth;
    int inputHeight;
    float texelSize;
    float roughness_gen;
};

cbuffer CONSTANT_BUFFER_ADDITIONAL_PER_VIEW_STRUCT : register(b2)
{
    row_major float4x4 viewProjCubeSides[6];
};

cbuffer CONSTANT_BUFFER_PER_VIEW_STRUCT : register(b1)
{
    row_major float4x4 viewProj;
    row_major float4x4 viewProjInv;
    row_major float4x4 viewInv;
    row_major float4x4 projInv;
    row_major float4x4 view;
    row_major float4x4 proj;
    float4 camera_pos;
};

cbuffer CONSTANT_BUFFER_PER_FRAME_STRUCT : register(b0)
{
    float g_time;
    float EV100;
    int pointLightNum;
    int spotLightNum;
    int dirLightNum;
    bool enableDiffuseIrradiance;
    bool enableSpecularIrradiance;
    bool overwriteRoughness;
    bool overwriteMetalness;
    float roughnessOverwriteValue;
    float metalnessOverwriteValue;
    int padding;
}
