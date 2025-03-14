癤#include "UI_Bullet_Bar.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_Bullet_Bar::CUI_Bullet_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Bullet_Bar::CUI_Bullet_Bar(const CUI_Bullet_Bar& Prototype)
	: CUI_Base(Prototype),
	m_Bullet_Bar_pTextureCom(Prototype.m_Bullet_Bar_pTextureCom),
	m_Bullet_Bar_pTransformCom(Prototype.m_Bullet_Bar_pTransformCom),
	m_Bullet_Bar_pVIBufferCom(Prototype.m_Bullet_Bar_pVIBufferCom),
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

	m_Bullet_Bar_pTransformCom->Set_Scale(m_Bullet_Bar_INFO.vSize.x, m_Bullet_Bar_INFO.vSize.y, 1.f);


	m_Bullet_Bar_pTransformCom->Set_State(CTransform::STATE_POSITION,
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
	_float fHP_Ratio = m_fBullet / 100.f;

	if (fHP_Ratio < 0.f)
		fHP_Ratio = 0.f;
	if (fHP_Ratio > 1.f)
		fHP_Ratio = 1.f;


	VTXPOSTEX* pVertices = nullptr;
	m_Bullet_Bar_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	//  (오른쪽부터 점점 안 보이게)
	pVertices[1].vTexcoord.x = fHP_Ratio; // 우측 상단
	pVertices[2].vTexcoord.x = fHP_Ratio; // 우측 하단

	//  정점 위치  (오른쪽부터 점점 줄어들게)  아직 안고쳤음!!!!!!!!!!
	float fNewWidth = m_Bullet_Bar_INFO.vSize.x * fHP_Ratio;
	pVertices[0].vPosition.x = +0.5f + fNewWidth / m_Bullet_Bar_INFO.vSize.x;
	pVertices[3].vPosition.x = +0.5f + fNewWidth / m_Bullet_Bar_INFO.vSize.x;

	m_Bullet_Bar_pVIBufferCom->Get_VertexBuffer()->Unlock();
}
HRESULT CUI_Bullet_Bar::Render()
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


	if (FAILED(m_Bullet_Bar_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_Bullet_Bar_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_Bullet_Bar_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_Bullet_Bar_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_Bullet_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Bar"),
		TEXT("Com_Texture_HP"), reinterpret_cast<CComponent**>(&m_Bullet_Bar_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Bullet_Bar"),
		TEXT("Com_VIBuffer_HP"), reinterpret_cast<CComponent**>(&m_Bullet_Bar_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Transform_Bullet_Bar"),
		TEXT("Com_Transform_HP"), reinterpret_cast<CComponent**>(&m_Bullet_Bar_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Bullet_Bar* CUI_Bullet_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Bullet_Bar* pInstance = new CUI_Bullet_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("泥대젰諛 UI 썝蹂 깮꽦 떎뙣 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Bullet_Bar::Clone(void* pArg)
{
	CUI_Bullet_Bar* pInstace = new CUI_Bullet_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("泥대젰諛 UI 蹂듭젣 떎뙣");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Bullet_Bar::Free()
{
	__super::Free();

	Safe_Release(m_Bullet_Bar_pTextureCom);
	Safe_Release(m_Bullet_Bar_pTransformCom);
	Safe_Release(m_Bullet_Bar_pVIBufferCom);
}
