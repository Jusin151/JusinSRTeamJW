#include "ShutGun.h"
#include "GameInstance.h"
#include "CUI_Manager.h"


CShutGun::CShutGun(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CShutGun::CShutGun(const CShutGun& Prototype)
	: CRanged_Weapon(Prototype),
	m_ShotGun_INFO{Prototype.m_ShotGun_INFO }
{
}

HRESULT CShutGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShutGun::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)	
		m_ShotGun_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_ShotGun_INFO.vSize.x, m_ShotGun_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_ShotGun_INFO.vPos.x, m_ShotGun_INFO.vPos.y, 0.f));

	m_vInitialPos = m_ShotGun_INFO.vPos;

	return S_OK;
}

void CShutGun::Priority_Update(_float fTimeDelta)
{
}

void CShutGun::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		t += speed;  
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		t += speed;
	}

	float v = 20.0f;  // 폭을 설정 하는변수
	_float3 vNewPos;
	vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
	vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	Attack(fTimeDelta);
	return;
}
void CShutGun::Attack(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_eState == State::Idle)
		{
			m_eState = State::Firing;
			m_iCurrentFrame = 0;
			m_fElapsedTime = 0.0f;
		}
	}

	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Idle:
		m_iCurrentFrame = 0;
		break;

	case State::Firing:
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 3)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_eState = State::Reloading;
				m_iCurrentFrame = 0;
			}
		}
		break;

	case State::Reloading:
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 14)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_eState = State::Idle;
				m_iCurrentFrame = 0;
			}
		}
		break;
	}
}
void CShutGun::Late_Update(_float fTimeDelta)
{
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CShutGun::Render()
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

	switch (m_eState)
	{
	case State::Idle:
	case State::Firing:
		if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
			return E_FAIL;
		break;

	case State::Reloading:
		if (FAILED(m_pTextureCom_Second->Bind_Resource(m_iCurrentFrame)))
			return E_FAIL;
		break;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CShutGun::On_Collision()
{
	


	return S_OK;
}

HRESULT CShutGun::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShotGun_1"),
		TEXT("Com_Texture_ShutGun_1"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShotGun_2"),
		TEXT("Com_Texture_ShutGun_2"), reinterpret_cast<CComponent**>(&m_pTextureCom_Second))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_ShutGun"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_ShutGun"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CShutGun* CShutGun::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShutGun* pInstance = new CShutGun(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("샷건 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CShutGun::Clone(void* pArg)
{
	CShutGun* pInstace = new CShutGun(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("샷건 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CShutGun::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
