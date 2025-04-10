#include "sky.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CSky::CSky(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CSky::CSky(const CSky& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{
}

void CSky::Update(_float fTimeDelta)
{
	_float4x4		ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *reinterpret_cast<_float3*>(&ViewMatrix.m[3][0]));
}

void CSky::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_PRIORITY, this);
}

HRESULT CSky::Render()
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

HRESULT CSky::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	return S_OK;
}

HRESULT CSky::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	

	return S_OK;
}

HRESULT CSky::Ready_Components()
{
	LEVEL nextLevel = static_cast<CLevel_Loading*>(m_pGameInstance->Get_Level())->Get_NextLevel();
	wstring textureKey = L"Prototype_Component_Texture_Cube_Base";
	switch (nextLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		textureKey = L"Prototype_Component_Texture_Skybox_Tundra";
		break;
	case Client::LEVEL_HUB:
		break;
	case Client::LEVEL_HONG:
		textureKey = L"Prototype_Component_Texture_Skybox_Hell";
		break;
	case Client::LEVEL_BOSS:
		textureKey = L"Prototype_Component_Texture_Skybox_Cthulhu";
		break;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, textureKey,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CSky* CSky::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky* pInstance = new CSky(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);



}
