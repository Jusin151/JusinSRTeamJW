#include "Camera_CutScene.h"
#include "Transform.h"
#include "GameInstance.h"
#include "HellBoss.h"
#include "UI_Manager.h"
#include <Camera_FirstPerson.h>
#include "Level_Hong.h"

CCamera_CutScene::CCamera_CutScene(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CCamera(pGraphic_Device) 
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& Prototype)
    : CCamera(Prototype) 
{
}

HRESULT CCamera_CutScene::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_CutScene::Initialize(void* pArg)
{

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (pArg != nullptr)
    {
        _float3* pEyeAt = (_float3*)pArg;
        m_vEye = pEyeAt[0];
        m_vAt = pEyeAt[1];
    }
    tagCameraDesc	Desc{};

    Desc.vEye = _float3(0.f, 100.f, -10.f);
    Desc.vAt = _float3(0.f, 0.f, 0.f);
    Desc.vAt = _float3(0.f, 10.f, 0.f);
    Desc.fFov = D3DXToRadian(45.f);
    Desc.fNear = 0.1f;
    Desc.fFar = 300.f;
    Desc.fFar = 200.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    return S_OK;
}
void CCamera_CutScene::Set_CutSceneMove(const _float3& vStart, const _float3& vEnd, _float fSpeed)
{
    m_vStart = vStart;
    m_vEnd = vEnd;
    m_vCurEye = vStart;
    m_fMoveSpeed = fSpeed;
    m_fLerpT = 0.f;
    m_bMoving = true;
    m_bUsePath = false; 
}


HRESULT CCamera_CutScene::Ready_Components()
{
    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.fSpeedPerSec = 30.f;
    TransformDesc.fRotationPerSec = D3DXToRadian(180.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;
    m_vScale = { 1.f, 1.f, 1.f };
    return S_OK;
}

void CCamera_CutScene::Shaking(_float fTimeDelta)
{
    m_fShakeTime += fTimeDelta;
    // 지속 시간 초과 시 흔들림 종료 및 초기화
    if (m_fShakeTime >= m_fShakeDuration)
    {
        m_fShakeDuration = 0.f;
        m_fShakeAmount = 0.f;
        m_fShakeTime = 0.f;
        m_bTriggerShake = false;
        _float3 basePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        basePos.y += 0.2f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, basePos);
        return;
    }

    // 랜덤 오프셋 생성 (범위: [-1, 1] * 흔들림 강도)
    float randomX = ((float)rand() / (float)RAND_MAX) * 1.5f - 1.f;
    float randomY = ((float)rand() / (float)RAND_MAX) * 1.5f - 1.f;
    _float3 randomOffset = { randomX * m_fShakeAmount, randomY * m_fShakeAmount, 0.f };

    _float3 basePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    basePos.y += 0.2f; // 기존에 사용하던 높이 오프셋

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, basePos + randomOffset);
}

void CCamera_CutScene::Set_LookTarget(CGameObject* pTarget)
{ 
    m_pTarget = pTarget;
}

void CCamera_CutScene::Set_FixedTransform(const _float3& vEye, const _float3& vAt)
{
    m_vEye = vEye;
    m_vAt = vAt;
}
void CCamera_CutScene::Set_CameraDisableDelay(_float fDelay) 
{
    m_fDisableDelay = fDelay;
    m_fDisableTimer = 0.f;
    m_bDelayDisable = true;
    CLevel_Hong::m_bCutSceneDelayDisable = true;
    CUI_Manager::GetInstance()->Set_Actived_UI(false);
    if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"))))
    {
        if (pPlayer->Get_Current_Weapon() != nullptr)
        {
        pPlayer->Get_Current_Weapon()->SetActive(false); 
        }
        _bool bLocked = true;
        pPlayer->OnNotify(&bLocked, L"");
        CCamera_FirstPerson* pCam = static_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
        if (pCam)
        {
            pCam->OnNotify(&bLocked, L"");
        }
    }
    
}


