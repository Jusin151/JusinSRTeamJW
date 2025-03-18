#include "Structure.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Collider_Cube.h"

CStructure::CStructure(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

CStructure::CStructure(const CStructure& Prototype)
	:CGameObject{ Prototype }
	, m_tStructure_Desc(Prototype.m_tStructure_Desc)
{
}

HRESULT CStructure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStructure::Initialize(void* pArg)
{
	STRUCTURE_DESC* tStructureDesc = static_cast<STRUCTURE_DESC*>(pArg);

	if (tStructureDesc)
	{
		m_tStructure_Desc = *tStructureDesc;

		if (m_tStructure_Desc.stVIBuffer)
			m_strVIBuffer = m_tStructure_Desc.stVIBuffer;

		if (m_tStructure_Desc.stCollProtoTag)
			m_strCollProtoTag = m_tStructure_Desc.stCollProtoTag;

		if (m_tStructure_Desc.stTextureTag)
			m_strTextureTag = m_tStructure_Desc.stTextureTag;

		// 기존 텍스처 경로는 이미 wstring이므로 그대로 복사
		m_tStructure_Desc.stTexturePath = m_tStructure_Desc.stTexturePath;

		// 복사된 wstring의 c_str() 포인터를 할당
		m_tStructure_Desc.stVIBuffer = m_strVIBuffer.c_str();
		m_tStructure_Desc.stCollProtoTag = m_strCollProtoTag.c_str();
		m_tStructure_Desc.stTextureTag = m_strTextureTag.c_str();
	}
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_Type(CG_STRUCTURE);
	m_pColliderCom->Set_Owner(this);
	m_bIsCubeCollider = (dynamic_cast<CCollider_Cube*>(m_pColliderCom) != nullptr);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tStructure_Desc.vPos);
	return S_OK;
}

void CStructure::Priority_Update(_float fTimeDelta)
{
}

void CStructure::Update(_float fTimeDelta)
{
	m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

	if (m_bIsCubeCollider)
	{
		static_cast<CCollider_Cube*>(m_pColliderCom)->Update_Desc();;
	}
	m_pGameInstance->Add_Collider(CG_STRUCTURE, m_pColliderCom);
}

void CStructure::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CStructure::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	return S_OK;
}

HRESULT CStructure::On_Collision(_float fTimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CStructure::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);*/

	return S_OK;
}

HRESULT CStructure::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CStructure::Ready_Components()
{
	_wstring str = m_tStructure_Desc.stTextureTag;
	if (FAILED(m_pGameInstance->Find_Prototype(str)))
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
			m_tStructure_Desc.stTextureTag,
			CTexture::Create(m_pGraphic_Device,
				m_tStructure_Desc.stTexturePath.c_str(), 1))))
			return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tStructure_Desc.stTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tStructure_Desc.stVIBuffer,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tStructure_Desc.stCollProtoTag,
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom))))
		return E_FAIL;

	return S_OK;
}

CStructure* CStructure::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStructure* pInstance = new CStructure(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStructure");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStructure::Clone(void* pArg)
{
	CStructure* pInstance = new CStructure(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CStructure");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStructure::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}

json CStructure::Serialize()
{
	json j = __super::Serialize();

	j["texture_tag"] = WideToUtf8(m_strTextureTag);
	j["texture_path"] = WideToUtf8(m_tStructure_Desc.stTexturePath); 
	j["vibuffer"] = WideToUtf8(m_strVIBuffer);
	j["collider_tag"] = WideToUtf8(m_strCollProtoTag);


	j["is_cube_collider"] = m_bIsCubeCollider;

	auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto scale = m_pTransformCom->Compute_Scaled();
	auto angle = m_pTransformCom->Get_EulerAngles();
	j["transform"] =
	{
		{"position", pos.x, pos.y, pos.z },
		{"rotation" ,angle.x, angle.y, angle.z },
		{"scale", scale.x, scale.y, scale.z }
	};

	return j;
}

void CStructure::Deserialize(const json& j)
{

	__super::Deserialize(j);
	// 컴포넌트 정보 설정
	if (j.contains("texture_tag")) {
		m_strTextureTag = Utf8ToWide(j["texture_tag"].get<string>());
	}

	if (j.contains("texture_path")) {
		m_tStructure_Desc.stTexturePath = Utf8ToWide(j["texture_path"].get<string>());
	}

	if (j.contains("vibuffer")) {
		m_strVIBuffer = Utf8ToWide(j["vibuffer"].get<string>());
	}

	if (j.contains("collider_tag")) {
		m_strCollProtoTag = Utf8ToWide(j["collider_tag"].get<string>());
	}

	// 트랜스폼 정보 설정
	if (j.contains("transform")) {
		auto& transform = j["transform"];

		// 위치 설정
		if (transform[0][0] == "position") {
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(
				transform[0][1].get<float>(),
				transform[0][2].get<float>(),
				transform[0][3].get<float>()
			));
		}

		// 회전 설정
		if (transform[1][0] == "rotation") {
			_float3 vRotation(
				transform[1][1].get<float>(),
				transform[1][2].get<float>(),
				transform[1][3].get<float>()
			);
			m_pTransformCom->Rotate_EulerAngles(vRotation);
		}

		// 크기 설정
		if (transform[2][0] == "scale") {
			_float3 vScale(
				transform[2][1].get<float>(),
				transform[2][2].get<float>(),
				transform[2][3].get<float>()
			);
			m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
		}
	}
}
