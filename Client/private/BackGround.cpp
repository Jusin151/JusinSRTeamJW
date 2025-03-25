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
	if(nullptr != m_pMaterial) m_pMaterial->Bind_Resource();

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

	m_pTransformCom->Bind_Resource();


	//__super::Begin();

	/* 정점을 그린다. */
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//__super::End();


	return S_OK;
}

HRESULT CBackGround::Ready_Components(const _wstring& strTextureTag)
{
	if (FAILED(__super::Add_Component(LEVEL_LOGO, strTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
   		return E_FAIL;


 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;
	
	//쉐이더 추가
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMaterial);
	Safe_Release(m_pShaderCom);

}
