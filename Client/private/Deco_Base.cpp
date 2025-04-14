#include "Deco_Base.h"
#include "GameInstance.h"
#include "Player.h"

CDeco_Base::CDeco_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollisionObject{ pGraphic_Device }
{
}

CDeco_Base::CDeco_Base(const CDeco_Base& Prototype)
	: CCollisionObject{ Prototype }
{
}

HRESULT CDeco_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeco_Base::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	INIT_PARENT(pArg)
	m_eType = CG_STRUCTURE_WALL;



	return S_OK;
}

HRESULT CDeco_Base::Ready_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f,0.5f,0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

void CDeco_Base::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pTarget)
			return;

		m_pTarget = pTarget;
		Safe_AddRef(pTarget);

	}
}

void CDeco_Base::Update(_float fTimeDelta)
{

}

void CDeco_Base::Late_Update(_float fTimeDelta)
{

}

HRESULT CDeco_Base::Render()
{
	return S_OK;
}



void CDeco_Base::Look_Player()
{
	if (!m_pTarget||(TEXT("Layer_Player") != m_pTarget->Get_Tag()))
		return;
	CTransform* pTrans = static_cast<CPlayer*>(m_pTarget)->Get_TransForm();

	_float3 vScaled = m_pTransformCom->Compute_Scaled(); // 스케일값 저장

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 위치 저장

	_float3 vLook = pTrans->Get_State(CTransform::STATE_POSITION) - vPosition; // 방향 벡터 (Look)
	vLook = vLook.GetNormalized(); // 정규화

	_float3 vUp = { 0.f, 1.f, 0.f }; // Y축 고정

	_float3 vRight = {};
	D3DXVec3Cross(&vRight, &vUp, &vLook); // Up이 고정이니까 Right만 외적해서 구함
	vRight = vRight.GetNormalized();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScaled.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * vScaled.y); // 고정된 Up 벡터
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScaled.z);

}

json CDeco_Base::Serialize()
{
	json j = __super::Serialize();

	auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto scale = m_pTransformCom->Compute_Scaled();
	auto angle = m_pTransformCom->Get_EulerAngles();
	j["position"] = {
	RoundToDecimalPlaces(pos.x, 2),
	RoundToDecimalPlaces(pos.y, 2),
	RoundToDecimalPlaces(pos.z, 2)
	};
	j["rotation"] = {
		RoundToDecimalPlaces(angle.x, 2),
		RoundToDecimalPlaces(angle.y, 2),
		RoundToDecimalPlaces(angle.z, 2)
	};
	j["scale"] = {
		RoundToDecimalPlaces(scale.x, 2),
		RoundToDecimalPlaces(scale.y, 2),
		RoundToDecimalPlaces(scale.z, 2)
	};

	return j;
}

void CDeco_Base::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);
}



void CDeco_Base::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pShaderCom);
}
