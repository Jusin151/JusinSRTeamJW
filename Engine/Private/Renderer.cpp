#include "Renderer.h"
#include "GameObject.h"
#include "Light.h"
#include "Collider.h"

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

//HRESULT CRenderer::Render_Collider()
//{
//	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//
//	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
//
//	//색깔 덧셈
//	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
//	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
//
//	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
//	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); // 와이어가 Z값을 덮어쓰지 않도록
//	m_pGraphic_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); // 동일한 깊이에서도 그려지도록
//
//
//	// 3. Z-파이팅 방지: 깊이 바이어스(Depth Bias) 적용 (중요!)
//	//    와이어프레임이 텍스처 표면과 거의 같은 깊이에 있어 깜빡이는 현상(Z-fighting)을 방지
//	//    값을 약간 조정하여 와이어프레임이 미세하게 더 가깝게 그려지도록 함
//	float fDepthBias = -0.00001f; // 값은 실험을 통해 조정 필요
//	float fSlopeScaleBias = -1.0f; // 표면 기울기에 따른 바이어스 조정
//	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
//	m_pGraphic_Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeScaleBias);
//
//	for (auto& pGameObject : m_RenderObjects[RG_COLLIDER])
//	{
//		if (nullptr != pGameObject && pGameObject->IsActive())
//		{
//			pGameObject->Render();	
//		}
//		Safe_Release(pGameObject);
//	}
//	m_RenderObjects[RG_COLLIDER].clear();
//	fDepthBias = 0.0f;
//	fSlopeScaleBias = 0.0f;
//	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
//	m_pGraphic_Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeScaleBias);
//	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // 와이어가 Z값을 덮어쓰지 않도록
//	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//
//	
//
//	return S_OK;
//}

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
