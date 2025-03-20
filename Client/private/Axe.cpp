#include "Axe.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "Item_Manager.h"

CAxe::CAxe(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMelee_Weapon(pGraphic_Device)
{
}

CAxe::CAxe(const CAxe& Prototype)
	: CMelee_Weapon(Prototype),
	m_Axe_INFO{Prototype.m_Axe_INFO }
{
}

HRESULT CAxe::Initialize_Prototype()
{


	if (FAILED(CMelee_Weapon::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CAxe::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_Axe_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(CMelee_Weapon::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_Scale(_float3(3.f, 0.5f, 3.f));

	m_pColTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pColTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(0.f, 0.f, 0.5f));


	m_pTransformCom->Set_Scale(m_Axe_INFO.vSize.x, m_Axe_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Axe_INFO.vPos.x, m_Axe_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Axe_INFO.vPos;




	CItem_Manager::GetInstance()->Add_Weapon(L"Axe", this);

	CItem_Icon::Icon_DESC Axe_Icon{};
	Axe_Icon.Icon_Image = Axe; // 초기 이미지
	Axe_Icon.Weapon_Type = CItem_Icon::Axe; // 선택되고 나서 되돌릴 이미지
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Icon"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Icon_Axe"),&Axe_Icon)))
		return E_FAIL;


	return S_OK;
}

void CAxe::Priority_Update(_float fTimeDelta)
{
}

void CAxe::Update(_float fTimeDelta)
{

		static bool bMotionPlayed = false;
		static float fElapsedTime = 0.0f;

		if (m_bIsActive && !bMotionPlayed)
		{
			fElapsedTime += fTimeDelta;

			if (fElapsedTime <= 0.5f) // 0.5초 동안 y축을 50만큼 올리기
			{
				float t = fElapsedTime / 0.5f;
				float yOffset = 50.0f * t;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION,
					_float3(m_Axe_INFO.vPos.x, m_vInitialPos.y + yOffset, 0.f));
			}
			else if (fElapsedTime <= 1.0f) // 0.5초 동안 반동을 줘서 다시 돌아오기
			{
				float t = (fElapsedTime - 0.5f) / 0.5f;
				float yOffset = 50.0f * (1.0f - t);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION,
					_float3(m_Axe_INFO.vPos.x, m_vInitialPos.y + yOffset, 0.f));
			}
			else // 모션 완료
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION,
					_float3(m_Axe_INFO.vPos.x, m_vInitialPos.y, 0.f));
				bMotionPlayed = true;
			}
		}

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

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_bIsAnimating = true;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}
	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 11)
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

	return;

}

void CAxe::Late_Update(_float fTimeDelta)
{
	if(m_bIsAnimating)
		On_Collision();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CAxe::Render()
{
	

	return __super::Render();
}

HRESULT CAxe::On_Collision()
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	// 안바뀌면 충돌 안일어남
	if (m_pColliderCom->Get_Other_Type() == CG_END)
		return S_OK;

	

	switch (m_pColliderCom->Get_Other_Type()) //여기서 디버깅 잡히나?
	{
	case CG_MONSTER:

		// 나중에 공격력 만들어서 추가하는 식으로
	
		break;

	default:
		break;
	}

	// 충돌 처리 하고 다시 type을 수정
	m_pColliderCom->Set_Other_Type(CG_END);

	return S_OK;
}

HRESULT CAxe::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Axe"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Orth"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CAxe* CAxe::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAxe* pInstance = new CAxe(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("도끼 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CAxe::Clone(void* pArg)
{
	CAxe* pInstace = new CAxe(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("도끼 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CAxe::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
