#include "BackGround.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CBackGround::CBackGround(const CBackGround& Prototype)
	: CUI_Base(Prototype),
	m_BackGround_INFO{ Prototype.m_BackGround_INFO },
	m_pMaterial { Prototype.m_pMaterial }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
 		m_BackGround_INFO = *reinterpret_cast<BackGround_DESC*>(pArg);
	}
	else
		return E_FAIL;


	if (FAILED(Ready_Components(m_BackGround_INFO.strTextureTag)))
		return E_FAIL;


	Set_Position(m_BackGround_INFO.BackGround_Desc.vPos);
	Set_Size(m_BackGround_INFO.BackGround_Desc.vSize);
	//CUI_Manager::GetInstance()->AddUI(L"LOGO%d", this); // 얘는 나중에 캔버스랑 연동


	m_pTransformCom->Set_Scale(m_BackGround_INFO.BackGround_Desc.vSize.x, m_BackGround_INFO.BackGround_Desc.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_BackGround_INFO.BackGround_Desc.vPos.x, m_BackGround_INFO.BackGround_Desc.vPos.y, 0.f));
	return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
}

void CBackGround::Update(_float fTimeDelta)
{
	if(abs(m_BackGround_INFO.fmoveSpeed)>0.f)
	accumulatedDistance += m_BackGround_INFO.fmoveSpeed * fTimeDelta;

	if (abs(accumulatedDistance) >= m_BackGround_INFO.fMoveDistance)  
	{
		accumulatedDistance = 0.0f;
	}

	m_BackGround_INFO.BackGround_Desc.vPos.x = m_BackGround_INFO.fNextx + accumulatedDistance; // 저게 그러니까 싸이클 한번돌고



	// 위치 설정
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_BackGround_INFO.BackGround_Desc.vPos.x, m_BackGround_INFO.BackGround_Desc.vPos.y, 0.f));
}


void CBackGround::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CBackGround::Render()
{	
	//// 픽셀 셰이더 로드 및 설정
	//IDirect3DPixelShader9* pPixelShader = nullptr;
	//LPD3DXBUFFER pShaderBuffer = nullptr;
	//LPD3DXBUFFER pErrorBuffer = nullptr;

	//HRESULT hr = D3DXCompileShaderFromFile(L"TestPixelShader.hlsl", nullptr, nullptr, "PS", "ps_3_0", D3DXSHADER_DEBUG, &pShaderBuffer, &pErrorBuffer, nullptr);
	//if (FAILED(hr))
	//{
	//	if (pErrorBuffer)
	//	{
	//		OutputDebugStringA((char*)pErrorBuffer->GetBufferPointer());
	//		pErrorBuffer->Release();
	//	}
	//	return E_FAIL;
	//}

	//hr = m_pGraphic_Device->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pPixelShader);
	//if (FAILED(hr))
	//{
	//	pShaderBuffer->Release();
	//	return E_FAIL;
	//}

	//m_pGraphic_Device->SetPixelShader(pPixelShader);

	//// 빛 정보 설정
	//D3DXVECTOR3 lightDirection(0.0f, 0.0f, -1.0f); // 빛의 방향 (예시)
	//D3DXCOLOR lightColor(1.0f, 0.0f, 0.0f, 1.0f); // 빨간 빛 (예시)

	//m_pGraphic_Device->SetPixelShaderConstantF(0, (float*)&lightDirection, 1);
	//m_pGraphic_Device->SetPixelShaderConstantF(1, (float*)&lightColor, 1);

	if(nullptr != m_pMaterial) m_pMaterial->Bind_Resource();

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL; // 방향성 광원
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 확산광 색상 (빨간색)
	light.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f); // 주변광 색상
	light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // 반사광 색상
	light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f); // 빛의 방향

	m_pGraphic_Device->SetLight(0, &light); // 0번 라이트 설정
	m_pGraphic_Device->LightEnable(0, TRUE); // 0번 라이트 활성화
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE); // 라이팅 활성화


	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 그거 왼쪽으로 갈지 오른쪽으로 갈지를 안정해줬으
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (m_BackGround_INFO.fStack_MoveDistance != 0.f)
	{
		// 두 번째 이미지 렌더링 (첫 번째 이미지의 오른쪽에 할꺼임!)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			_float3(m_BackGround_INFO.BackGround_Desc.vPos.x + m_BackGround_INFO.fMoveDistance, m_BackGround_INFO.BackGround_Desc.vPos.y, 0.f));
		if (FAILED(m_pTransformCom->Bind_Resource()))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_Resource(0)))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE); // 라이팅 비활성화
	m_pGraphic_Device->LightEnable(0, FALSE); // 0번 라이트 비활성화

	return S_OK;
}

HRESULT CBackGround::Ready_Components(const _wstring& strTextureTag)
{
	if (FAILED(__super::Add_Component(LEVEL_LOGO, strTextureTag,
		TEXT("Com_Texture_Back"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
   		return E_FAIL;


 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Back"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Back"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;




	//마테리얼 추가
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterial))))
		return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround* pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("첫번째백그라운드원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstace = new CBackGround(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("첫번째백그라운드 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CBackGround::Free()
{
	__super::Free();
	Safe_Release(m_pMaterial);
}
