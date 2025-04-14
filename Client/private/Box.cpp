#include "Box.h"
#include "GameInstance.h"
#include "Item.h"
#include "Player.h"
#include "Ranged_Weapon.h"


CBox::CBox(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CDeco_Base(pGraphic_Device)
{
}

CBox::CBox(const CBox& Prototype)
    :CDeco_Base(Prototype)
{
}

HRESULT CBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBox::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
    m_pColliderCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));

   m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-20.f, 0.46f, -30.f));

    m_eType = CG_OBJECT;

    m_iCurrentFrame = 0;

    return S_OK;
}

HRESULT CBox::Ready_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Box"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

void CBox::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CBox::Update(_float fTimeDelta)
{
    if (!m_bInit)
    {
        m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_vNextPos = m_vCurPos;
        m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
        Calc_Position();
        m_bInit = true;
    }

    Look_Player();


    if (m_eCurState == DS_DEATH && !m_bChange)
    {
        m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
        m_bChange = true;
    }


    if (!(m_eCurState != DS_IDLE && m_eCurState == m_ePreState))
        m_pGameInstance->Add_Collider(CG_OBJECT, m_pColliderCom);
}

void CBox::Late_Update(_float fTimeDelta)
{
    Select_State();




    Calc_Position();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);




    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBox::Render()
{
    if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    SetUp_RenderState();
    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
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

HRESULT CBox::On_Collision(CCollisionObject* other)
{
    if (nullptr == m_pColliderCom)
        return E_FAIL;

    if (nullptr == other)
        return S_OK;

    // 안바뀌면 충돌 안일어남
    if (other->Get_Type() == CG_END)
        return S_OK;

    switch (other->Get_Type())
    {
    case CG_PLAYER:

        break;

    case CG_WEAPON:
        m_eCurState = DS_HIT;


        break;

    case CG_MONSTER:

        break;

    case CG_PLAYER_PROJECTILE_SPHERE:
        m_eCurState = DS_HIT;

        break;

    case CG_PLAYER_PROJECTILE_CUBE:
        m_eCurState = DS_HIT;

        break;

    default:
        break;
    }


    return S_OK;
}

HRESULT CBox::SetUp_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100); // 기준값 설정 (0~255)
    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);
    return S_OK;
}

HRESULT CBox::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    return S_OK;
}

void CBox::Select_State()
{
    if (m_eCurState != m_ePreState)
    {

        if (m_eCurState == DS_HIT)
        {

            m_pColliderCom->Set_Scale(_float3(1.5f, 0.75f, 1.5f));
            m_pTransformCom->Set_Scale(1.5f, 0.75f, 1.5f);
            m_iCurrentFrame = 1;
            m_eCurState = DS_DEATH;

            // 아이템 생성 로직 추가

            // 아이템 생성하고, 위치까지
            Smart_Drop();
        
        }
        else if (m_eCurState == DS_DEATH)
        {
            m_bChange = true;
            m_ePreState = DS_DEATH;
        }


    }
    else
    {
        if (m_eCurState == DS_IDLE)
            m_iCurrentFrame = 0;
    }
}

void CBox::Smart_Drop()
{
    // 정보를 일단 가져옴
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pTarget);
    pair<_int, _int> PlayerHp = pPlayer->Get_PlayerHp();
    pair<_int, _int> PlayerMp = pPlayer->Get_PlayerMp();
    CWeapon_Base* pWeapon = pPlayer->Get_Current_Weapon();

    // 체력 부터 체크

    CItem::ITEM_DESC tItemDesc;

    if (PlayerHp.first / float(PlayerHp.second) <= 0.5f)
    {
        if (PlayerHp.first / float(PlayerHp.second) <= 0.1f)
        {
            tItemDesc.eType = CItem::ITEM_TYPE::HP;
            tItemDesc.strItemName = L"HP_Big";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_HP_Big"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
        }
        else
        {
            tItemDesc.eType = CItem::ITEM_TYPE::HP;
            tItemDesc.strItemName = L"HP_Small";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_HP_Small"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
        }
    }
    else if (PlayerMp.first / float(PlayerMp.second) <= 0.5f)
    {
       
        tItemDesc.eType = CItem::ITEM_TYPE::MP;
        tItemDesc.strItemName = L"MP_Small";
        m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_MP_Small"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
        
    }
    else
    {
        CWeapon_Base::WEAPON_ID WeaponID = pWeapon->Get_Weapon_ID();

        tItemDesc.eType = CItem::ITEM_TYPE::AMMO;

        switch (WeaponID)
        {
        case Client::CWeapon_Base::Claymore:
            tItemDesc.eType = CItem::ITEM_TYPE::HP;
            tItemDesc.strItemName = L"HP_Small";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_HP_Small"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Axe:
            tItemDesc.eType = CItem::ITEM_TYPE::HP;
            tItemDesc.strItemName = L"HP_Small";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_HP_Small"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::ShotGun:
            tItemDesc.strItemName = L"ShotGun_Ammo_Big";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_ShotGun_Ammo_Big"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Magnum:
            tItemDesc.strItemName = L"Magnum_Ammo_Big";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_Magnum_Ammo_Big"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Staff:
            tItemDesc.strItemName = L"Staff_Ammo_Big";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_Staff_Ammo_Big"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Minigun:
            tItemDesc.strItemName = L"Minigun_Ammo";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_Minigun_Ammo"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Harvester:
            tItemDesc.strItemName = L"ShotGun_Ammo_Big";
            m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Item_ShotGun_Ammo_Big"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &tItemDesc);
            break;
        case Client::CWeapon_Base::Sonic:
            break;
        default:
            break;
        }
   
    }



    CGameObject* pObject = m_pGameInstance->Find_Last_Object(LEVEL_GAMEPLAY, TEXT("Layer_Item"));

    CTransform* pTrans = static_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));

    _float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    pTrans->Set_State(CTransform::STATE_POSITION, fPos);
}

CBox* CBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBox* pInstance = new CBox(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBox::Clone(void* pArg)
{
    CBox* pInstance = new CBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBox::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTarget);
}
