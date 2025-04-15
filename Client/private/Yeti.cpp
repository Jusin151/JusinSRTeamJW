#include "Yeti.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CYeti::CYeti(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster_Base(pGraphic_Device)
{
}

CYeti::CYeti(const CYeti& Prototype)
    :CMonster_Base(Prototype)
{
}

HRESULT CYeti::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CYeti::Initialize(void* pArg)
{
    INIT_PARENT(pArg)
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eType = CG_MONSTER;

    m_iAp = 5;

    m_iHp = 30;

    m_iExp = 64;

    m_fSpeed = 0.4f;

    m_pColliderCom->Set_Scale(_float3(2.f, 2.f, 2.f));

    return S_OK;
}

void CYeti::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_eCurState == MS_DEATH && !m_bGib)
    {
        m_bGib = true;
        Create_Gibs(2);
        m_pSoundCom->Play_Event(L"event:/Monsters/Yeti/Yeti_Death", m_pTransformCom)->SetVolume(0.5f);
    }
        
    if (m_iCurrentFrame > 40)
    {
        m_bIsActive = false;
    }
    
}

void CYeti::Update(_float fTimeDelta)
{
   
    if (!m_bCheck)
    {
        m_vAnchorPoint = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());
        m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
        return;
    }
    if (m_pTarget == nullptr)
        return;
    Select_Pattern(fTimeDelta);


    __super::Update(fTimeDelta);


    if (m_eCurState != MS_DEATH)
    {

        m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

        m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
    }
    m_pSoundCom->Update(fTimeDelta);
}

void CYeti::Late_Update(_float fTimeDelta)
{

    if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
            return;
    }

    if (nullptr == m_pTarget)
        return;

    if (!m_bCheck)
        return;

  


    Calc_Position();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

    m_vObjectMtvSum = { 0.f, 0.f, 0.f };
    m_vWallMtvs.clear();

    Select_Frame(fTimeDelta);
}

HRESULT CYeti::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    SetUp_RenderState();
    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if(FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (g_bDebugCollider)
    {
        m_pColliderCom->Render();
    }
    m_pShaderCom->End();

    Release_RenderState();


    return S_OK;
}

void CYeti::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}

HRESULT CYeti::On_Collision(CCollisionObject* other)
{
    if (nullptr == m_pColliderCom)
        return E_FAIL;
    if (nullptr == other)
        return S_OK;

    // 안바뀌면 충돌 안일어남
    if (other->Get_Type() == CG_END)
        return S_OK;


    _float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float3 vMtv = m_pColliderCom->Get_MTV();
    _float3 vMove = { vMtv.x, 0.f, vMtv.z };

    switch (other->Get_Type())
    {
    case CG_PLAYER:

        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
        //m_pTransformCom->Go_Backward(fTimeDelta);
   
        if (m_eCurState != MS_ATTACK)
        {
         
           
        }
        else
        {
            m_iAp *= 3;
            Take_Damage(other);
            m_iAp /= 3;
        }
        break;
     
    case CG_WEAPON:
        Create_Stains(5);
        m_pSoundCom->Play_Event(L"event:/Monsters/Yeti/Yeti_Pain", m_pTransformCom)->SetVolume(0.5f);
        m_eCurState = MS_HIT;
        break;

    case CG_MONSTER:
        m_vObjectMtvSum += vMove * 0.5f;
      

        break;
    case CG_STRUCTURE_WALL:
       
        break;
    case CG_DOOR:

        break;
    case CG_OBJECT:
        m_vObjectMtvSum += vMove;
        break;
    default:
        break;
    }



    return S_OK;
}

