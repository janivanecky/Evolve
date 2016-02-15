struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 texcoord: TEXCOORD;
	float4 color: COLOR;
};

Texture2D screenTexture;
SamplerState textureSampler;

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = screenTexture.Sample(textureSampler, input.texcoord) * input.color;
	return float4(pow(color.xyz, 1.0 / 2.2), color.a);  
}
