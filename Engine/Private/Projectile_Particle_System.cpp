#include "Projectile_Particle_System.h"

CProjectile_Particle_System::CProjectile_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{

}

CProjectile_Particle_System::CProjectile_Particle_System(const CProjectile_Particle_System& Prototype)
	: CParticle_System{ Prototype }
	, m_vDir{ Prototype.m_vDir }
	, m_fDistance{ Prototype.m_fDistance }
{

}

HRESULT CProjectile_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Particle_System::Initialize(void* pArg)
{
	PROJECTILEDESC desc = *reinterpret_cast<PROJECTILEDESC*>(pArg);
	m_vPos = { 0.f, 0.f, 0.f };
	m_fSize = desc.fSize;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 1;
	m_iMaxParticles = desc.iNumParticles;
	m_fDistance = desc.fDistance;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

void CProjectile_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
	_float3 vPos = *reinterpret_cast<_float3*>(pArg);
	pAttribute->bIsAlive = true;
	pAttribute->fAge = 0;
	_float3 randPos; GetRandomVector(&randPos, &m_vPos, 0.1f);
	pAttribute->vPosition = vPos + randPos;
	pAttribute->vVelocity = -m_vDir;
	pAttribute->fLifetime = 1.f;

	pAttribute->fSize = m_fSize;
	pAttribute->vCurrentColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	pAttribute->vColorFade = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.0f);
	pAttribute->iIndex = 0;
}

void CProjectile_Particle_System::Update(float fTimeDelta)
{
	const _uint numTextures = m_pTexture->Get_NumTextures();
	const float maxAnimDistance = 15.0f; // 예시 값
	//Reset_Particle(&*m_Particles.begin());
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			//i.vPosition -= i.vVelocity * fTimeDelta;
			
			// --- 1. 시간 비율 계산 (0.0 ~ 1.0) ---
			float ageRatio = 0.0f;
			if (i.fLifetime > 0.0f) {
				ageRatio = i.fAge / i.fLifetime;
				ageRatio = std::max(0.0f, std::min(ageRatio, 1.0f));
			}
			i.fAge += fTimeDelta;

			//// --- 2. 거리 비율 계산 (0.0 ~ 1.0) ---
			/*_float3 pos = i.vPosition;
			float currentDistance = pos.Length();
			float distanceRatio = currentDistance / maxAnimDistance;
			distanceRatio = max(0.0f, std::min(distanceRatio, 1.0f));*/

			//// --- 3. 두 비율 조합 (최댓값 사용) ---
			//// 시간과 거리 중 더 많이 진행된 쪽의 비율을 최종 비율로 사용
			//float finalRatio = max(ageRatio, distanceRatio);
			float finalRatio = ageRatio;

			///* --- 다른 조합 방식 예시 (주석 처리됨) ---
			//// 가중 평균 (예: 시간 60%, 거리 40%)
			//// float timeWeight = 0.6f;
			//// float distanceWeight = 0.4f;
			//// float finalRatio = timeWeight * ageRatio + distanceWeight * distanceRatio;
			//// finalRatio = std::max(0.0f, std::min(finalRatio, 1.0f)); // 결과 클램핑

			//// 곱셈
			//// float finalRatio = ageRatio * distanceRatio;
			//*/

			//// --- 4. 최종 비율을 프레임 인덱스로 매핑 ---
			i.fSize *= 0.90f;
			float targetFrame = finalRatio * static_cast<float>(numTextures);
			i.iIndex = static_cast<int>(targetFrame);
			//i.iIndex = min(i.iIndex, numTextures - 1); // 인덱스 범위 제한
		}
		if (i.iIndex >= m_pTexture->Get_NumTextures() || i.fAge >= i.fLifetime)
			i.bIsAlive = false;
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CProjectile_Particle_System::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));

	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

	// 깊이 버퍼 테스트 비활성화 (항상 그리도록)
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	// (선택 사항) 깊이 버퍼 쓰기 비활성화
	// 이 객체가 다른 객체의 렌더링에 영향을 주지 않도록 합니다.
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); // 정점 색상만 사용
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    // 텍스쳐 색상 지정

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CProjectile_Particle_System::Render()
{
	Pre_Render();

	PARTICLE* v = 0;

	_float4x4 matrixIdentity;
	D3DXMatrixIdentity(&matrixIdentity);
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &matrixIdentity);
	__super::Render();
	Post_Render();
	return S_OK;
}

HRESULT CProjectile_Particle_System::Post_Render()
{
	// 깊이 버퍼 테스트 비활성화 (항상 그리도록)
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// (선택 사항) 깊이 버퍼 쓰기 비활성화
	// 이 객체가 다른 객체의 렌더링에 영향을 주지 않도록 합니다.
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CProjectile_Particle_System::Add_Particle(void* pArg)
{
	for (_uint i = 0; i < m_iMaxParticles; ++i)
	{
		ATTRIBUTE attribute = {};
		Reset_Particle(&attribute, pArg);
		m_Particles.push_back(attribute);
	}
}

CProjectile_Particle_System* CProjectile_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CProjectile_Particle_System* CProjectile_Particle_System::Clone(void* pArg)
{
	CProjectile_Particle_System* pInstance = new CProjectile_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CProjectile_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Particle_System::Free()
{
	__super::Free();
}
