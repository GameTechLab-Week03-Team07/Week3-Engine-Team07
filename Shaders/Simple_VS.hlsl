cbuffer FConstantsComponentData : register(b0)
{
	float4x4 MVP;
	float4 CustomColor;
	float4 UUIDColor;
	uint bUseVertexColor;
};

struct VS_INPUT
{
	float3 position : POSITION; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
	float4 color : COLOR; // 4개의 float 값 (DXGI_FORMAT_R32G32B32A32_FLOAT)
	float2 texcoord : TEXCOORD; // 2개의 float 값 (DXGI_FORMAT_R32G32_FLOAT)
	float3 normal : NORMAL; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
};

struct PS_INPUT
{
	float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
	float4 color : COLOR; // Color to pass to the pixel shader
    // float4 depthPosition : TEXCOORD0;
};

PS_INPUT Simple_VS(VS_INPUT input)
{
	PS_INPUT output;

	output.position = mul(float4(input.position.xyz, 1), MVP);
    // output.depthPosition = output.position;

	output.color = bUseVertexColor == true ? input.color : CustomColor;
	return output;
}