Texture2D staticMeshTexture : register(t2);
SamplerState samLinear : register(s0);

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float3 Normal : NORMAL;
};


float4 StaticMesh_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 sampledColor = staticMeshTexture.Sample(samLinear, input.Texcoord);
	return sampledColor;
}