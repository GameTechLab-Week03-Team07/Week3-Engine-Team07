cbuffer FConstantsComponentData : register(b0)
{
	float4x4 MVP;
	float4 CustomColor;
	float4 UUIDColor;
	uint bUseVertexColor;
};

struct PS_INPUT
{
	float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
	float4 color : COLOR; // Color to pass to the pixel shader
    // float4 depthPosition : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0; //
	uint4 UUID : SV_Target1;
    //float depth : SV_Depth;
};

PS_OUTPUT Simple_PS(PS_INPUT input)
{
	PS_OUTPUT output;
	
	output.color = input.color;
	output.UUID = UUIDColor;
    // output.depth = saturate(depth);
    
	return output;
}

// float4 outlinePS(PS_INPUT input) : SV_TARGET
// {
//     // Output the color directly
//     return float4(1.0f, 0.647f, 0.0f, 0.1f);
// }

// PS_OUTPUT PickingPS(PS_INPUT input):SV_TARGET{
//     PS_OUTPUT output;
//     output.color = UUIDColor;
//     return output;
// }