#include "Renderer.h"
#include "GameObject.h"
#include "Light.h"

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RG_END ||
		nullptr == pRenderObject)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Add_Light(CLight* pLight)
{
	if (nullptr == pLight)
		return E_FAIL;
	m_Lights.push_back(pLight);
	Safe_AddRef(pLight);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Enable_Lights()))
		return E_FAIL;
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Disable_Lights()))
		return E_FAIL;
	if (FAILED(Render_Collider()))
		return E_FAIL;
	if (FAILED(Render_UI_Background()))
		return E_FAIL;
	if (FAILED(Render_UI_Button()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;
	
	

	return S_OK;
}

HRESULT CRenderer::Enable_Lights()
{

	if (!m_Lights.empty())
	{

		int index = 0;
		for (auto& pLight : m_Lights)
		{
			pLight->Bind_Resouce(index);
			m_pGraphic_Device->LightEnable(index++, TRUE);
		}
		m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		m_pGraphic_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	}
	return S_OK;
}

HRESULT CRenderer::Disable_Lights()
{
	int index = 0;
	for (auto& pLight : m_Lights)
	{
		m_pGraphic_Device->LightEnable(index++, FALSE);
		Safe_Release(pLight);
	}
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_NORMALIZENORMALS, false);
	m_Lights.clear();
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Collider()
{
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_GREEN);
	for (auto& pGameObject : m_RenderObjects[RG_COLLIDER])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_COLLIDER].clear();
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	return S_OK;
}

HRESULT CRenderer::Render_UI_Background()
{
	for (auto& pGameObject : m_RenderObjects[RG_UI_BACKGROUND])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI_BACKGROUND].clear();

	return S_OK;
}
HRESULT CRenderer::Render_UI_Button()
{
	for (auto& pGameObject : m_RenderObjects[RG_UI_BUTTON])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI_BUTTON].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return new CRenderer(pGraphic_Device);
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
	{
		Safe_Release(pLight);
	}
	m_Lights.clear();

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);
		ObjectList.clear();
	}

	Safe_Release(m_pGraphic_Device);
}
