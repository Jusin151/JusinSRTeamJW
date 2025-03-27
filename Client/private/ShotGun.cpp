#include "ShotGun.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "Item_Manager.h"

CShotGun::CShotGun(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CShotGun::CShotGun(const CShotGun& Prototype)
	: CRanged_Weapon(Prototype),
	m_ShotGun_INFO{Prototype.m_ShotGun_INFO }
{
}

HRESULT CShotGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShotGun::Initialize(void* pArg)
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

	// 초기화 시 설정
	m_TextureRanges["Idle"] = { 0,0 };
	m_TextureRanges["Reloading"] = { 3, 16 };
	m_TextureRanges["Firing"] = { 1, 2 };

	CItem_Manager::GetInstance()->Add_Weapon(L"ShotGun", this);

	CItem_Icon::Icon_DESC ShotGun_Icon{};
	ShotGun_Icon.Icon_Image = ShotGun;
	ShotGun_Icon.Weapon_Type = CItem_Icon::ShotGun; // 선택되고 나서 되돌릴 이미지
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Icon"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Icon_ShotGun"),&ShotGun_Icon)))
		return E_FAIL;

	__super::Ready_Picking();


	return S_OK;
}

void CShotGun::Priority_Update(_float fTimeDelta)
{
}

void CShotGun::Update(_float fTimeDelta)
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
void CShotGun::Attack(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_eState == State::Idle)
		{
			m_eState = State::Firing;
			m_iCurrentFrame = m_TextureRanges["Firing"].first;
			m_fElapsedTime = 0.0f;

			__super::Picking_Object(5);
			CUI_Manager::GetInstance()->Set_Shotgun_Bullet(1);
		}
	}

	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Idle:
		m_iCurrentFrame = m_TextureRanges["Idle"].first;
		break;

	case State::Firing:
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Firing"].second)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_eState = State::Reloading;
				m_iCurrentFrame = m_TextureRanges["Reloading"].first;
			}
		}
		break;

	case State::Reloading:
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Reloading"].second)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_eState = State::Idle;
				m_iCurrentFrame = m_TextureRanges["Idle"].first;
			}
		}
		break;
	}
}


void CShotGun::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CShotGun::Render()
{
	/*if (m_bWall)
		m_pGameInstance->Render_Font(L"MainFont", L"벽 명중!!!", _float2(-200.f, -205.0f), _float3(1.f, 1.f, 0.0f));
	if (m_bMonster)
		m_pGameInstance->Render_Font(L"MainFont", L"몬스터 명중!!!", _float2(200.f, -205.0f), _float3(1.f, 1.f, 0.0f));*/


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
		if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
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

HRESULT CShotGun::On_Collision()
{
	


	return S_OK;
}

HRESULT CShotGun::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShotGun"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CShotGun* CShotGun::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShotGun* pInstance = new CShotGun(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("샷건 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CShotGun::Clone(void* pArg)
{
	CShotGun* pInstace = new CShotGun(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("샷건 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CShotGun::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
