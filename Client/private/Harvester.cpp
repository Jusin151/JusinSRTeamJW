#include "Harvester.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"

CHarvester::CHarvester(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CHarvester::CHarvester(const CHarvester& Prototype)
	: CRanged_Weapon(Prototype),
	m_Staff_INFO{ Prototype.m_Staff_INFO }
{
}

HRESULT CHarvester::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHarvester::Initialize(void* pArg)
{ 
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (pArg != nullptr)
		m_Staff_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_Staff_INFO.vSize.x, m_Staff_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Staff_INFO.vPos.x, m_Staff_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Staff_INFO.vPos;

	// 초기화 시 설정
	m_TextureRanges["Idle"] = { 0,0 };
	m_TextureRanges["Firing"] = { 1, 4 };
	m_TextureRanges["Reloading"] = { 5, 26 };
	
	m_Weapon_INFO.WeaponID = WEAPON_ID::Minigun;
	//m_Weapon_INFO.vPos = {};
	//m_Weapon_INFO.vSize = {};
	m_Weapon_INFO.Damage = 2;
	m_Weapon_INFO.AttackSpeed = 1.2f;

	Ranged_INFO.CurrentAmmo = 20; //현총알
	Ranged_INFO.MaxAmmo = 20;    // 최대 5
	m_fAnimationSpeed = 0.01f; //미니건에서는  발사애니메이션속도


	CItem_Manager::GetInstance()->Add_Weapon(L"Harvester", this);

	if (FAILED(Ready_Icon()))
		return E_FAIL;

	__super::Ready_Picking();

	return S_OK;
}
HRESULT CHarvester::Ready_Icon()
{
	CImage::Image_DESC Image_INFO = {};
	Image_INFO.vPos = { 200.f,150.f };
	Image_INFO.vSize = { 100.f,41.f };
	Image_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	Image_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	Image_INFO.WeaponTag = L"Harvester";
	Image_INFO.TextureImageNum = Harvester;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Image"),
		LEVEL_GAMEPLAY, TEXT("Layer_Image"), &Image_INFO)))
		return E_FAIL;

	return S_OK;
}
HRESULT CHarvester::Ready_Texture()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Harvester"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CHarvester::Priority_Update(_float fTimeDelta)
{
}

void CHarvester::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	m_fElapsedTime += fTimeDelta;

	switch (m_eState)
	{
	case State::Firing:
		if (m_fElapsedTime >= m_fAnimationSpeed * (1.f / m_Weapon_INFO.AttackSpeed)) // AttackSpeed 2면 2배라는뜻
		{
			m_fElapsedTime = 0.0f;

			if (!m_bHasFired && m_iCurrentFrame == m_TextureRanges["Firing"].first + 1)
			{
				__super::Picking_Object(5, m_Weapon_INFO.Damage);
				Ranged_INFO.CurrentAmmo--;
				Notify_Bullet();
				m_bHasFired = true;
			}


			if (m_iCurrentFrame < m_TextureRanges["Firing"].second)
				m_iCurrentFrame++;
			else
			{
				m_eState = State::Reloading;
				m_iCurrentFrame = m_TextureRanges["Reloading"].first;
			}
		}
		break;

	case State::Reloading:
		if (m_fElapsedTime >= 0.06f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < m_TextureRanges["Reloading"].second)
				m_iCurrentFrame++;
			else
			{
				m_eState = State::Idle;
				m_iCurrentFrame = m_TextureRanges["Idle"].first;
			}
		}
		break;

	case State::Idle:
		m_iCurrentFrame = m_TextureRanges["Idle"].first;
		break;
	}

	return;
}

void CHarvester::Attack_WeaponSpecific(_float fTimeDelta)
{
	if (m_eState == State::Idle && Ranged_INFO.CurrentAmmo > 0)
	{
		m_eState = State::Firing;
		m_iCurrentFrame = m_TextureRanges["Firing"].first;
		m_fElapsedTime = 0.f;
		m_bHasFired = false;
	}

}
void CHarvester::Attack(_float fTimeDelta)
{
	__super::Attack(fTimeDelta);
}

void CHarvester::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);;
}

HRESULT CHarvester::Render()
{


	//if (m_bWall)
	//	m_pGameInstance->Render_Font(L"MainFont", L"벽 명중!!!", _float2(-200.f, -205.0f), _float3(1.f, 1.f, 0.0f));
	//if (m_bMonster)
	//	m_pGameInstance->Render_Font(L"MainFont", L"몬스터 명중!!!", _float2(200.f, -205.0f), _float3(1.f, 1.f, 0.0f));


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

HRESULT CHarvester::On_Collision()
{



	return S_OK;
}


CHarvester* CHarvester::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHarvester* pInstance = new CHarvester(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("샷건 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CHarvester::Clone(void* pArg)
{
	CHarvester* pInstace = new CHarvester(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("샷건 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CHarvester::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

