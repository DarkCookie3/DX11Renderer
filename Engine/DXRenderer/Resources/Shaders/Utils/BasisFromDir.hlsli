
void basisFromDir(out float3 S, out float3 T, in float3 N)
{
    float k = 1.0 / max(1.0 + N.z, 0.00001);
    float a = N.y * k;
    float b = N.y * a;
    float c = -N.x * a;
    S = float3(N.z + b, c, -N.x);
    T = float3(c, 1.0 - b, -N.y);
}