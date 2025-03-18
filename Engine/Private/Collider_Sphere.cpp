#include "Collider_Sphere.h"
#include "GameObject.h"
#include "Transform.h"

CCollider_Sphere::CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Sphere::CCollider_Sphere(const CCollider_Sphere& Prototype)
	:CCollider(Prototype)
{
}

HRESULT CCollider_Sphere::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void* pArg)
{
    COL_DESC* tDesc = {};
    tDesc = static_cast<COL_DESC*>(pArg);

    m_fLocalPos = tDesc->fLocalPos;
    m_fScale = tDesc->fScale;
    m_pOwner = tDesc->pOwner;
    m_eType = tDesc->eType;

    m_fRadius = m_fScale.Length() / 4.f;

    D3DXMatrixIdentity(&m_WorldMatrix);

	return S_OK;
}

HRESULT CCollider_Sphere::Render()
{
    // 현재 상태를 저장
    DWORD dwFillMode, dwLighting;
    m_pGraphic_Device->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
    m_pGraphic_Device->GetRenderState(D3DRS_LIGHTING, &dwLighting);

    // 월드 매트릭스 저장
    _float4x4 matOldWorld;

    m_pGraphic_Device->GetTransform(D3DTS_WORLD, &matOldWorld);

    // 반지름 확인 및 임시 조정 (디버깅용)
    _float fRenderRadius = 5.f;



    LPD3DXMESH pSphereMesh = nullptr;
    if (FAILED(D3DXCreateSphere(m_pGraphic_Device, fRenderRadius, 10, 10, &pSphereMesh, nullptr)))
        return E_FAIL;

    // 렌더 상태 설정
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // Z 버퍼 쓰기 활성화

    // 색상 설정 (콜라이더 색상을 구분하기 위해)
    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 255, 0));
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    _float4x4 matWorld = m_WorldMatrix;
     // 스케일 성분 1로 설정 (스케일 제거)
    // matWorld._11 = 1.0f;
    //matWorld._22 = 1.0f;
    //matWorld._33 = 1.0f;
    // 월드 매트릭스 설정
    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &matWorld);

    // 메시 렌더링
    pSphereMesh->DrawSubset(0);

    // 원래 상태로 복원
    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &matOldWorld);
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, dwFillMode);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    // 메시 해제
    Safe_Release(pSphereMesh);

    return S_OK;
}

HRESULT CCollider_Sphere::Update_Collider()
{
    if (nullptr == m_pOwner)
        return E_FAIL;


    // owner 가 가지고 있는 월드 행렬을 이용해서 collider의 월드 행렬을 만든다
    CTransform* pTransfrom = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")));

    

    _float3 fRight = pTransfrom->Get_State(CTransform::STATE_RIGHT).GetNormalized();
    _float3 fUp = pTransfrom->Get_State(CTransform::STATE_UP).GetNormalized();
    _float3 fLook = pTransfrom->Get_State(CTransform::STATE_LOOK).GetNormalized();
    _float3 fObjectPos = pTransfrom->Get_State(CTransform::STATE_POSITION);

    Set_State(CTransform::STATE_RIGHT, fRight * m_fScale.x);
    Set_State(CTransform::STATE_UP, fUp * m_fScale.y);
    Set_State(CTransform::STATE_LOOK, fLook * m_fScale.z);
    Set_State(CTransform::STATE_POSITION, fObjectPos);
   
    

    D3DXVec3TransformCoord(&m_fPos, &m_fLocalPos, &m_WorldMatrix);
    Set_State(CTransform::STATE_POSITION, m_fPos);

    m_fRadius = m_fScale.LengthSq() / 4.f;

    return S_OK;
}
	

CCollider_Sphere* CCollider_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Sphere::Clone(void* pArg)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Sphere::Free()
{
	__super::Free();
}
