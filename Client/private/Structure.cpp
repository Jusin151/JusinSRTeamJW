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
{
}

HRESULT CStructure::Initialize_Prototype()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	return S_OK;
}

HRESULT CStructure::Initialize(void* pArg)
{
	INIT_PARENT(pArg)

		if (FAILED(Ready_Components()))
			return E_FAIL;


	m_bIsCubeCollider = (dynamic_cast<CCollider_Cube*>(m_pColliderCom) != nullptr);

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
		(m_pColliderCom)->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
	}
	m_pGameInstance->Add_Collider(CG_STRUCTURE, m_pColliderCom);


	if (m_eStructureType == STRUCTURE_TYPE::MAGMA)
	{
		m_fFrame += 90.f * fTimeDelta;
		if (m_fFrame >= 90.f)
			m_fFrame = 0.f;

		m_iCurrentTexture = (_uint)(m_fFrame / 22.5f); 
	}
}

void CStructure::Late_Update(_float fTimeDelta)
{
	m_fWaveTime += fTimeDelta;

	if (m_fWaveTime > 1000.0f)
		m_fWaveTime = 0.f;
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CStructure::Render()
{
	if (FAILED(m_pMaterialCom->Bind_Resource()))
		return E_FAIL;
	if (m_eStructureType == STRUCTURE_TYPE::MAGMA)
	{
		if (FAILED(m_pTextureCom->Bind_Resource(static_cast<_int>(m_iCurrentTexture))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_Resource(0)))
			return E_FAIL;
	}

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

	// 현재 샘플러 상태 값을 저장하여 렌더링 후 복원할 수 있도록 함
	m_pGraphic_Device->GetSamplerState(0, D3DSAMP_ADDRESSU, &originalAddressU);
	m_pGraphic_Device->GetSamplerState(0, D3DSAMP_ADDRESSV, &originalAddressV);
	_float4x4 matTexture;
	// 텍스처 좌표가 0-1 범위를 벗어날 때 반복되도록 샘플러 상태 설정
	// D3DTADDRESS_WRAP: UV 좌표가 1을 초과하면 다시 0부터 시작 (텍스처 반복)
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	_float3 scale = m_pTransformCom->Compute_Scaled();

	// 보스 맵 물결 효과 
	if (m_eStructureType==STRUCTURE_TYPE::BOSS_FLOOR)
	{
		_float fOffsetU = sin(m_fWaveTime * m_fWaveSpeed) * 0.5f;
		_float fOffsetV = cos(m_fWaveTime * m_fWaveSpeed) * 0.5f;

		D3DXVECTOR2 vScaleFactor(scale.x, scale.y);
		D3DXVECTOR2 vOffsetFactor(fOffsetU, fOffsetV);

		// 텍스처 변환 행렬 생성
		D3DXMatrixTransformation2D(&matTexture, NULL, 0.0f,
			&vScaleFactor, // 스케일
			NULL, 0.0f,
			&vOffsetFactor); // 오프셋
	}
	else if (m_eStructureType == STRUCTURE_TYPE::BOSS_WALL)
	{
		_float tileReduction = 0.10f; // 타일링을 1/10로 줄임
		D3DXVECTOR2 vScaleFactor(scale.x * 0.3f, -scale.y * tileReduction);
		D3DXVECTOR2 vOffsetFactor(0.5f, 0.5f); // Y축 반전을 위한 오프셋 조정

		D3DXMatrixTransformation2D(&matTexture, NULL, 0.0f,
			&vScaleFactor, NULL, 0.0f, &vOffsetFactor);
	}
	else
	{
#pragma region 텍스쳐 스케일에 따라 반복
	//	D3DXMatrixScaling(&matTexture, scale.x, scale.y, 1.0f);
		D3DXVECTOR2 vScaleFactor(scale.x, -scale.y);
		D3DXVECTOR2 vOffsetFactor(0.5f, 0.5f); // Y축 반전을 위한 오프셋 조정

		D3DXMatrixTransformation2D(&matTexture, NULL, 0.0f,
			&vScaleFactor, NULL, 0.0f, &vOffsetFactor);
#pragma endregion
	}

	// 스케일 값으로 텍스처 변환 행렬 생성
		// scale.x > 1: X축으로 텍스처가 여러 번 반복됨
		// scale.y > 1: Y축으로 텍스처가 여러 번 반복됨
		// scale.x 또는 y < 1: 텍스처의 일부만 표시됨


	m_pGraphic_Device->SetTransform(D3DTS_TEXTURE0, &matTexture);
	// 텍스처 변환을 활성화 (2D 텍스처 좌표에 대해)
// D3DTTFF_COUNT2: u, v 두 좌표에 대해 변환 적용 (2D)
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CStructure::Release_RenderState()
{
	// 기본 컬링 모드로 복원 (반시계 방향 면만 렌더링 - 일반적인 앞면)
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// 알파 테스트 비활성화
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// 텍스처 변환 비활성화 - 다른 객체 렌더링에 영향을 주지 않도록 함
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	// 원래 샘플러 상태로 복원
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, originalAddressU);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, originalAddressV);
	return S_OK;
}

