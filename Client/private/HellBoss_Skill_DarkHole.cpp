#include "HellBoss_Skill_DarkHole.h"
#include "HellBoss.h"

CHellBoss_Skill_DarkHole::CHellBoss_Skill_DarkHole(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CHellBoss_SkillBase{pGraphic_Device}
{
}

CHellBoss_Skill_DarkHole::CHellBoss_Skill_DarkHole(const CHellBoss_Skill_DarkHole& Prototype)
	:CHellBoss_SkillBase( Prototype )
{
}

HRESULT CHellBoss_Skill_DarkHole::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_Skill_DarkHole::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	BossDESC m_Skill_Info = *reinterpret_cast<BossDESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_Skill_Info.vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_Skill_Info.vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_Skill_Info.vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_Skill_Info.vPos);

	m_pTransformCom->Set_Scale(5.f,5.f,5.f);

	m_AnimationManager.AddAnimation("Warp", 0, 16);

	return S_OK;
}


HRESULT CHellBoss_Skill_DarkHole::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss_Skill_DarkHole"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CHellBoss_Skill_DarkHole::Priority_Update(_float fTimeDelta)
{
	static _bool bInit = { false };

	if (!bInit)
	{
		if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_HONG, TEXT("Prototype_GameObject_HellBoss_DarkBullet"), TEXT("Layer_HellBoss_DarkBullet"), 30)))
			return;

		bInit = true;
	}

}

void CHellBoss_Skill_DarkHole::Update(_float fTimeDelta)
{

	m_AnimationManager.SetCurrentAnimation("Warp");

	m_AnimationManager.Update(fTimeDelta); 
	
	if (m_AnimationManager.GetCurrentFrame() == 7)
	{
		if (!m_pGameInstance->Add_GameObject_FromPool(LEVEL_HONG, LEVEL_HONG, TEXT("Layer_HellBoss_DarkBullet")))
		{
			MSG_BOX("다크뷸릿 생성안됨");
		}

	}

	if (m_AnimationManager.IsFinished())
	{
		SetActive(false);
	}
}

void CHellBoss_Skill_DarkHole::Process_Input()
{
}

void CHellBoss_Skill_DarkHole::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta); 
}

HRESULT CHellBoss_Skill_DarkHole::Render()
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

HRESULT CHellBoss_Skill_DarkHole::SetUp_RenderState()
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

HRESULT CHellBoss_Skill_DarkHole::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	return S_OK;
}


CHellBoss_Skill_DarkHole* CHellBoss_Skill_DarkHole::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_Skill_DarkHole* pInstance = new CHellBoss_Skill_DarkHole(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CHellBoss_Skill_DarkHole");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_Skill_DarkHole::Clone(void* pArg) 
{
	CHellBoss_Skill_DarkHole* pInstance = new CHellBoss_Skill_DarkHole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CHellBoss_Skill_DarkHole");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_Skill_DarkHole::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
 
}
