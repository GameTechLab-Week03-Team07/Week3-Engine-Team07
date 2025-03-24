struct VS_INPUT
{
	float3 Position : POSITION; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
	float4 Color : COLOR; // 4개의 float 값 (DXGI_FORMAT_R32G32B32A32_FLOAT)
	float2 Texcoord : TEXCOORD; // 2개의 float 값 (DXGI_FORMAT_R32G32_FLOAT)
	float3 Normal : NORMAL; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float3 Normal : NORMAL;
};

cbuffer constants : register(b0)
{
	matrix MVP;
	float4 CustomColor;
	float4 UUIDColor;
	uint bUseVertexColor;
}

cbuffer Depth : register(b2)
{
	int depth;
	int nearPlane;
	int farPlane;
}

VS_OUTPUT StaticMesh_VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Position = mul(float4(input.Position.xyz, 1.0f), MVP);
	output.Texcoord = input.Texcoord;
	output.Normal = input.Normal;
	output.Color = bUseVertexColor == true ? input.Color : CustomColor;

	return output;
}