#include "Glacier.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "Projectile_Base.h"

CGlacier::CGlacier(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster_Base(pGraphic_Device)
{
}

CGlacier::CGlacier(const CGlacier& Prototype)
    :CMonster_Base(Prototype)
{
}

HRESULT CGlacier::Initialize_Prototype()

{
    return S_OK;
}

HRESULT CGlacier::Initialize(void* pArg)
{
    INIT_PARENT(pArg)
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eType = CG_MONSTER;

    m_iMaxHp = 60;

    m_iHp = m_iMaxHp;

    m_fSpeed = 0.25f;

    m_pColliderCom->Set_Scale(_float3(2.f, 2.f, 2.f));

    return S_OK;
}

void CGlacier::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

}

void CGlacier::Update(_float fTimeDelta)
{
    
    if (!m_bCheck)
    {
        m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
        return;
    }

    if (m_pTarget == nullptr)
        return;
    m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    Select_HpState();


    __super::Update(fTimeDelta);

    Select_Pattern(fTimeDelta);

    m_vDir = m_vNextPos - m_vCurPos;
    m_fLength = m_vDir.Length();
    m_vDir.Normalize();

    if (m_eCurState != MS_DEATH)
    {
        m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());

        
    }
    m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
}

void CGlacier::Late_Update(_float fTimeDelta)
{
    if (!m_bCheck)
        return;
    if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
            return;
    }

    if (nullptr == m_pTarget)
        return;
    if (0.f <= Check_Direction())
        m_bRight = true;
    else
        m_bRight = false;

    Calc_Position();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

    m_vObjectMtvSum = { 0.f, 0.f, 0.f };
    m_vWallMtvs.clear();


    _float3 vScale = m_pTransformCom->Compute_Scaled();
    _float3 extents = _float3(
        0.5f * vScale.x,
        0.5f * vScale.y,
        0.5f * vScale.z
    );
   
 
    Select_Frame(fTimeDelta);
}

HRESULT CGlacier::Render()
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
    if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
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

void CGlacier::Deserialize(const json& j)
{
    SET_TRANSFORM(j, m_pTransformCom);
}

HRESULT CGlacier::On_Collision(CCollisionObject* other)
{
    if (nullptr == m_pColliderCom)
        return E_FAIL;

    if (nullptr == other)
        return S_OK;

    // 안바뀌면 충돌 안일어남
    if (other->Get_Type() == CG_END)
        return S_OK;

    _float3 vMtv = m_pColliderCom->Get_MTV();
    _float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float3 vMove = { vMtv.x, 0.f, vMtv.z };
    switch (other->Get_Type())
    {
    case CG_PLAYER:

        Take_Damage(other);
        break;

    case CG_WEAPON:
        // 죽은 상태고, 프레임 끝나있으면 죽이기
        if(m_eCurState == MS_DEATH && m_iCurrentFrame >= 52)
            m_bIsActive = false;
       
        break;

    case CG_MONSTER:
        m_vObjectMtvSum += vMove * 0.5f;

        break;

    case CG_PLAYER_PROJECTILE_SPHERE:
        if (m_eCurState == MS_DEATH && m_iCurrentFrame >= 52)
            m_bIsActive = false;
        break;

    case CG_PLAYER_PROJECTILE_CUBE:
        if (m_eCurState == MS_DEATH && m_iCurrentFrame >= 52)
            m_bIsActive = false;
        break;
    case CG_STRUCTURE_WALL:

        break;
    case CG_OBJECT:
        m_vObjectMtvSum += vMove;
        break;
    case CG_DOOR:

        break;
    default:
        break;
    }


    return S_OK;
}

void CGlacier::Select_Pattern(_float fTimeDelta)
{
    if (m_eCurState == MS_DEATH)
        return;
    _float3 vDist;
    vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

    switch (m_eHpState)
    {
    case HP_MAX:
        m_eCurState = MS_WALK;
        Chasing(fTimeDelta, 5.f);
        break;
    case HP_HURT:
        if (vDist.LengthSq() > 25)
            Chasing(fTimeDelta,5.f);
        else
        {
            m_eCurState = MS_IDLE;
        }
        break;
    case HP_VERYHURT:

        Shooting(fTimeDelta);
        Chasing(fTimeDelta, 20.f);
        break;
    default:
        break;
    }
}

