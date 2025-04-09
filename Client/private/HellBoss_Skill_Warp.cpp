#include "HellBoss_Skill_Warp.h"
#include "HellBoss.h"

CHellBoss_Skill_Warp::CHellBoss_Skill_Warp(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CHellBoss_SkillBase{pGraphic_Device}
{
}

CHellBoss_Skill_Warp::CHellBoss_Skill_Warp(const CHellBoss_Skill_Warp& Prototype)
	:CHellBoss_SkillBase( Prototype )
{
}

HRESULT CHellBoss_Skill_Warp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_Skill_Warp::Initialize(void* pArg)
{
	if(FAILED(Ready_Components()))
		return E_FAIL;

	BossDESC m_Skill_Info{};

	m_Skill_Info = *reinterpret_cast<BossDESC*>(pArg);


	 _float3 Skill_vecPos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT); 
	 _float3 Skill_vecUp = m_pTransformCom->Get_State(CTransform::STATE_UP); 
	 _float3 Skill_vecLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK); 
	  

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Skill_vecPos); 
	m_pTransformCom->Set_State(CTransform::STATE_UP, Skill_vecUp); 
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Skill_vecLook); 

	m_pTransformCom->Set_Scale(20.f, 30.f, 10.f);
	
	m_AnimationManager.AddAnimation("Warp", 0, 16);
	

	return S_OK;
}


HRESULT CHellBoss_Skill_Warp::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Skill_Warp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CHellBoss_Skill_Warp::Priority_Update(_float fTimeDelta)
{
	static _bool bInit = { false };

	if (!bInit)
	{

		m_AnimationManager.SetCurrentAnimation("Warp");

		bInit = true;
	}
}

void CHellBoss_Skill_Warp::Update(_float fTimeDelta)
{


	m_AnimationManager.Update(fTimeDelta); 
	
		

	if (m_AnimationManager.IsFinished())
	{
		SetActive(false);
	}
}

void CHellBoss_Skill_Warp::Process_Input()
{
}

void CHellBoss_Skill_Warp::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
}

HRESULT CHellBoss_Skill_Warp::Render()
{
	_int iCurFrame = m_AnimationManager.GetCurrentFrame();


	if (FAILED(m_pTextureCom->Bind_Resource(iCurFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	//if (FAILED(m_pParticleCom->Render()))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CHellBoss_Skill_Warp::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHellBoss_Skill_Warp::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}


CHellBoss_Skill_Warp* CHellBoss_Skill_Warp::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_Skill_Warp* pInstance = new CHellBoss_Skill_Warp(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CHellBoss_Skill_Warp");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_Skill_Warp::Clone(void* pArg) 
{
	CHellBoss_Skill_Warp* pInstance = new CHellBoss_Skill_Warp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CHellBoss_Skill_Warp");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_Skill_Warp::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
 
}