HRESULT CStructure::Ready_Components()
{
	//_wstring str = m_tStructure_Desc.stTextureTag;
	//if (FAILED(m_pGameInstance->Find_Prototype(str)))
	//{
	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY,
	//		m_tStructure_Desc.stTextureTag,
	//		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_2D,
	//			m_tStructure_Desc.stTexturePath.c_str(), 1))))
	//		return E_FAIL;
	//}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_tObjDesc.iProtoLevel, m_tObjDesc.stProtTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (m_tObjDesc.stProtTextureTag.find(L"AntFloor") != wstring::npos)
		m_eStructureType = STRUCTURE_TYPE::BOSS_FLOOR;
	else if (m_tObjDesc.stProtTextureTag.find(L"BossWall") != wstring::npos)
		m_eStructureType = STRUCTURE_TYPE::BOSS_WALL;
	else if (m_tObjDesc.stProtTag.find(L"Magma") != wstring::npos)
		m_eStructureType = STRUCTURE_TYPE::MAGMA;
	else
		m_eStructureType = STRUCTURE_TYPE::NORMAL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tObjDesc.stBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.eType = CG_STRUCTURE;
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
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
	Safe_Release(m_pMaterialCom);
}

json CStructure::Serialize()
{
	json j = __super::Serialize();


	//j["texture_tag"] = WideToUtf8(m_strTextureTag);
	//j["texture_path"] = WideToUtf8(m_tStructure_Desc.stTexturePath); 
	//j["vibuffer"] = WideToUtf8(m_strVIBuffer);
	//j["collider_tag"] = WideToUtf8(m_strCollProtoTag);


	j["is_cube_collider"] = m_bIsCubeCollider;

	auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto scale = m_pTransformCom->Compute_Scaled();
	auto angle = m_pTransformCom->Get_EulerAngles();
	j["position"] = { pos.x, pos.y, pos.z };
	j["rotation"] = { angle.x, angle.y, angle.z };
	j["scale"] = { scale.x, scale.y, scale.z };

	return j;
}

void CStructure::Deserialize(const json& j)
{

	SET_TRANSFORM(j, m_pTransformCom);
	//if (j.contains("position")) 
	//{
	//	auto pos = j["position"];
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
	//		_float3(pos[0], pos[1], pos[2]));
	//}

	//if (j.contains("rotation"))
	//{
	//	auto angle = j["rotation"];
	//	m_pTransformCom->Rotate_EulerAngles(
	//		_float3(angle[0], angle[1], angle[2]));
	//}

	//if (j.contains("scale"))
	//{
	//	auto scale = j["scale"];
	//	m_pTransformCom->Set_Scale(scale[0], scale[1], scale[2]);
	//}
}
