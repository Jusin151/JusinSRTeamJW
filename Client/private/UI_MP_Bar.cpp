#include "UI_MP_Bar.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_MP_Bar::CUI_MP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_MP_Bar::CUI_MP_Bar(const CUI_MP_Bar& Prototype)
	: CUI_Base(Prototype),
	m_MP_INFO{ Prototype.m_MP_INFO }
{
}

HRESULT CUI_MP_Bar::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_MP_Bar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_MP_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_MP_INFO.vPos += CUI_Manager::GetInstance()->GetLeftPanel_Pos();
		Set_Position(m_MP_INFO.vPos);
		Set_Size(m_MP_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"MP_Bar", this);
	}
	else
		return E_FAIL;


	m_iMp = 50;
	m_pTransformCom->Set_Scale(m_MP_INFO.vSize.x, m_MP_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_MP_INFO.vPos.x, m_MP_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_MP_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_MP_Bar::Update(_float fTimeDelta)
{
	
}

void CUI_MP_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}
void CUI_MP_Bar::Update_Mp_Bar()
{
	_float fHP_Ratio = m_iMp / 50.f;

	if (fHP_Ratio < 0.f)
		fHP_Ratio = 0.f;
	if (fHP_Ratio > 1.f)
		fHP_Ratio = 1.f;


	VTXPOSTEX* pVertices = nullptr;
	m_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	//  (오른쪽부터 점점 안 보이게)
	pVertices[1].vTexcoord.x = fHP_Ratio; // 우측 상단
	pVertices[2].vTexcoord.x = fHP_Ratio; // 우측 하단

	//  정점 위치  (오른쪽부터 점점 줄어들게) 
	float fNewWidth = m_MP_INFO.vSize.x * fHP_Ratio;
	pVertices[1].vPosition.x = -0.5f + fNewWidth / m_MP_INFO.vSize.x;
	pVertices[2].vPosition.x = -0.5f + fNewWidth / m_MP_INFO.vSize.x;

	m_pVIBufferCom->Get_VertexBuffer()->Unlock();
}


HRESULT CUI_MP_Bar::Render()
{
	

	if (FAILED(__super::Render()))
		return E_FAIL;

	 m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_iMp), _float2(-547.f, 298.0f), _float2(10.f, 20.f), _float3(1.f, 1.f, 1.f));

	 return S_OK;
}

HRESULT CUI_MP_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MP_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_MP"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_MP_Bar* CUI_MP_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_MP_Bar* pInstance = new CUI_MP_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Mana UI,can't Create ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_MP_Bar::Clone(void* pArg)
{
	CUI_MP_Bar* pInstace = new CUI_MP_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("Mana UI ,Can't Clone");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_MP_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
