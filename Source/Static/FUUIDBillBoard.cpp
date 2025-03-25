#include "FUUIDBillBoard.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Core/Rendering/FontAtlas.h"
#include "Object/World/World.h"
#include "Object/Actor/Actor.h"
#include "Object/Actor/Camera.h"
#include <d3dcompiler.h>
#include "DirectXTK/WICTextureLoader.h"

struct HangulJamo {
	wchar_t cho;
	wchar_t jung;
	wchar_t jong;
};

HangulJamo DecomposeHangul(wchar_t c) {
	HangulJamo result = { 0, 0, 0 };

	// 한글 완성형 범위 (0xAC00 ~ 0xD7A3) 확인
	if (c < 0xAC00 || c > 0xD7A3) {
		return result;
	}

	const int baseCode = 0xAC00;
	const int choCount = 19;
	const int jungCount = 21;
	const int jongCount = 28; // 종성 없음(0) 포함

	int code = c - baseCode;

	int jongIndex = code % jongCount;
	code /= jongCount;

	int jungIndex = code % jungCount;

	int choIndex = code / jungCount;

	// 초성 매핑
	static const wchar_t choTable[19] = {
		0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143,
		0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
	};

	// 중성 매핑
	static const wchar_t jungTable[21] = {
		0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157,
		0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E, 0x315F, 0x3160, 0x3161, 0x3162, 0x3163
	};

	// 종성 매핑
	static const wchar_t jongTable[28] = {
		0, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3139, 0x313A,
		0x313B, 0x313C, 0x313D, 0x313E, 0x313F, 0x3140, 0x3141, 0x3142, 0x3144, 0x3145,
		0x3146, 0x3147, 0x3148, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
	};

	result.cho = choTable[choIndex];
	result.jung = jungTable[jungIndex];
	result.jong = jongTable[jongIndex];

	return result;
}

// 중성이 초성의 오른쪽에 오는지 확인
bool IsRightJungseong(wchar_t jung) {
	static const std::wstring rightJungseongs = L"ㅏㅐㅑㅒㅓㅔㅕㅖㅣ";
	return rightJungseongs.find(jung) != std::wstring::npos;
}

// 중성이 초성의 아래에 오는지 확인
bool IsBelowJungseong(wchar_t jung) {
	static const std::wstring belowJungseongs = L"ㅗㅛㅜㅠㅡ";
	return belowJungseongs.find(jung) != std::wstring::npos;
}

// 복합 중성의 첫 부분 반환 (예: ㅘ → ㅗ)
wchar_t GetFirstPartOfComplex(wchar_t jung) {
	switch (jung) {
	case L'ㅘ': case L'ㅙ': case L'ㅚ': return L'ㅗ';
	case L'ㅝ': case L'ㅞ': case L'ㅟ': return L'ㅜ';
	case L'ㅢ': return L'ㅡ';
	default: return 0;
	}
}

// 복합 중성의 두번째 부분 반환 (예: ㅘ → ㅏ)
wchar_t GetSecondPartOfComplex(wchar_t jung) {
	switch (jung) {
	case L'ㅘ': return L'ㅏ';
	case L'ㅙ': return L'ㅐ';
	case L'ㅚ': return L'ㅣ';
	case L'ㅝ': return L'ㅓ';
	case L'ㅞ': return L'ㅔ';
	case L'ㅟ': return L'ㅣ';
	case L'ㅢ': return L'ㅣ';
	default: return 0;
	}
}

