
float3 randomHemisphere(out float NdotV, float i, float N)
{
    const float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;
    float theta = 2.0 * PI * i / GOLDEN_RATIO;
    float phiCos = NdotV = 1.0 - (i + 0.5) / N;
    float phiSin = sqrt(1.0 - phiCos * phiCos);
    float thetaCos, thetaSin;
    sincos(theta, thetaSin, thetaCos);
    return float3(thetaCos * phiSin, thetaSin * phiSin, phiCos);
}