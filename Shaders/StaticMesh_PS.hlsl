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

cbuffer FConstantsComponentData : register(b0)
{
	float4x4 MVP;
	float4 CustomColor;
	float4 UUIDColor;
	uint bUseVertexColor;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	uint4 UUID : SV_Target1;
};

PS_OUTPUT StaticMesh_PS(VS_OUTPUT input) : SV_TARGET
{
	PS_OUTPUT output;
  float4 sampledColor = staticMeshTextures.Sample(samLinear, float3(input.Texcoord, (float) MaterialIndex));	
	output.color = bUseVertexColor == true ? sampledColor : input.Color;
	output.UUID = UUIDColor;
	
	return output;
}