void CGlacier::Select_HpState()
{
    if (m_iHp == m_iMaxHp)
    {
        m_eHpState = HP_MAX;
    }
    else
    {
        if (m_iHp / float(m_iMaxHp) >= 0.5)
            m_eHpState = HP_HURT;
        else
            m_eHpState = HP_VERYHURT;
    }
}

_float  CGlacier::Check_Direction()
{
    CTransform* pTargetTrans = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _float3 lineThisToTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pTargetTrans->Get_State(CTransform::STATE_POSITION) ;

    lineThisToTarget.Normalize();

    return lineThisToTarget.Dot(pTargetTrans->Get_State(CTransform::STATE_RIGHT).GetNormalized());
}

void CGlacier::Shooting(_float fTimeDelta)
{
    if (m_eCurState != MS_ATTACK)
    {
        if (m_fElapsedTime >= 0.5f)
            m_eCurState = MS_ATTACK;
        else
            return;
    }

    if (m_iCurrentFrame == 39)
    {

        CProjectile_Base::PROJ_DESC pDesc = {};
        pDesc.fSpeed = 10.f;
        pDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized();
        pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

        // 오브젝트 풀링으로 변경 필요
        m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_GlacierBullet"), LEVEL_GAMEPLAY, TEXT("Layer_Monster_Projectile_GlacierBullet"), &pDesc);

        m_iCurrentFrame++;
    }
}

void CGlacier::Select_Frame(_float fTimeDelta)
{
    if (m_iCurrentFrame == 52)
        return;

    m_fElapsedTime += fTimeDelta;

    switch (m_eCurState)
    {
    case MS_IDLE:
        m_iCurrentFrame = 0;
        break;
    case MS_WALK:
        // 오른쪽
        if (m_bRight)
        {
            if (HP_MAX == m_eHpState)
            {

                if (m_iCurrentFrame < 9 || m_iCurrentFrame > 16)
                    m_iCurrentFrame = 9;

                if (m_fElapsedTime >= 0.1f)
                {
                    m_fElapsedTime = 0.0f;

                    m_iCurrentFrame++;

                }
                break;
            }
            else
            {
                if (m_iCurrentFrame == 25)
                {
                    m_eCurState = MS_IDLE;
                    m_iCurrentFrame = 0;
                    return;
                }

                if (m_iCurrentFrame < 25 || m_iCurrentFrame > 32)
                    m_iCurrentFrame = 25;

                if (m_fElapsedTime >= 0.1f)
                {
                    m_fElapsedTime = 0.0f;

                    m_iCurrentFrame++;

                }
                break;
            }
        }
        // 왼쪽
        else
        {

            if (HP_MAX == m_eHpState)
            {

                if (m_iCurrentFrame < 1 || m_iCurrentFrame > 8)
                    m_iCurrentFrame = 1;

                if (m_fElapsedTime >= 0.1f)
                {
                    m_fElapsedTime = 0.0f;

                    m_iCurrentFrame++;

                }
                break;
            }
            else
            {
                if (m_iCurrentFrame == 17)
                {
                    m_eCurState = MS_IDLE;
                    m_iCurrentFrame = 0;
                    return;
                }

                if (m_iCurrentFrame < 17 || m_iCurrentFrame > 24)
                    m_iCurrentFrame = 17;

                if (m_fElapsedTime >= 0.1f)
                {
                    m_fElapsedTime = 0.0f;

                    m_iCurrentFrame++;

                }
                break;
            }
        }
       
    case MS_ATTACK:

        if (m_iCurrentFrame == 43)
        {
            m_iCurrentFrame = 33;
            return;
        }

        if (m_iCurrentFrame < 33 || m_iCurrentFrame > 43)
            m_iCurrentFrame = 33;

        if (m_fElapsedTime >= 0.1f)
        {
            m_fElapsedTime = 0.0f;

            m_iCurrentFrame++;

        }
        break;
    case MS_DEATH:
        if (m_iCurrentFrame < 44)
            m_iCurrentFrame = 44;

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

HRESULT CGlacier::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0); // 기준값 설정 (0~255)
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CGlacier::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

HRESULT CGlacier::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(m_tObjDesc.iProtoLevel, m_tObjDesc.stProtTextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CGlacier* CGlacier::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CGlacier* pInstance = new CGlacier(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGlacier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGlacier::Clone(void* pArg)
{
    CGlacier* pInstance = new CGlacier(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGlacier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGlacier::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTarget);
}