void CCamera_CutScene::Set_CutScenePath(const vector<_float3>& vecPath, float fSpeed)
{
    m_vecPathPoints = vecPath;
    m_fMoveSpeed = fSpeed;
    m_iCurrentSegment = 0;
    m_fLerpT = 0.f;
    m_bMoving = true;
    m_bUsePath = true;
    m_vCurEye = vecPath.front();
    if (CHellBoss* pBoss = dynamic_cast<CHellBoss*>(m_pHellBoss = m_pGameInstance->Find_Object(LEVEL_HONG, TEXT("Layer_HellBoss"))))
    {
        pBoss->Set_CutSceneCamera_Lock(true);
    }
}

void CCamera_CutScene::TriggerShake(_float shakeAmount, _float duration)
{
    m_fShakeAmount = shakeAmount;
    m_fShakeDuration = duration;
    m_fShakeTime = 0.f; // 초기화하여 새 효과 적용
    m_bTriggerShake = true; // 트리거 활성화
    m_vOriginalCameraPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 원래 카메라 위치 저장
    m_vOriginalCameraPosition.y += 0.5f; // 카메라 위치 조정
}

void CCamera_CutScene::Update(_float fTimeDelta)
{

    if (m_bMoving)
    {
        if (m_bMoving && m_bUsePath && m_vecPathPoints.size() >= 2)
        {
            _float3 vFrom = m_vecPathPoints[m_iCurrentSegment];
            _float3 vTo = m_vecPathPoints[m_iCurrentSegment + 1];

            m_fLerpT += fTimeDelta * m_fMoveSpeed;
            m_vCurEye = VectorLerp(vFrom, vTo, m_fLerpT);

            if (m_fLerpT >= 1.f)
            {
                m_fLerpT = 0.f;
                ++m_iCurrentSegment;

                if (m_iCurrentSegment >= m_vecPathPoints.size() - 1)
                {
                    m_bMoving = false;
                    m_iCurrentSegment = 0;
                }
            }
        }
        else if (m_bMoving && !m_bUsePath)
        {
            m_fLerpT += fTimeDelta * m_fMoveSpeed;
            m_vCurEye = VectorLerp(m_vStart, m_vEnd, m_fLerpT);

            if (m_fLerpT >= 1.f)
            {
                m_fLerpT = 1.f;
                m_bMoving = false;
            }
        }

        else
        {
            // 단일 이동
            m_fLerpT += fTimeDelta * m_fMoveSpeed;
            if (m_fLerpT >= 1.f)
            {
                m_fLerpT = 1.f;
                m_bMoving = false;
            }

            m_vCurEye = VectorLerp(m_vStart, m_vEnd, m_fLerpT);
        }
    }



    if (m_pTarget)
    {
        m_vAt = static_cast<CHellBoss*>(m_pTarget)->Get_Pos();
    }


    _float3 vLook = (m_vAt - m_vCurEye);
    vLook.Normalize();
    _float3 vRight = _float3(0.f, 1.f, 0.f).Cross(vLook).GetNormalized();
    _float3 vUp = vLook.Cross(vRight).GetNormalized();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurEye);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);

    // 카메라 자동 비활성화 타이머
    if (m_bDelayDisable)
    {
        m_fDisableTimer += fTimeDelta;
        if (m_fDisableTimer >= m_fDisableDelay)
        {
            m_bDelayDisable = false; 
            CLevel_Hong::m_bCutSceneDelayDisable = false;
            SetActive(false); 
            CUI_Manager::GetInstance()->Set_Actived_UI(true);
            if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"))))
            {
                if (pPlayer->Get_Current_Weapon() != nullptr)
                    pPlayer->Get_Current_Weapon()->SetActive(false);
                _bool bLocked = false;
                pPlayer->OnNotify(&bLocked, L"");

                CCamera_FirstPerson* pCam = static_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
                if (pCam)
                {
                    pCam->OnNotify(&bLocked, L"");
                }
            }
        }
    }

    Shaking(fTimeDelta);

    __super::Update_VP_Matrices();
}
void CCamera_CutScene::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CCamera_CutScene::Render()
{
    return S_OK;
}
CCamera_CutScene* CCamera_CutScene::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCamera_CutScene* pInstance = new CCamera_CutScene(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
    CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCamera_CutScene");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_CutScene::Free()
{
    __super::Free();

}