void FUUIDBillBoard::CreateKoreanQuad(const wchar_t character, float& cursorX, int StringLength) {
	// 한글 자모 분리
	HangulJamo jamo = DecomposeHangul(character);

	// 초성 쿼드 구성
	if (jamo.cho) {
		CreateKoreanConsonantVowel(jamo.cho, cursorX, 0.0f, 0.0f);
	}
	
	// 중성 쿼드 구성
	if (jamo.jung) {
		// 중성이 오른쪽에 오는 경우
		if (IsRightJungseong(jamo.jung)) {
			CreateKoreanConsonantVowel(jamo.jung, cursorX, 0.5f, 0.0f);
		}
		// 중성이 아래에 오는 경우
		else if (IsBelowJungseong(jamo.jung)) {
			CreateKoreanConsonantVowel(jamo.jung, cursorX, 0.0f, 0.7f);
		}
		// 복합 중성인 경우 (ㅘ, ㅙ, ㅚ, ㅝ, ㅞ, ㅟ, ㅢ 등)
		else {
			// 복합 중성의 첫 부분 (예: ㅘ의 ㅗ)
			wchar_t firstPart = GetFirstPartOfComplex(jamo.jung);
			// 복합 중성의 두번째 부분 (예: ㅘ의 ㅏ)
			wchar_t secondPart = GetSecondPartOfComplex(jamo.jung);

			if (firstPart) {
				CreateKoreanConsonantVowel(firstPart, cursorX, 0.0f, 0.8f); // 아래 부분
			}
			if (secondPart) {
				CreateKoreanConsonantVowel(secondPart, cursorX, 0.5f, 0.3f); // 오른쪽 부분
			}
		}
	}

	// 종성 쿼드 구성
	if (jamo.jong) {
		if (IsRightJungseong(jamo.jung)) {
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.3f, 1.0f);
		}
		else if (IsBelowJungseong(jamo.jung)) {
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.0f, 1.4f);
		}
		else 
		{
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.3f, 1.4f);
		}
	}

	// 커서 이동 (한 글자 너비만큼)
	cursorX += 2 * FFontAtlas::Get().GlyphAspectRatio * FFontAtlas::Get().Kerning;
}

void FUUIDBillBoard::CreateKoreanConsonantVowel(wchar_t jamo, float posX, float offsetX, float offsetY) {
	const GlyphInfo& glyph = FFontAtlas::Get().GetGlyph(jamo);

	FVertexSimple vertices[4] = {
		{ 0.0f, posX + offsetX, 1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v },
		{ 0.0f, posX + offsetX + FFontAtlas::Get().GlyphAspectRatio, 1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v },
		{ 0.0f, posX + offsetX + FFontAtlas::Get().GlyphAspectRatio, -1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v + glyph.height },
		{ 0.0f, posX + offsetX, -1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v + glyph.height }
	};

	// 정점 및 인덱스 버퍼에 추가
	for (int j = 0; j < 4; ++j) {
		VertexBuffer.Add(vertices[j]);
	}

	uint32 baseIndex = static_cast<uint32>(VertexBuffer.Num()) - 4;
	IndexBuffer.Add(baseIndex + 0);
	IndexBuffer.Add(baseIndex + 1);
	IndexBuffer.Add(baseIndex + 2);
	IndexBuffer.Add(baseIndex + 0);
	IndexBuffer.Add(baseIndex + 2);
	IndexBuffer.Add(baseIndex + 3);
}

void FUUIDBillBoard::UpdateString(const std::wstring& String)
{
	Flush();

	uint32 StringLen = static_cast<uint32>(String.size());
	float AspectRatio = FFontAtlas::Get().GlyphAspectRatio;
	float Kerning = FFontAtlas::Get().Kerning;
	float cursorX = (StringLen - 1) * -AspectRatio * Kerning;

	for (size_t i = 0; i < StringLen; ++i)
	{
		wchar_t c = String[i];

		if (c >= 0xAC00 && c <= 0xD7A3) {
			CreateKoreanQuad(c, cursorX, StringLen);
		}
		else
		{
			const GlyphInfo& glyph = FFontAtlas::Get().GetGlyph(c);

			FVertexSimple vertices[4] =
			{
				{ 0.0f, -AspectRatio + cursorX, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v },
				{ 0.0f, AspectRatio + cursorX, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v },
				{ 0.0f, AspectRatio + cursorX, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v + glyph.height },
				{ 0.0f, -AspectRatio + cursorX, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v + glyph.height }
			};

			for (int j = 0; j < 4; ++j)
			{
				VertexBuffer.Add(vertices[j]);
			}

			uint32 baseIndex = static_cast<uint32>(VertexBuffer.Num()) - 4;
			IndexBuffer.Add(baseIndex + 0);
			IndexBuffer.Add(baseIndex + 1);
			IndexBuffer.Add(baseIndex + 2);
			IndexBuffer.Add(baseIndex + 0);
			IndexBuffer.Add(baseIndex + 2);
			IndexBuffer.Add(baseIndex + 3);

			cursorX += 2 * AspectRatio * Kerning;
		}
	}

	// 버텍스 버퍼 업데이트
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	FDevice::Get().GetDeviceContext()->Map(FontVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, VertexBuffer.GetData(), sizeof(FVertexSimple) * VertexBuffer.Num());
	FDevice::Get().GetDeviceContext()->Unmap(FontVertexBuffer, 0);

	// 인덱스 버퍼 업데이트
	FDevice::Get().GetDeviceContext()->Map(FontIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, IndexBuffer.GetData(), sizeof(uint32) * IndexBuffer.Num());
	FDevice::Get().GetDeviceContext()->Unmap(FontIndexBuffer, 0);
}

