#include "UI_Player_Icon.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Player_Icon::CUI_Player_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Player_Icon::CUI_Player_Icon(const CUI_Player_Icon& Prototype)
	: CUI_Base(Prototype),
	m_PlayerICon_pTextureCom(Prototype.m_PlayerICon_pTextureCom),
	m_PlayerICon_pTransformCom(Prototype.m_PlayerICon_pTransformCom),
	m_PlayerICon_pVIBufferCom(Prototype.m_PlayerICon_pVIBufferCom),
	m_PlayerICon_INFO{ Prototype.m_PlayerICon_INFO }
{
}

HRESULT CUI_Player_Icon::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CUI_Player_Icon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_PlayerICon_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_PlayerICon_INFO.vPos += CUI_Manager::GetInstance()->GetParent_Pos();
		Set_Position(m_PlayerICon_INFO.vPos);
		Set_Size(m_PlayerICon_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Player_Icon", this);
	}
	else
		return E_FAIL;

	 
	m_PlayerICon_pTransformCom->Set_Scale(m_PlayerICon_INFO.vSize.x, m_PlayerICon_INFO.vSize.y, 1.f);
	m_PlayerICon_pTransformCom->Set_State(CTransform::STATE_POSITION,
	_float3(m_PlayerICon_INFO.vPos.x, m_PlayerICon_INFO.vPos.y, 0.f));


	return S_OK;
}

void CUI_Player_Icon::Priority_Update(_float fTimeDelta)
{
	m_iPriorityHp = CUI_Manager::GetInstance()->Get_Hp();
	

}
void CUI_Player_Icon::Set_Hp_Event()
{
	m_bIsHit = true;
	m_fHitElapsedTime = 0.0f;
}

void CUI_Player_Icon::Update(_float fTimeDelta)
{


}
void CUI_Player_Icon::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
	
	Update_Animation(fTimeDelta);
}


void CUI_Player_Icon::Update_Animation(_float fTimeDelta)
{

	m_iLateHp = CUI_Manager::GetInstance()->Get_Hp();

	if (m_bIsHit)
	{
		Update_Hit_Animation(fTimeDelta);
		return;
	}

	int defaultStart = 0, defaultCount = 4;
	if (m_iLateHp >= 70)
	{
		defaultStart = 0;    // 0~3
		defaultCount = 4;
	}
	else if (m_iLateHp >= 50)
	{
		defaultStart = 5;    // 5~8
		defaultCount = 4;
	}
	else if (m_iLateHp >= 20)
	{
		defaultStart = 10;   // 10~13
		defaultCount = 4;
	}
	else
	{
		defaultStart = 15;   // 15~18
		defaultCount = 4;
	}

	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= m_fFrameDuration)
	{
		m_fElapsedTime = 0.0f;
		if (m_iCurrentFrame < defaultStart || m_iCurrentFrame >= defaultStart + defaultCount)
			m_iCurrentFrame = defaultStart;
		else
			m_iCurrentFrame = defaultStart + ((m_iCurrentFrame - defaultStart + 1) % defaultCount);
	}
	if (m_iPriorityHp != m_iLateHp)
	{
	
	}
}


void CUI_Player_Icon::Update_Hit_Animation(_float fTimeDelta)
{
	int hitFrame = 4; 
	if (m_iLateHp >= 70)
	{
		hitFrame = 4;
	}
	else if (m_iLateHp >= 50)
	{
		hitFrame = 9;
	}
	else if (m_iLateHp >= 20)
	{
		hitFrame = 14;
	}
	else
	{
		hitFrame = 19;
	}

	
	m_iCurrentFrame = hitFrame;
	m_fHitElapsedTime += fTimeDelta;
	if (m_fHitElapsedTime >= m_fHitDuration)
	{
		m_bIsHit = false; 
		if (m_iLateHp >= 70)
			m_iCurrentFrame = 0;
		else if (m_iLateHp >= 50)
			m_iCurrentFrame = 5;
		else if (m_iLateHp >= 20)
			m_iCurrentFrame = 10;
		else
			m_iCurrentFrame = 15;
	}
}




HRESULT CUI_Player_Icon::Render()
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

	if (FAILED(m_PlayerICon_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_PlayerICon_pTextureCom->Bind_Resource(m_iCurrentFrame))) // 현재 프레임 인덱스를 사용하여 텍스처 바인딩
		return E_FAIL;
	if (FAILED(m_PlayerICon_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_PlayerICon_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}


HRESULT CUI_Player_Icon::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_PlayerICon_pTextureCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_PlayerICon_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_PlayerICon_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}


CUI_Player_Icon* CUI_Player_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Player_Icon* pInstance = new CUI_Player_Icon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("플레이어 아이콘 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Player_Icon::Clone(void* pArg)
{
	CUI_Player_Icon* pInstace = new CUI_Player_Icon(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("플레이어 아이콘 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Player_Icon::Free()
{
	__super::Free();

	Safe_Release(m_PlayerICon_pTextureCom);
	Safe_Release(m_PlayerICon_pTransformCom);
	Safe_Release(m_PlayerICon_pVIBufferCom);
}
