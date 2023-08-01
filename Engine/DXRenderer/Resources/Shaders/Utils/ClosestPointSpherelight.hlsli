
float3 ClosestSphereDir(float3 viewDir, float3 normal, float3 lightDir, float radiusCos, float radiusSin)
{
    float3 reflected = reflect(-viewDir, normal);
    float RoL = dot(reflected, lightDir);
    
    if(RoL >= radiusCos)
    {
        return reflected;
    }
    float3 vertical = normalize(cross(reflected, lightDir));
    float3 horizontal = normalize(cross(lightDir, vertical));
    
    return normalize(radiusCos * lightDir + radiusSin * horizontal);
}