void FUUIDBillBoard::Flush()
{
	VertexBuffer = {};
	IndexBuffer = {};
}

void FUUIDBillBoard::CalculateModelMatrix(FMatrix& OutMatrix)
{
	ACamera* cam = UEngine::Get().GetWorld()->GetCamera();

	FVector cameraPosition = cam->GetActorTransform().GetPosition();

	FVector objectPosition = TargetObject->GetWorldTransform().GetPosition() + FVector(0.0f, 0.0f, 1.0f);
	FVector objectScale(0.2f, 0.2f, 0.2f);

	FVector lookDir = (objectPosition - cameraPosition).GetSafeNormal();
	FVector right = FVector(0, 0, 1).Cross(lookDir).GetSafeNormal();
	FVector up = lookDir.Cross(right).GetSafeNormal();

	FMatrix rotationMatrix;

	// 회전 행렬 구성
	rotationMatrix.X = FVector4(lookDir.X, lookDir.Y, lookDir.Z, 0);
	rotationMatrix.Y = FVector4(right.X, right.Y, right.Z, 0);
	rotationMatrix.Z = FVector4(up.X, up.Y, up.Z, 0);
	rotationMatrix.W = FVector4(0, 0, 0, 1);

	FMatrix positionMatrix = FMatrix::GetTranslateMatrix(objectPosition);
	FMatrix scaleMatrix = FMatrix::GetScaleMatrix(objectScale);

	OutMatrix = scaleMatrix * rotationMatrix * positionMatrix;
	return;
}

void FUUIDBillBoard::SetTarget(AActor* Target)
{
	TargetObject = Target->GetRootComponent();
	
	//std::wstring string = L"UUID:";
	std::wstring string = L"유유아이디 :";
	UpdateString(string.append(std::to_wstring(Target->GetUUID())));
	//std::wstring string = L"가츄괘퓌덤굥맑욳낢귉 TEST ENGLISH!";
	//UpdateString(string);
}

