#include "../Globals/ConstantBuffers.hlsli"

struct GS_INPUT
{
    float4 position : POSITION;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GShader(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
	[unroll]
    for (int face = 0; face < 6; face++)
    {
        GS_OUTPUT output;
        output.RTIndex = face;
        
        [unroll]
        for (int i = 0; i < 3; i++)
        {
            output.position = mul(input[i].position, viewProjCubeSides[face]);
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}