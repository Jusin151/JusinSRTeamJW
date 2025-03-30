#include "Magnum.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include "Sound_Source.h"
#include "Light.h"



CMagnum::CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CMagnum::CMagnum(const CMagnum& Prototype)
	: CRanged_Weapon(Prototype),
	m_Magnum_INFO{ Prototype.m_Magnum_INFO }
{
	
}

HRESULT CMagnum::Initialize_Prototype()
{
 

	return S_OK;
}

HRESULT CMagnum::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
		m_Magnum_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;

	

 	m_pTransformCom->Set_Scale(m_Magnum_INFO.vSize.x, m_Magnum_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Magnum_INFO.vPos.x, m_Magnum_INFO.vPos.y, 0.0f));

	m_vInitialPos.x = m_Magnum_INFO.vPos.x;
	m_vInitialPos.y = m_Magnum_INFO.vPos.y;
	CItem_Manager::GetInstance()->Add_Weapon(L"Magnum", this);


	CImage::Image_DESC Image_INFO = {};
	Image_INFO.vPos = { -100.f,150.f };
	Image_INFO.vSize = { 74.f,31.f };
	Image_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
	Image_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
	Image_INFO.WeaponTag = L"Magnum";
	Image_INFO.TextureImageNum = Magnum;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Image"),
		LEVEL_GAMEPLAY, TEXT("Layer_Image"), &Image_INFO)))
		return E_FAIL;

	__super::Ready_Picking();

	return S_OK;
}

void CMagnum::Priority_Update(_float fTimeDelta)
{
}

void CMagnum::Update(_float fTimeDelta)
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
void CMagnum::Attack(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_bHasFired)
		{
			//m_pGameInstance->Play_Event(L"event:/magnum_shot").SetVolume(0.5f);
			m_bIsAnimating = true;
			m_iCurrentFrame = 0;
			m_fElapsedTime = 0.0f;
			m_bHasFired = true; // 발사 상태를 true로 설정
			__super::Picking_Object(1); // 클릭 한 번에 한 번만 호출
			CUI_Manager::GetInstance()->Set_Pistol_Bullet(1);
		}
	}
	else
	{
		m_bHasFired = false; // 버튼을 떼면 다시 발사 가능하도록 설정
	}

	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 4) // 몇장까지인지
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
				m_iCurrentFrame = 0;
			}
		}
	}
}


void CMagnum::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_bIsAnimating)
	{
		m_pGameInstance->Add_Light(m_pLightCom);
	}
}

HRESULT CMagnum::Render()
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

HRESULT CMagnum::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Magnum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;


	//CLight::LIGHT_DESC lDesc = {};
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
		TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom))))
		return E_FAIL;

	//CSound_Source::LIGHT_DESC lDesc = {};
	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
		TEXT("Com_Sound_Source"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;*/

	return S_OK;
}

CMagnum* CMagnum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagnum* pInstance = new CMagnum(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("매그넘 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CMagnum::Clone(void* pArg)
{
	CMagnum* pInstace = new CMagnum(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("매그넘 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CMagnum::Free()
{
	__super::Free();
	Safe_Release(m_pPickingSys);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pLightCom);
	Safe_Release(m_pSoundCom);
}
