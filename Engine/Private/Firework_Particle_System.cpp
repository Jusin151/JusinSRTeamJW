#include "Firework_Particle_System.h"

CFirework_Particle_System::CFirework_Particle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle_System{ pGraphic_Device }
{
	
}

CFirework_Particle_System::CFirework_Particle_System(const CParticle_System& Prototype)
	: CParticle_System { Prototype }
{

}

HRESULT CFirework_Particle_System::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirework_Particle_System::Initialize(void* pArg)
{
	FIREWORKDESC desc = *reinterpret_cast<FIREWORKDESC*>(pArg);
	m_Bounding_Box = desc.Bounding_Box;
	m_vPos = desc.vOrigin;
	m_fSize = desc.fSize;
	m_VBSize = 2048;
	m_VBOffset = 0;
	m_VBBatchSize = 512;
	m_iMaxParticles = desc.iNumParticles;
	m_fInitialUpSpeed = 2.0f;
	m_fHorizontalSpread = 1.5f;

	

	//PARTICLEDESC pDesc = { m_VBSize, desc.strShaderPath, desc.strTexturePath, };

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	for (_uint i = 0; i < desc.iNumParticles; ++i)
	{
		Add_Particle();
	}
	return S_OK;
}

void CFirework_Particle_System::Reset_Particle(ATTRIBUTE* pAttribute, void* pArg)
{
    pAttribute->bIsAlive = true;
    pAttribute->vPosition = m_vPos; // 시작 위치는 동일

    // --- 속도 설정 변경 ---
    // X와 Z 방향 속도는 약간의 무작위성 부여
    float fVelX = GetRandomFloat(-m_fHorizontalSpread, m_fHorizontalSpread);
    float fVelZ = GetRandomFloat(-m_fHorizontalSpread, m_fHorizontalSpread);

    // Y 방향 속도는 위쪽을 향하도록 설정 (약간의 무작위성 추가 가능)
    // float fVelY = m_fInitialUpSpeed; // 고정된 상승 속도
    float fVelY = GetRandomFloat(m_fInitialUpSpeed * 0.8f, m_fInitialUpSpeed * 1.2f); // 약간의 속도 변화 추가

    pAttribute->vVelocity = D3DXVECTOR3(fVelX, fVelY, fVelZ);

    // D3DXVec3Normalize(&pAttribute->vVelocity, &pAttribute->vVelocity); // <-- 이 줄을 제거하거나 주석 처리합니다.

    // --- 나머지 설정은 기존과 동일 ---
	pAttribute->vInitialColor = 0xFFFFE880;
	pAttribute->vCurrentColor = pAttribute->vInitialColor;
	pAttribute->vColorFade = 0x00F29538;

    pAttribute->fAge = 0.0f;
    pAttribute->fLifetime = 2.0f; // 필요하다면 조절
    pAttribute->fSize = m_fSize;
    pAttribute->iIndex = 0; //rand() % m_pTexture->Get_NumTextures();
}

void CFirework_Particle_System::Update(float fTimeDelta)
{
	for (auto& i : m_Particles)
	{
		if (i.bIsAlive)
		{
			// --- 중력 적용(위치 업데이트 전에 속도 변경) -- -
				// Y축 속도를 중력만큼 감소시킵니다.
				i.vVelocity.y -= GRAVITY * fTimeDelta;

			// --- 위치 업데이트 (변경된 속도 적용) ---
			i.vPosition += i.vVelocity * fTimeDelta;
			i.fSize *= 0.90f;
			
			// --- 색상 페이드 아웃 및 수명 관리 (기존과 동일) ---
			float ratio = i.fAge / i.fLifetime;
			i.vCurrentColor = ColorLerp(i.vInitialColor, i.vColorFade, ratio);
			i.fAge += fTimeDelta;

			if (i.fAge > i.fLifetime)
				i.bIsAlive = false;
		}
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CFirework_Particle_System::Pre_Render()
{
	__super::Pre_Render();

	return S_OK;
}

HRESULT CFirework_Particle_System::Post_Render()
{
	__super::Post_Render();

	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	return S_OK;
}

CFirework_Particle_System* CFirework_Particle_System::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFirework_Particle_System* CFirework_Particle_System::Clone(void* pArg)
{
	CFirework_Particle_System* pInstance = new CFirework_Particle_System(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CFirework_Particle_System");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFirework_Particle_System::Free()
{
	__super::Free();
}
