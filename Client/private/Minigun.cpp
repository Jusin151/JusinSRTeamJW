#include "Minigun.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include <Camera_FirstPerson.h>

CMinigun::CMinigun(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CRanged_Weapon(pGraphic_Device)
{
}

CMinigun::CMinigun(const CMinigun& Prototype)
    : CRanged_Weapon(Prototype),
    m_Staff_INFO{ Prototype.m_Staff_INFO }
{
}

HRESULT CMinigun::Initialize_Prototype()
{


    //아무런 입력이 없을때 0번쨰 이미지 출력
    // m_TextureRanges["Idle"] = { 0,0 };

     //좌클릭이 감지가 되면 바로 발사
     //하지만 꾹 누르고 있는다면 
    // m_TextureRanges["Attack"] 의 1부터
    //  m_TextureRanges["Attack"] 의 16까지 이미지를 출력하고

    // 그래도 계속 누르고 있다면
    //  m_TextureRanges["Charging"] = { 17, 26 };
    // m_TextureRanges["Loop"] = { 27, 31 };

    return S_OK;
}

HRESULT CMinigun::Initialize(void* pArg)
{
    if (pArg != nullptr)
        m_Staff_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
    else
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

  
    m_pTransformCom->Set_Scale(m_Staff_INFO.vSize.x, m_Staff_INFO.vSize.y, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
     _float3(m_Staff_INFO.vPos.x, m_Staff_INFO.vPos.y, 0.f));

    m_Weapon_INFO.WeaponID = WEAPON_ID::Minigun;
    //m_Weapon_INFO.vPos = {};
    //m_Weapon_INFO.vSize = {};
    m_Weapon_INFO.Damage = 200;
    m_Weapon_INFO.AttackSpeed = 1.2f;

    m_iAp = 50;
    m_eType = CG_WEAPON;
   
    Ranged_INFO.CurrentAmmo = 170; //현총알
    Ranged_INFO.MaxAmmo = 170;    // 최대 50발
    m_fAnimationSpeed = 0.01f; //미니건에서는  발사애니메이션속도
    m_fSpinSpeed = 0.02f; // 얘는 루프 전용 애니메이션속도

    m_vInitialPos = m_Staff_INFO.vPos;

    // 미니건 초기화 시 설정
    m_TextureRanges["Idle"] = { 0,0 };
    m_TextureRanges["Loop"] = { 0, 3 };
    m_TextureRanges["Attack"] = { 4, 7 };

    CItem_Manager::GetInstance()->Add_Weapon(L"Minigun", this);


    __super::Ready_Picking();
    return S_OK;
}

HRESULT CMinigun::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Minigun"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
        return E_FAIL;

    CLight::LIGHT_INIT lDesc = { L"../../Resources/Lights/GunLight.json" };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Light_Point"),
        TEXT("Com_Light"), reinterpret_cast<CComponent**>(&m_pLightCom), &lDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
        TEXT("Com_Sound_Source"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
        TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
        return E_FAIL;

    return S_OK;
}

void CMinigun::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMinigun::Update(_float fTimeDelta)
{
    if (GetAsyncKeyState('R') & 0x8000)
    {
        Ranged_INFO.CurrentAmmo += 100;
        Notify_Bullet(); 
    }
    __super::Update(fTimeDelta);
    m_pSoundCom->Update(fTimeDelta);
}

void CMinigun::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);;

    m_fElapsedTime += fTimeDelta;

    if (Ranged_INFO.CurrentAmmo <= 0)
    {
        m_eState = State::Idle;
        m_iCurrentFrame = m_TextureRanges["Idle"].first;
    }


    switch (m_eState)
    {
    case State::Idle:
        if (m_bAttackInput)
        {
            m_eState = State::Loop;
            m_iCurrentFrame = m_TextureRanges["Loop"].first;
            m_fHoldTime = 0.f;
            m_fElapsedTime = 0.f;
        }
        else
        {
            m_iCurrentFrame = m_TextureRanges["Idle"].first;
        }
        break;

    case State::Loop:
        if (m_bAttackInput)
        {
            m_fHoldTime += fTimeDelta;
            if (m_fElapsedTime >= m_fSpinSpeed)
            {
                m_fElapsedTime = 0.f;
                m_iCurrentFrame++;
                if (m_iCurrentFrame > m_TextureRanges["Loop"].second)
                    m_iCurrentFrame = m_TextureRanges["Loop"].first;
            }
            if (m_fHoldTime >= 1.0f)
            {
                m_eState = State::Firing;
                m_iCurrentFrame = m_TextureRanges["Attack"].first;
                m_fElapsedTime = 0.f;
                m_bHasFired = false;
            }
        }
        else
        {
            m_eState = State::Idle;
            m_iCurrentFrame = m_TextureRanges["Idle"].first;
            m_fHoldTime = 0.f;
            m_fElapsedTime = 0.f;
        }
        break;

    case State::Firing:
        if (m_fElapsedTime >= m_fAnimationSpeed)
        {
            m_fElapsedTime = 0.f;

            if (!m_bHasFired && m_iCurrentFrame == m_TextureRanges["Attack"].first + 1)
            {
                if (Ranged_INFO.CurrentAmmo > 0)
                {
                    __super::Picking_Object(1, m_Weapon_INFO.Damage);
                    Ranged_INFO.CurrentAmmo--;
                    Notify_Bullet();
                    m_bHasFired = true;

                    CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
                    if (pCamera)
                    {
                        pCamera->ApplyRecoil(0.01f);
                    }
                }
            }

            m_iCurrentFrame++;

            if (m_iCurrentFrame > m_TextureRanges["Attack"].second)
            {
                m_iCurrentFrame = m_TextureRanges["Attack"].first;
                m_bHasFired = false;

                if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
                {
                    m_eState = State::Idle;
                    m_iCurrentFrame = m_TextureRanges["Idle"].first;
                }
            }
        }
        break;



    case State::SpinDown:
        m_eState = State::Idle;
        m_iCurrentFrame = m_TextureRanges["Idle"].first;
        m_fHoldTime = 0.f;
        m_pSoundCom->Play_Event(L"event:/Weapons/Range/Minigun/minigun_rotate_end")->SetVolume(0.5f);
        break;

    default:
        break;
    }

    if (State::Firing == m_eState)
    {
        CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
        if (nullptr == pPlayer)
            return;
        CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pTransform)
            return;
        m_pGameInstance->Add_Light(m_pLightCom);
        _float3 pos = pTransform->Get_State(CTransform::STATE_POSITION);
        pos += pTransform->Get_State(CTransform::STATE_LOOK) * 1.5f;
        m_pLightCom->Set_Position(pos);
        m_pLightCom->DecreaseIntensity(m_iCurrentFrame);
    }
    m_bAttackInput = false;
}

