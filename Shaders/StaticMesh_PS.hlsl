Texture2DArray staticMeshTextures : register(t2);
SamplerState samLinear : register(s0);

cbuffer MaterialConstants : register(b3)
{
	int MaterialIndex;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float3 Normal : NORMAL;
};


float4 StaticMesh_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 sampledColor = staticMeshTextures.Sample(samLinear, float3(input.Texcoord, MaterialIndex));
	return sampledColor;
}