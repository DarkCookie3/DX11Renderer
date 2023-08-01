
struct PatchConstants
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactors : SV_InsideTessFactor;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    row_major float4x4 instanceMatrix : INSTANCE_MODEL_MATRIX;
    row_major float4x4 instanceMatrixInversed : INSTANCE_MODEL_MATRIX_INV;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

struct DS_OUTPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 bitangent : BITANGENT;
    float4 texCoord : TEXCOORD;
    nointerpolation float4 color : COLOR;
    row_major float4x4 instanceMatrix : INSTANCE_MODEL_MATRIX;
    row_major float4x4 instanceMatrixInversed : INSTANCE_MODEL_MATRIX_INV;
    int texArrIndex[8] : TEXARR_INDEX;
    float2 texCoordScale : TEX_COORD_SCALE;
};

[domain("tri")]
DS_OUTPUT DShader(PatchConstants patchConstants, float3 loc : SV_DomainLocation,
               const OutputPatch<DS_INPUT, 3> input,
               float edgeFactors[3] : SV_TessFactor,
               float insideFactors : SV_InsideTessFactor)
{
    DS_OUTPUT result;
    result.position = loc.x * input[0].position +
                      loc.y * input[1].position +
                      loc.z * input[2].position;
    result.normal = loc.x * input[0].normal +
                      loc.y * input[1].normal +
                      loc.z * input[2].normal;
    result.tangent = loc.x * input[0].tangent +
                      loc.y * input[1].tangent +
                      loc.z * input[2].tangent;
    result.bitangent = loc.x * input[0].bitangent +
                      loc.y * input[1].bitangent +
                      loc.z * input[2].bitangent;
    result.color = input[0].color;
    result.instanceMatrix = input[0].instanceMatrix;
    result.instanceMatrixInversed = input[0].instanceMatrixInversed;
    result.texArrIndex = input[0].texArrIndex;
    result.texCoordScale = input[0].texCoordScale;
    result.texCoord = loc.x * input[0].texCoord +
                      loc.y * input[1].texCoord +
                      loc.z * input[2].texCoord;
    return result;
}