void FUUIDBillBoard::Render()
{
	if (VertexBuffer.Num() == 0 || !FEngineShowFlags::Get().GetSingleFlag(EEngineShowFlags::SF_BillboardText) || !TargetObject)
		return;

	//Prepare
	ID3D11DeviceContext* DeviceContext = FDevice::Get().GetDeviceContext();


	// 파이프라인 상태 설정
	UINT stride = sizeof(FVertexSimple);
	UINT offset = 0;

	// 기본 셰이더랑 InputLayout을 설정

	DeviceContext->IASetVertexBuffers(0, 1, &FontVertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(FontIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(PrimitiveTopology);

	DeviceContext->VSSetShader(FontVertexShader, nullptr, 0);
	DeviceContext->PSSetShader(FontPixelShader, nullptr, 0);
	DeviceContext->IASetInputLayout(FontInputLayout);

	DeviceContext->RSSetState(RasterizerState);
	DeviceContext->OMSetBlendState(BlendState, BlendFactor, 0xffffffff);
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);

	// Billboard
	FMatrix ModelMatrix;
	CalculateModelMatrix(ModelMatrix);

	// 버텍스 쉐이더에 상수 버퍼를 설정
	if (FontConstantBuffer)
	{
		DeviceContext->VSSetConstantBuffers(0, 1, &FontConstantBuffer);

		D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
		FFontConstantInfo Constants;
		URenderer* Renderer = UEngine::Get().GetRenderer();

		Constants.ViewProjectionMatrix = FMatrix::Transpose(
			ModelMatrix
			* UEngine::Get().GetWorld()->GetCamera()->GetViewProjectionMatrix()
		);

		// D3D11_MAP_WRITE_DISCARD는 이전 내용을 무시하고 새로운 데이터로 덮어쓰기 위해 사용
		DeviceContext->Map(FontConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
		{
			memcpy(ConstantBufferMSR.pData, &Constants, sizeof(FFontConstantInfo));
		}
		DeviceContext->Unmap(FontConstantBuffer, 0);
	}

	DeviceContext->DrawIndexed((UINT)IndexBuffer.Num(), 0, 0);
}

void FUUIDBillBoard::Create()
{
	ID3D11Device* Device = FDevice::Get().GetDevice();
	ID3D11DeviceContext* DeviceContext = FDevice::Get().GetDeviceContext();

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(FVertexSimple) * MaxVerticesPerBatch;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 버텍스 버퍼 생성
	HRESULT hr = Device->CreateBuffer(&vertexBufferDesc, nullptr, &FontVertexBuffer);

	if (FAILED(hr))
	{
		return;
	}

	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.ByteWidth = sizeof(uint32) * MaxIndicesPerBatch;
	IndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 인덱스 버퍼 생성
	hr = Device->CreateBuffer(&IndexBufferDesc, nullptr, &FontIndexBuffer);
	if (FAILED(hr))
	{
	}

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 버텍스 쉐이더 컴파일
	hr = D3DCompileFromFile(L"Shaders/Font_VS.hlsl", nullptr, nullptr, "Font_VS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errorBlob);
	if (FAILED(hr))
	{
		// 컴파일 오류 처리
		if (errorBlob)
		{
		}
	}

	// 픽셀 쉐이더 컴파일
	hr = D3DCompileFromFile(L"Shaders/Font_PS.hlsl", nullptr, nullptr, "Font_PS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errorBlob);
	if (FAILED(hr))
	{
		// 컴파일 오류 처리
		if (errorBlob)
		{
		}
	}

	// 쉐이더 생성
	hr = Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &FontVertexShader);
	if (FAILED(hr))
	{
		// 오류 처리
	}

	hr = Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &FontPixelShader);
	if (FAILED(hr))
	{
		// 오류 처리
	}

	// 입력 레이아웃 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	hr = Device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &FontInputLayout);
	if (FAILED(hr))
	{
		// 오류 처리
	}

	// 버텍스/픽셀 쉐이더 블롭 해제
	if (vsBlob) vsBlob->Release();
	if (psBlob) psBlob->Release();

	// 래스터라이저 상태 설정
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // 라인은 양면을 볼 수 있도록 컬링 없음
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = TRUE; // 안티앨리어싱된 라인 활성화

	// 래스터라이저 상태 생성
	hr = Device->CreateRasterizerState(&rasterizerDesc, &RasterizerState);
	if (FAILED(hr))
	{
		// 오류 처리
	}

	// 상수 버퍼 생성
	D3D11_BUFFER_DESC ConstantBufferDesc = {};
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
	ConstantBufferDesc.ByteWidth = (sizeof(FFontConstantInfo) + 0xf) & 0xfffffff0;  // 16byte의 배수로 올림
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정

	Device->CreateBuffer(&ConstantBufferDesc, nullptr, &FontConstantBuffer);

	// Blend State
	// Blend
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Device->CreateBlendState(&blendDesc, &BlendState);

	D3D11_DEPTH_STENCIL_DESC AlwaysVisibleDepthStencilDesc = {};
	AlwaysVisibleDepthStencilDesc.DepthEnable = false;                       // Disable depth testing completely
	AlwaysVisibleDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // Don't write to depth buffer
	AlwaysVisibleDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;       // Optional since testing is disabled
	AlwaysVisibleDepthStencilDesc.StencilEnable = false;                     // Stencil not needed

	Device->CreateDepthStencilState(&AlwaysVisibleDepthStencilDesc, &DepthStencilState);

	DirectX::CreateWICTextureFromFile(FDevice::Get().GetDevice(), FDevice::Get().GetDeviceContext(), L"Pretendard_Kor.png", nullptr, &FontTextureSRV);

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

	FDevice::Get().GetDevice()->CreateSamplerState(&samplerDesc, &FontSamplerState);
	FDevice::Get().GetDeviceContext()->PSSetShaderResources(0, 1, &FontTextureSRV);
	FDevice::Get().GetDeviceContext()->PSSetSamplers(0, 1, &FontSamplerState);
}