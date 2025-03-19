#include "UI_Bullet_Bar.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_Bullet_Bar::CUI_Bullet_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Bullet_Bar::CUI_Bullet_Bar(const CUI_Bullet_Bar& Prototype)
	: CUI_Base(Prototype),
	m_Bullet_Bar_INFO{ Prototype.m_Bullet_Bar_INFO }
{
}

HRESULT CUI_Bullet_Bar::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Bullet_Bar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Bullet_Bar_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_Bullet_Bar_INFO.vPos += CUI_Manager::GetInstance()->GetRightPanel_Pos();
		Set_Position(m_Bullet_Bar_INFO.vPos);
		Set_Size(m_Bullet_Bar_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Bullet", this);
	}
	else
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_Bullet_Bar_INFO.vSize.x, m_Bullet_Bar_INFO.vSize.y, 1.f);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Bullet_Bar_INFO.vPos.x, m_Bullet_Bar_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Bullet_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_Bullet_Bar::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('0') & 0x8000)
	{
		m_fBullet -= 1.f;
		if (m_fBullet < 0.f)
			m_fBullet = 0.f; // 최소 체력 제한

		Update_Bullet_Bar();
	}
}

void CUI_Bullet_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}
void CUI_Bullet_Bar::Update_Bullet_Bar()
{
	_float fBullet_Ratio = m_fBullet / 100.f;
	if (fBullet_Ratio < 0.f)
		fBullet_Ratio = 0.f;
	if (fBullet_Ratio > 1.f)
		fBullet_Ratio = 1.f;

	VTXPOSTEX* pVertices = nullptr;
	m_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	// 전체 이미지가 보이는 상태(fBullet_Ratio==1)면 좌측 texcoord.x = 0,
	// fBullet_Ratio가 줄어들면 좌측 부분만 보이도록 (예: 0.5이면 texcoord.x = 0.5)
	pVertices[0].vTexcoord.x = 1.0f - fBullet_Ratio; // 좌측 상단
	pVertices[3].vTexcoord.x = 1.0f - fBullet_Ratio; // 좌측 하단

	// 전체 상태(fBullet_Ratio == 1): 좌측 x = -0.5
	// fBullet_Ratio가 줄어들면 좌측 x = -0.5 + (1 - fBullet_Ratio)
	pVertices[0].vPosition.x = -0.5f + (1.0f - fBullet_Ratio);
	pVertices[3].vPosition.x = -0.5f + (1.0f - fBullet_Ratio);

	m_pVIBufferCom->Get_VertexBuffer()->Unlock();
}



HRESULT CUI_Bullet_Bar::Render()
{

	return __super::Render();
}

HRESULT CUI_Bullet_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Bullet"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Bullet_Bar* CUI_Bullet_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Bullet_Bar* pInstance = new CUI_Bullet_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("총알 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Bullet_Bar::Clone(void* pArg)
{
	CUI_Bullet_Bar* pInstace = new CUI_Bullet_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("총알 UI 클론 생성 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Bullet_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
