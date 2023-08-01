
uint selectCubeFace(float3 unitDir)
{
    float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
    uint maxIndex = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
    return maxIndex + (asuint(unitDir[maxIndex / 2]) >> 31);
}