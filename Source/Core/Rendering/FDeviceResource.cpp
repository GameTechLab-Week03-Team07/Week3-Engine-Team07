#include "FDevice.h"
#include "Core/Rendering/URenderer.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/Sampler.h"
#include "Resource/DirectResource/Rasterizer.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/ConstantBuffer.h"
#include "Resource/Texture.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Primitive/UGeometryGenerator.h"
#include "Resource/StaticMesh/FStaticMeshManager.h"

void FDevice::InitResource()
{
	// 테스트용 - obj파일 preload
	FStaticMeshManager::Get().LoadObjStaticMesh("cube.obj");
	FStaticMeshManager::Get().LoadObjStaticMesh("FinalBaseMesh.obj");
	FStaticMeshManager::Get().LoadObjStaticMesh("12140_Skull_v3.obj");

	const std::shared_ptr<FVertexShader> VS = FVertexShader::Load(L"Shaders/ShaderW0.hlsl","Simple_VS","mainVS");
	FInputLayout::Create("Simple_VS" , VS);
	FPixelShader::Load(L"Shaders/ShaderW0.hlsl","Simple_PS","mainPS");

	{
		
		std::shared_ptr<FVertexShader> TempVS = FVertexShader::Load(L"Shaders/Font_VS.hlsl","Font_VS","Font_VS");
		//FInputLayout::Create("Font_VS" , VS);
	}

	// 정적 메시용 셰이더 준비
	FVertexShader::Load(L"Shaders/StaticMesh_VS.hlsl", "StaticMesh_VS", "StaticMesh_VS");
	FPixelShader::Load(L"Shaders/StaticMesh_PS.hlsl", "StaticMesh_PS", "StaticMesh_PS");
	FPixelShader::Load(L"Shaders/Font_PS.hlsl", "Font_PS", "Font_PS");
	FPixelShader::Load(L"Shaders/SubUV_PS.hlsl", "SubUV_PS", "SubUV_PS");
	FConstantBuffer::Create("DefaultConstantBuffer", sizeof(FConstantsComponentData));

	//FPixelShader::Load(L"Shaders/Font_PS.hlsl","Font_PS","Font_PS");
	{
		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
		RasterizerDesc.CullMode = D3D11_CULL_BACK;  // 백 페이스 컬링
		RasterizerDesc.FrontCounterClockwise = FALSE;
	
		FRasterizer::Create("DefaultRasterizer", RasterizerDesc);
	}

	{
		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; // 채우기 모드
		RasterizerDesc.CullMode = D3D11_CULL_NONE;  // 백 페이스 컬링
		RasterizerDesc.FrontCounterClockwise = FALSE;

		FRasterizer::Create("DebugRasterizer", RasterizerDesc);
	}

	{
		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
		DepthStencilDesc.DepthEnable = true;
		// 깊이 테스트만 하고 안쓸수도 있다.
		// Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		DepthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		DepthStencilDesc.StencilEnable = false;

		FDepthStencilState::Create("DefaultDepthStencilState", DepthStencilDesc);
	}

	{
		D3D11_DEPTH_STENCIL_DESC AlwaysVisibleDepthStencilDesc = {};
		AlwaysVisibleDepthStencilDesc.DepthEnable = false;                       // Disable depth testing completely
		AlwaysVisibleDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // Don't write to depth buffer
		AlwaysVisibleDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;       // Optional since testing is disabled
		AlwaysVisibleDepthStencilDesc.StencilEnable = false;                     // Stencil not needed

		FDepthStencilState::Create("AlwaysVisibleDepthStencilState", AlwaysVisibleDepthStencilDesc);
	}


	{
		// Blend
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
	
		// src srcColor * src의 알파
		// 1, 0, 0(, 1) * 1.0f
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[1].BlendEnable = false;
		blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
		FBlendState::Create("DefaultBlendState", blendDesc);
	}

	{
		// Sampler state
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		FSampler::Create("LinearSamplerState", samplerDesc);
	}
	
	{
		// TextureSRV
		std::shared_ptr<FTexture> TextureImage = FTexture::Load("font_atlas.dds", "SubUVTexture");
		TextureImage->CreateShaderResourceView();
	}

	{
		// Blend
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;

		// src srcColor * src의 알파
		// 1, 0, 0(, 1) * 1.0f
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		//안개처럼 알파값이 계속 누적됨
		FBlendState::Create("AddBlendState", blendDesc);
	}

	////Material
	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("DefaultMaterial");
		Mat->SetRasterizer("DefaultRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("DefaultDepthStencilState");
		Mat->SetVertexShader("Simple_VS");
		Mat->SetPixelShader("Simple_PS");
	}
	
	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("AlwaysVisibleMaterial");
		Mat->SetRasterizer("DefaultRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("AlwaysVisibleDepthStencilState");
		Mat->SetVertexShader("Simple_VS");
		Mat->SetPixelShader("Simple_PS");
	}

	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("FontMaterial");
		Mat->SetRasterizer("DefaultRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("DefaultDepthStencilState");
		Mat->SetVertexShader("Font_VS");
		Mat->SetPixelShader("Font_PS");
	}

	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("SubUVMaterial");
		Mat->SetRasterizer("DefaultRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("DefaultDepthStencilState");
		Mat->SetVertexShader("Font_VS");
		Mat->SetPixelShader("SubUV_PS");
	}

	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("DebugMaterial");
		Mat->SetRasterizer("DebugRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("DefaultDepthStencilState");
		Mat->SetVertexShader("Simple_VS");
		Mat->SetPixelShader("Simple_PS");
	}
	// 정적 메시 오브젝트용 머티리얼 정의
	{
		std::shared_ptr<FMaterial> Mat = FMaterial::Create("StaticMeshMaterial");
		Mat->SetRasterizer("DefaultRasterizer");
		Mat->SetBlendState("DefaultBlendState");
		Mat->SetDepthState("DefaultDepthStencilState");
		Mat->SetVertexShader("StaticMesh_VS");
		Mat->SetPixelShader("StaticMesh_PS");
	}

	/// Mesh
	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;
		float size = 1.f;

		UGeometryGenerator::CreateCube(size, vertices, indices);

		FVertexBuffer::Create(FString("Cube"), vertices);
		FIndexBuffer::Create(FString("Cube"), indices);
		FMesh::Create("Cube");
	}

	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;
		int slices = 16;
		int stacks = 16;
		float radius = 1.f;
		[[maybe_unused]] float height = 1.f;

		UGeometryGenerator::CreateSphere(radius, slices, stacks, vertices, indices);
		
		FVertexBuffer::Create(FString("Sphere"), vertices);
		FIndexBuffer::Create(FString("Sphere"), indices);
		FMesh::Create("Sphere");

		
	}

	{
		FVertexSimple tempArray[] =
		{
			{  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f } 
		};
		TArray<FVertexSimple> vertices;

		vertices.Add(tempArray[0]);
		vertices.Add(tempArray[1]);
		vertices.Add(tempArray[2]);
		
		uint32 TriangleIndices[3] =
		{
			0, 1, 2
		};
		
		TArray<uint32> indices;
		indices.Add(TriangleIndices[0]);
		indices.Add(TriangleIndices[1]);
		indices.Add(TriangleIndices[2]);
		
		FVertexBuffer::Create(FString("Triangle"), vertices);
		FIndexBuffer::Create(FString("Triangle"), indices);
		FMesh::Create("Triangle");
		
	}
	
	{
		FVertexSimple tempArray[] =
		{
			{  0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{  0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
			{  0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{  0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f }
		};

		TArray<FVertexSimple> vertices;

		vertices.Add(tempArray[0]);
		vertices.Add(tempArray[1]);
		vertices.Add(tempArray[2]);
		vertices.Add(tempArray[3]);

		uint32 QuadIndices[6] =
		{
			0, 1, 2,
			0, 2, 3
		};

		TArray<uint32> indices;
		indices.Add(QuadIndices[0]);
		indices.Add(QuadIndices[1]);
		indices.Add(QuadIndices[2]);
		indices.Add(QuadIndices[3]);
		indices.Add(QuadIndices[4]);
		indices.Add(QuadIndices[5]);

		FVertexBuffer::Create(FString("Quad"), vertices);
		FIndexBuffer::Create(FString("Quad"), indices);

		FMesh::Create("Quad");
	}

	{
		FVertexSimple tempArray[2] =
		{
			{ -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }
		};
		TArray<FVertexSimple> vertices;

		vertices.Add(tempArray[0]);
		vertices.Add(tempArray[1]);
		
		uint32 tempIndices[2] =
		{
			0, 1
		};
		
		TArray<uint32> indices;
		indices.Add(tempIndices[0]);
		indices.Add(tempIndices[1]);
		
		FVertexBuffer::Create(FString("Line"), vertices);
		FIndexBuffer::Create(FString("Line"), indices);
		
		FMesh::Create(FString("Line"), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;
		int slices = 36;
		int stacks = 36;
		float BottomRadius = .2f;
		float TopRadius = .2f;
		float height = 1.f;

		UGeometryGenerator::CreateCylinder(BottomRadius, TopRadius, height, slices, stacks, vertices , indices);
		
		FVertexBuffer::Create(FString("Cylinder"), vertices);
		FIndexBuffer::Create(FString("Cylinder"), indices);
		 
		FMesh::Create(FString("Cylinder"));
	}

	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;
		int slices = 36;
		int stacks = 6;
		float radius = 1.f;
		float height = 1.f;

		UGeometryGenerator::CreateCone(radius, height, slices, stacks, vertices, indices);
		FVertexBuffer::Create(FString("Cone"), vertices);
		FIndexBuffer::Create(FString("Cone"), indices);


		FMesh::Create(FString("Cone"));
	}
	
	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;

		vertices.SetNum(sizeof(GizmoArrowVertices)/sizeof(FVertexSimple));
		memcpy(vertices.GetData(), GizmoArrowVertices, sizeof(GizmoArrowVertices));
		indices.SetNum(sizeof(GizmoArrowIndices) / sizeof(uint32));
		memcpy(indices.GetData(), GizmoArrowIndices, sizeof(GizmoArrowIndices));
		FVertexBuffer::Create(FString(TEXT("GizmoArrow")), vertices);
		FIndexBuffer::Create(FString(TEXT("GizmoArrow")), indices);

		FMesh::Create(TEXT("GizmoArrow"));
	}

	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;

		vertices.SetNum(sizeof(GizmoRotationVertices) / sizeof(FVertexSimple));
		memcpy(vertices.GetData(), GizmoRotationVertices, sizeof(GizmoRotationVertices));
		indices.SetNum(sizeof(GizmoRotationIndices) / sizeof(uint32));
		memcpy(indices.GetData(), GizmoRotationIndices, sizeof(GizmoRotationIndices));

		FVertexBuffer::Create(FString(TEXT("GizmoRotation")), vertices);
		FIndexBuffer::Create(FString(TEXT("GizmoRotation")), indices);

		FMesh::Create(TEXT("GizmoRotation"));
	}

	{
		TArray<FVertexSimple> vertices;
		TArray<uint32> indices;

		vertices.SetNum(sizeof(GizmoScaleVertices) / sizeof(FVertexSimple));
		memcpy(vertices.GetData(), GizmoScaleVertices, sizeof(GizmoScaleVertices));
		indices.SetNum(sizeof(GizmoScaleIndices) / sizeof(uint32));
		memcpy(indices.GetData(), GizmoScaleIndices, sizeof(GizmoScaleIndices));

		FVertexBuffer::Create(FString(TEXT("GizmoScale")), vertices);
		FIndexBuffer::Create(FString(TEXT("GizmoScale")), indices);

		FMesh::Create(TEXT("GizmoScale"));
	}

}
