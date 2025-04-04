#include "Renderer.h"
#include "GameObject.h"
#include "Light.h"
#include "Collider.h"
#include "GameInstance.h"


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
	if (FAILED(Render_ViewModel()))
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
		{
			//CShader* shader = dynamic_cast<CShader*>(pGameObject->Get_Component(L"Com_Shader"));
			//if (shader)
			//{
			//	for (int i = 0; i < m_Lights.size(); ++i)
			//	{
			//		//shader.
			//	}
			//}
			pGameObject->Render();
		}
			

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (!m_RenderObjects[RG_BLEND].empty())
	{
		if (CGameInstance::Get_Instance()->Find_Object(3, L"Layer_Camera"))
		{
			CTransform* cameraTransform = dynamic_cast<CTransform*>(CGameInstance::Get_Instance()->Find_Object(3, L"Layer_Camera")->Get_Component(L"Com_Transform"));
			if (cameraTransform)
			{
				m_RenderObjects[RG_BLEND].sort([cameraTransform](CGameObject* a, CGameObject* b) -> bool {
					// 입력 유효성 검사 (선택 사항이지만 권장)
					if (!a || !b) return false; // 혹은 다른 규칙 적용 (예: nullptr을 뒤로 보내기)
					CTransform* transformA = static_cast<CTransform*>(a->Get_Component(L"Com_Transform"));
					CTransform* transformB = static_cast<CTransform*>(b->Get_Component(L"Com_Transform"));
					if (!transformA || !transformB) return false; // 위와 동일

					// 각 게임 오브젝트의 월드 위치 가져오기
					_float3 posA = transformA->Get_State(CTransform::STATE_POSITION);
					_float3 posB = transformB->Get_State(CTransform::STATE_POSITION);
					_float3 posC = cameraTransform->Get_State(CTransform::STATE_POSITION);

					// 카메라로부터 각 오브젝트까지의 거리 '제곱' 계산
					// 제곱 거리를 사용하는 이유: sqrt() 함수 호출보다 훨씬 빠르며, 크기 비교 목적으론 충분함
					// YourVectorType은 D3DXVECTOR3 또는 사용하는 벡터 라이브러리에 맞춰 변경

					_float3 CtoA = posC - posA;
					_float3 CtoB = posC - posB;

					_float distSqA = D3DXVec3LengthSq(&CtoA); // 아래 Helper 함수 참조
					_float distSqB = D3DXVec3LengthSq(&CtoB); // 아래 Helper 함수 참조

					// back-to-front 정렬: 거리 제곱이 큰 쪽(더 먼 쪽)이 먼저 오도록 함
					return distSqA > distSqB;
					// front-to-back 정렬 (가까운 순서)을 원하면: return distSqA < distSqB;
					});
			}
		}
	}
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

HRESULT CRenderer::Render_ViewModel()
{
	for (auto& pGameObject : m_RenderObjects[RG_VIEWMODEL])
	{
		if (nullptr != pGameObject && pGameObject->IsActive())
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_VIEWMODEL].clear(); 

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

HRESULT CRenderer::SetLights(CShader* pShader)
{
	_uint numLights = m_Lights.size();
	CLight::SHADER_LIGHT sl = {};

	for (_uint i = 0; i < numLights; ++i)
	{

	}
	return E_NOTIMPL;
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
