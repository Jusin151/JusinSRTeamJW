#include "Magnum.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CMagnum::CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CMagnum::CMagnum(const CMagnum& Prototype)
	: CRanged_Weapon(Prototype),
	m_Claymore_INFO{ Prototype.m_Claymore_INFO }
{
}

HRESULT CMagnum::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CMagnum::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		m_Claymore_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pTransformCom->Set_Scale(m_Claymore_INFO.vSize.x, m_Claymore_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Claymore_INFO.vPos.x, m_Claymore_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Claymore_INFO.vPos;

	return S_OK;
}

void CMagnum::Priority_Update(_float fTimeDelta)
{
}

void CMagnum::Update(_float fTimeDelta)
{
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
	__super::Update(fTimeDelta);
}

void CMagnum::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CMagnum::Render()
{


	return __super::Render();
}

HRESULT CMagnum::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Claymore"),
		TEXT("Com_Texture_Claymore"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Claymore"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Claymore"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CMagnum* CMagnum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagnum* pInstance = new CMagnum(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("클레이모어 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CMagnum::Clone(void* pArg)
{
	CMagnum* pInstace = new CMagnum(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("클레이모어 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CMagnum::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