void CYeti::Select_Pattern(_float fTimeDelta)
{
    if (m_eCurState == MS_DEATH)
        return;

    _float3 vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

  

    switch (m_eCurState)
    {
    case MS_IDLE:
        if (Check_DIstance(fTimeDelta))
        {
            if (vDist.LengthSq() > 10)
            {
                m_eCurState = MS_WALK;
               
            }
            else
            {
                m_eCurState = MS_ATTACK;
            }
        }
        else
        {
            m_eCurState = MS_BACK;
            m_fBackTime = 2.f; // 돌아가기 시작 시 2초 설정
        }
        break;
    case MS_BACK:
        
        m_fBackTime -= fTimeDelta;
        m_pTransformCom->Chase(m_vAnchorPoint, fTimeDelta * m_fSpeed * 2.f, 5.f);
        m_vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

        if (m_fBackTime <= 0)
        {
            m_eCurState = MS_IDLE; // 2초 후에 IDLE 상태로 복귀
        }
              
     
        break;
    case MS_WALK:
        m_pSoundCom->Play_Event(L"event:/Monsters/Yeti/Yeti_Detect", m_pTransformCom)->SetVolume(0.5f);
        Chasing(fTimeDelta, m_pColliderCom->Get_Scale().Length());
        break;
    case MS_HIT:
        // 맞고 바로 안바뀌도록
        if (m_fElapsedTime >= 0.5f)
            m_eCurState = MS_IDLE;
        else
            return;

        break;
    case MS_ATTACK:
        m_pSoundCom->Play_Event(L"event:/Monsters/Yeti/Yeti_Attack", m_pTransformCom)->SetVolume(0.5f);
        Attack_Melee(fTimeDelta);
        break;

    default:
        break;
    }

}

void CYeti::Attack_Melee(_float fTimeDelta)
{
    if (m_eCurState != MS_ATTACK)
    {
        if (m_fElapsedTime >= 1.f)
            m_eCurState = MS_ATTACK;
        else
            return;
    }
}

_bool CYeti::Check_DIstance(_float fTimeDelta)
{
    _float3 Dist = m_vAnchorPoint - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (Dist.LengthSq() < 100)
    {
        m_fBackTime = 2.f;
        return true;  // 설정한 값 이내면 바로 true
    }
    
    if (m_fBackTime > 0)
    {
        m_fBackTime -= fTimeDelta;
        return false; // 아직 돌아갈 시간이 남아있다면 false 반환
    }
 

    return true;
    
}

void CYeti::Select_Frame(_float fTimeDelta)
{
    m_fElapsedTime += fTimeDelta*1.7f; //임의로 애니메이션 속도 좀 올림



    switch (m_eCurState)
    {
    case MS_IDLE:
        m_iCurrentFrame = 0;
        break;
    case MS_HIT:
        m_iCurrentFrame = 1;
        break;
    case MS_BACK:
        m_iCurrentFrame = 2;
        break;
    case MS_WALK:

        if (m_iCurrentFrame == 10)
        {
            m_eCurState = MS_IDLE;

        }

        if (m_iCurrentFrame < 3 || m_iCurrentFrame > 10)
            m_iCurrentFrame = 3;

        if (m_fElapsedTime >= 0.2f)
        {
            m_fElapsedTime = 0.0f;

            m_iCurrentFrame++;

        }
        break;
    case MS_ATTACK:

        if (m_iCurrentFrame == 27)
        {
            m_eCurState = MS_IDLE;
        }

        if (m_iCurrentFrame < 11 || m_iCurrentFrame > 27)
            m_iCurrentFrame = 11;

        if (m_fElapsedTime >= 0.1f)
        {
            m_fElapsedTime = 0.0f;

            m_iCurrentFrame++;

        }
        break;
    case MS_DEATH:
        if (m_iCurrentFrame < 28)
            m_iCurrentFrame = 28;

        if (m_fElapsedTime >= 0.1f)
        {
            m_fElapsedTime = 0.0f;

            m_iCurrentFrame++;
        }
        break;

    default:
        break;
    }
}

HRESULT CYeti::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CYeti::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CYeti::Ready_Components()
{
    if (FAILED(__super::Add_Component(m_tObjDesc.iLevel, m_tObjDesc.stProtTextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CYeti* CYeti::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CYeti* pInstance = new CYeti(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CYeti");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CYeti::Clone(void* pArg)
{
    CYeti* pInstance = new CYeti(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CYeti");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CYeti::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTarget);
}
