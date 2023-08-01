
float randomVanDeCorput(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 randomHammersley(uint i, uint N)
{
    return float2(float(i) / float(N), randomVanDeCorput(i));
}

float3 randomGGX(float2 random, float roughness)
{
    float alpha = roughness * roughness;

    float cosTheta = sqrt((1.0 - random.y) / (1.0 + (alpha * alpha - 1.0) * random.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi = 2 * PI * random.x;
        
    return normalize(float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta));
}

float3 randomGGX(out float NdotH, uint index, uint N, float roughness, float3x3 rotation)
{
    float3 H = randomGGX(randomHammersley(index, N), roughness);
    NdotH = H.z;
    return mul(H, rotation);
}
