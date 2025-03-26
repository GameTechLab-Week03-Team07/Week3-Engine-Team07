Texture2DArray staticMeshTextures : register(t2); // Diffuse
Texture2DArray staticMeshNormals : register(t3); // Normal map
Texture2DArray staticMeshSpeculars : register(t4); // Specular map

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
	
	float4 diffuseColor = staticMeshTextures.Sample(samLinear, float3(input.Texcoord, (float) MaterialIndex));
	float3 normalSample = staticMeshNormals.Sample(samLinear, float3(input.Texcoord, (float) MaterialIndex)).xyz;
	float4 specularColor = staticMeshSpeculars.Sample(samLinear, float3(input.Texcoord, (float) MaterialIndex));
    
    // Normal map 값을 -1~1 범위로 변환 후 정규화
	float3 normal = normalize(normalSample * 2.0f - 1.0f);
    
    // 간단한 조명 계산
	// TO-DO: refactor
	float3 lightDir = normalize(float3(1.0f, 0.0f, 0.0f)); 
	float NdotL = saturate(dot(normal, lightDir));
    
    // Phong 계열 스펙큘러 계산 (고정 shininess 값 16.0 사용)
	// TO-DO: refactor
	float3 viewDir = normalize(float3(0.0f, 0.0f, 1.0f));
	float3 halfDir = normalize(lightDir + viewDir);
	float specFactor = pow(saturate(dot(normal, halfDir)), 96.0f);
    
    // 최종 색상 조합: Diffuse와 스펙큘러 성분을 합산
	float4 finalColor = diffuseColor * NdotL + specularColor * specFactor;
    
	output.color = bUseVertexColor ? diffuseColor : finalColor;
	output.UUID = UUIDColor;
    
	return output;


}