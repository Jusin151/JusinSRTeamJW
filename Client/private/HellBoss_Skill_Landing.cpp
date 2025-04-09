#include "HellBoss_Skill_Landing.h"
#include "HellBoss.h"

CHellBoss_Skill_Landing::CHellBoss_Skill_Landing(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CHellBoss_SkillBase{pGraphic_Device}
{
}

CHellBoss_Skill_Landing::CHellBoss_Skill_Landing(const CHellBoss_Skill_Landing& Prototype)
	:CHellBoss_SkillBase( Prototype )
{
}

HRESULT CHellBoss_Skill_Landing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_Skill_Landing::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	BossDESC m_Skill_Info = *reinterpret_cast<BossDESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_Skill_Info.vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_Skill_Info.vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_Skill_Info.vLook);

	m_Skill_Info.vPos.y -= 4.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Skill_Info.vPos);

	m_pTransformCom->Set_Scale(20.f,20.f,20.f);

	m_AnimationManager.AddAnimation("Up", 0, 7);
	m_AnimationManager.AddAnimation("Down", 8, 18);

	 m_AnimationManager.SetCurrentAnimation(m_Skill_Info.strState);


	return S_OK;
}


HRESULT CHellBoss_Skill_Landing::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Skill_Landing"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CHellBoss_Skill_Landing::Priority_Update(_float fTimeDelta)
{
	
}

void CHellBoss_Skill_Landing::Update(_float fTimeDelta)
{

	

	m_AnimationManager.Update(fTimeDelta); 


	if (m_AnimationManager.IsFinished())
	{
		SetActive(false);
	}
}

void CHellBoss_Skill_Landing::Process_Input()
{
}

void CHellBoss_Skill_Landing::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CHellBoss_Skill_Landing::Render()
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

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스킬 위치 X:") + to_wstring(m_pTransformCom->Get_WorldMat()._41),
		_float2(-300.f, -300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스킬 위치 Y:") + to_wstring(m_pTransformCom->Get_WorldMat()._42),
		_float2(-100.f, -300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("스킬 위치 Z:") + to_wstring(m_pTransformCom->Get_WorldMat()._43),
		_float2(100.f, -300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	return S_OK;
}

HRESULT CHellBoss_Skill_Landing::SetUp_RenderState()
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

HRESULT CHellBoss_Skill_Landing::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}


CHellBoss_Skill_Landing* CHellBoss_Skill_Landing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_Skill_Landing* pInstance = new CHellBoss_Skill_Landing(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CHellBoss_Skill_Landing");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_Skill_Landing::Clone(void* pArg) 
{
	CHellBoss_Skill_Landing* pInstance = new CHellBoss_Skill_Landing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CHellBoss_Skill_Landing");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_Skill_Landing::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
 
}
