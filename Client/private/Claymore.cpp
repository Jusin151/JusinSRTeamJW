#include "Claymore.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CClaymore::CClaymore(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMelee_Weapon(pGraphic_Device)
{
}

CClaymore::CClaymore(const CClaymore& Prototype)
	: CMelee_Weapon(Prototype),
	m_Claymore_INFO{Prototype.m_Claymore_INFO }
{
}

HRESULT CClaymore::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CClaymore::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		m_Claymore_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pTransformCom->Set_Scale(m_Claymore_INFO.vSize.x, m_Claymore_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Claymore_INFO.vPos.x, m_Claymore_INFO.vPos.y, 0.f));
	return S_OK;
}

void CClaymore::Priority_Update(_float fTimeDelta)
{
}

void CClaymore::Update(_float fTimeDelta)
{
	m_iCurrentFrame = (m_iCurrentFrame + 1) % 15; // 내일부터 무기 찍어낸다
}

void CClaymore::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CClaymore::Render()
{
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


	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CClaymore::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Claymore"),
		TEXT("Com_Texture_Claymore"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Claymore"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Claymore"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CClaymore* CClaymore::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CClaymore* pInstance = new CClaymore(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("클레이모어 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CClaymore::Clone(void* pArg)
{
	CClaymore* pInstace = new CClaymore(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("클레이모어 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CClaymore::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
