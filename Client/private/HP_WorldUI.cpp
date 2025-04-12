#include "HP_WorldUI.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "Material.h"
#include <Player.h>


CHP_WorldUI::CHP_WorldUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CHP_WorldUI::CHP_WorldUI(const CHP_WorldUI& Prototype)
	:CUI_Base(Prototype),
	m_bIsCloned(true),
	m_pShaderCom{ Prototype.m_pShaderCom },
	m_pMaterialCom{ Prototype.m_pMaterialCom },
	m_pBarTextureCom{ Prototype.m_pBarTextureCom }
{
	m_pTextureCom = Prototype.m_pTextureCom;
	m_pVIBufferCom = Prototype.m_pVIBufferCom;
}

HRESULT CHP_WorldUI::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CHP_WorldUI::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(2.f,0.2f, 1.f);
	return S_OK;
}

void CHP_WorldUI::Priority_Update(_float fTimeDelta)
{
	
}

void CHP_WorldUI::Update(_float fTimeDelta)
{
	m_fFloatingTime += fTimeDelta;
	if (m_fFloatingTime >= 1.5f)
	{
		m_fFloatingTime = 0.f;
		SetActive(false);
	}
}

void CHP_WorldUI::Late_Update(_float fTimeDelta)
{
	Setting_Position();
	Billboarding(fTimeDelta);
	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CHP_WorldUI::Render()
{

	m_pTransformCom->Set_Scale(2.17f, 0.2f, 1.f);

	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fOffsetAmount = 0.1f;
	_float3 vOffsetX = vRight * fOffsetAmount;
	_float3 vOffsetY = vUp * -fOffsetAmount;
	_float3 vOffsetZ = vLook * fOffsetAmount;

	// 현재 위치를 가져온 후 오프셋을 적용합니다.
	_float3 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vCurPos += vOffsetZ+ vOffsetY; 
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);\

	if (FAILED(m_pTextureCom->Bind_Resource(1)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;
	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom,1)))
		return E_FAIL;
	m_pShaderCom->Begin(0);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();

	return S_OK;
}

void CHP_WorldUI::Reset()
{
	m_fFloatingTime = 0.f;
}

HRESULT CHP_WorldUI::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP_Bar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHP_WorldUI::SetUp_RenderState()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);

	m_pShaderCom->Set_Bool("g_bUseTiling", false);
	m_pShaderCom->Set_Bool("g_bUseHPClip", true);
	m_pShaderCom->Bind_Float("g_HpRatio", m_fHpRatio);


	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	return S_OK;
}

HRESULT CHP_WorldUI::Release_RenderState()
{
	m_pShaderCom->Set_Bool("g_bUseHPClip", false);

	m_pShaderCom->Set_Bool("g_bUseTiling", true);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CHP_WorldUI::Setting_Position()
{
	if (m_pOwner)
	{
 		auto pOwnerTrans = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")));
		if (pOwnerTrans)
		{
			auto vOwnerPos = pOwnerTrans->Get_State(CTransform::STATE_POSITION);
			vOwnerPos.y += 1.2f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOwnerPos);
		}
	}

}

void CHP_WorldUI::Billboarding(_float fTimeDelta)
{
	CTransform* pOwnerTransform = static_cast<CTransform*>(m_pOwner->Get_Component(TEXT("Com_Transform")));

	if (!pOwnerTransform) return;
	_float3 vOwnerLook = pOwnerTransform->Get_State(CTransform::STATE_LOOK);

	vOwnerLook.y = 0.f;
	vOwnerLook.Normalize();

	_float3 vLook = vOwnerLook;


	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	_float3 vRight = vUp.Cross(vLook);
	vRight.Normalize();

	_float3 vNewUp = vLook.Cross(vRight);
	vNewUp.Normalize();

	_float3 vScale = m_pTransformCom->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}

void CHP_WorldUI::OnNotify(void* pArg, const wstring& type)
{
	if (pArg)
	{
		m_fHpRatio = *static_cast<_float*>(pArg);
		this->m_bIsActive = true;
	}
}

CHP_WorldUI* CHP_WorldUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHP_WorldUI* pInstance = new CHP_WorldUI(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CHP_WorldUI instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHP_WorldUI::Clone(void* pArg)
{
	CHP_WorldUI* pClone = new CHP_WorldUI(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CHP_WorldUI instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CHP_WorldUI::Free()
{
	__super::Free();

	if (!m_bIsCloned)
	{
		Safe_Release(m_pMaterialCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pBarTextureCom);
	}
	Safe_Release(m_pTransformCom);
}