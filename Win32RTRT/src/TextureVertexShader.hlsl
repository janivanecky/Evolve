struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 texcoord: TEXCOORD;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color: COLOR;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);
	output.pos = pos;
	output.texcoord = input.texcoord;
	output.color = float4(1,1,1,1);
	return output;
}
