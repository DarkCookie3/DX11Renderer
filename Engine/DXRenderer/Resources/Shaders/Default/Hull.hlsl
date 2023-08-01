
struct HS_INPUT
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

struct HS_OUTPUT
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

struct PatchOut // 3 outer factors and 1 inner factor specifically for “tri” domain
{
    float EdgeFactors[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};


PatchOut mainPatch(InputPatch<HS_INPUT, 3> input, uint patchId : SV_PrimitiveID)
{
    PatchOut output;
    output.EdgeFactors[0] = ceil(distance(input[0].position, input[1].position) * 10);
    output.EdgeFactors[1] = ceil(distance(input[1].position, input[2].position) * 10);
    output.EdgeFactors[2] = ceil(distance(input[2].position, input[0].position) * 10);
    output.InsideFactor = max(max(output.EdgeFactors[0], output.EdgeFactors[1]), output.EdgeFactors[2]);
    return output;
}


[outputcontrolpoints(3)] // doesn’t have to match InputPatch argument size (3)
[domain("tri")]
[outputtopology("triangle_cw")]
[partitioning("integer")]
[patchconstantfunc("mainPatch")]
HS_OUTPUT HShader(InputPatch<HS_INPUT, 3> input,
 uint pointId : SV_OutputControlPointID,
 uint patchId : SV_PrimitiveID)
{
    return input[pointId]; // input size matches the number specified in the set PATCHLIST Topology
}