void CMinigun::Attack(_float fTimeDelta)
{
    if (Ranged_INFO.CurrentAmmo <= 0)
    {
        m_pSoundCom->Play_Event(L"event:/Weapons/Range/Gun/gun_empty")->SetVolume(0.5f);
        return;
    }

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        m_bAttackInput = true;
    __super::Attack(fTimeDelta);
}


void CMinigun::Attack_WeaponSpecific(_float fTimeDelta)
{

}

HRESULT CMinigun::Render()
{
    D3DXMATRIX matOldView, matOldProj;
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

    D3DXMATRIX matView;
    D3DXMatrixIdentity(&matView);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIX matProj;
    D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    _float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    vPos.z = 0.5f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    _float2 ScaleFactor = { 1.0f, 1.0f };
    _float2 Offset = { 0.f, 0.f };
    m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
    m_pShaderCom->Set_UVOffsetFactor(&Offset);

    if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Transform()))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Lights()))
        return E_FAIL;

    m_pShaderCom->Begin(2);
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pShaderCom->End();

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

    return S_OK;
}

HRESULT CMinigun::On_Collision()
{


    return S_OK;
}

CMinigun* CMinigun::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CMinigun* pInstance = new CMinigun(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {

        MSG_BOX("미니건 원본 생성 실패 ");

        Safe_Release(pInstance);
    }


    return pInstance;
}

CGameObject* CMinigun::Clone(void* pArg)
{
    CMinigun* pInstace = new CMinigun(*this);

    if (FAILED(pInstace->Initialize(pArg)))
    {

        MSG_BOX("미니건 복제 실패");

        Safe_Release(pInstace);
    }

    return pInstace;
}

void CMinigun::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pLightCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pMaterialCom);
}

