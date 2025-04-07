#include "Claymore.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include "Effects.h"



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
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


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

	if (FAILED(CMelee_Weapon::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_Scale(_float3(2.f, 1.f, 2.f));

	m_pColTransformCom->Set_Scale(2.f, 1.f, 2.f);
	m_pColTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(0.f, 0.f, 0.5f));


	m_pTransformCom->Set_Scale(m_Claymore_INFO.vSize.x, m_Claymore_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Claymore_INFO.vPos.x, m_Claymore_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Claymore_INFO.vPos;
	CItem_Manager::GetInstance()->Add_Weapon(L"Claymore",this);
	


	m_eType = CG_WEAPON;
	m_iAp = 15;

	return S_OK;
}





void CClaymore::Priority_Update(_float fTimeDelta)
{
}

void CClaymore::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


 
}

void CClaymore::Late_Update(_float fTimeDelta)
{
	if (!IsActive())
		return;

	

	__super::Late_Update(fTimeDelta);
}

HRESULT CClaymore::Render()
{
	
	return __super::Render();
}

HRESULT CClaymore::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (!m_bIsAnimating)
		return S_OK;
	

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	
	

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };


	switch (other->Get_Type()) //여기서 디버깅 잡히나?
	{
	case CG_MONSTER:

		// 나중에 공격력 만들어서 추가하는 식으로
			Take_Damage(other);
			m_bAttack = true;
		break;
	case CG_STRUCTURE_WALL:
		m_bAttack = true;

	default:
		break;
	}

	if (other->Get_Type() == CG_STRUCTURE_WALL && true == m_bAttack)
	{
		// 좀 수정해야됨
		// 슬라이딩 벡터 느낌으로 구현하면 될듯?

		CTransform* pTrans = static_cast<CTransform*>(other->Get_Component(TEXT("Com_Transform")));

		// 벽의 정보
		_float3 vCenter = pTrans->Get_State(CTransform::STATE_POSITION);       // 벽 중심
		_float3 vWallRight = pTrans->Get_State(CTransform::STATE_RIGHT).GetNormalized();
		_float3 vWallUp = pTrans->Get_State(CTransform::STATE_UP).GetNormalized();
		_float3 vWallLook = pTrans->Get_State(CTransform::STATE_LOOK).GetNormalized();

		_float3 vExtents = pTrans->Compute_Scaled() * 0.5f; // 벽 반사이즈 (Right/Up/Look 방향 길이)

		// MTV와 가장 가까운 축 찾기
		_float3 vMtv = m_pColliderCom->Get_MTV().GetNormalized();

		_float fDotR = fabs(vMtv.Dot(vWallRight));
		_float fDotU = fabs(vMtv.Dot(vWallUp));
		_float fDotL = fabs(vMtv.Dot(vWallLook));

		_float3 vNormal;
		_float3 vOffset;

		if (fDotR > fDotU && fDotR > fDotL) {
			vNormal = vWallRight * (vMtv.Dot(vWallRight) > 0.f ? 1.f : -1.f);
			vOffset = vNormal * vExtents.x;
		}
		else if (fDotU > fDotL) {
			vNormal = vWallUp * (vMtv.Dot(vWallUp) > 0.f ? 1.f : -1.f);
			vOffset = vNormal * vExtents.y;
		}
		else {
			vNormal = vWallLook * (vMtv.Dot(vWallLook) > 0.f ? 1.f : -1.f);
			vOffset = vNormal * vExtents.z;
		}

		// ✅ 벽 면 중심 위치 계산
		_float3 vWallSurfacePos = vCenter + vOffset;

		// ✅ Y축에 수직인 이펙트 회전 설정
		_float3 vEffectUp = { 0.f, 1.f, 0.f };
		_float3 vEffectRight = vEffectUp.Cross(vNormal).GetNormalized();
		_float3 vEffectLook = vEffectRight.Cross(vEffectUp).GetNormalized();

		// ✅ 이펙트 세팅
		CEffect_Base::EFFECT_DESC effectDesc = {};
		effectDesc.vPos = vWallSurfacePos + vNormal * 0.01f;
		effectDesc.vRight = vEffectRight;
		effectDesc.vUp = vEffectUp;
		effectDesc.vLook = vEffectLook;
		effectDesc.vScale = { 10.f, 1.f, 10.f }; // 적당히


		m_pGameInstance->Add_GameObject(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_Weapon_Effect"),
			LEVEL_STATIC,
			TEXT("Layer_Weapon_Effect"),
			&effectDesc);
	}

	

	return S_OK;
}

HRESULT CClaymore::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Claymore"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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
void CClaymore::Attack(_float fTimeDelta)
{
	if (m_bIsAnimating) return;

		m_bIsAnimating = true;
		m_bAttack = false;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
		m_iLastFrame = 13;
}

void CClaymore::Attack_WeaponSpecific(_float fTimeDelta)
{


}
