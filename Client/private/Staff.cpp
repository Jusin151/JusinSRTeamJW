#include "Staff.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include "Player.h"
CStaff::CStaff(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CStaff::CStaff(const CStaff& Prototype)
	: CRanged_Weapon(Prototype),
	m_Staff_INFO{Prototype.m_Staff_INFO }
{
}

HRESULT CStaff::Initialize_Prototype()
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

HRESULT CStaff::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
		m_Staff_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_Staff_INFO.vSize.x, m_Staff_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Staff_INFO.vPos.x, m_Staff_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Staff_INFO.vPos;


    m_TextureRanges["Idle"] = { 0,0 };
    m_TextureRanges["Charging"] = { 1, 16 };
    m_TextureRanges["Loop"] = { 17, 26 };
    m_TextureRanges["Attack"] = { 27, 31 };

    CItem_Manager::GetInstance()->Add_Weapon(L"Staff", this);

    //if (FAILED(Ready_Icon()))
    //    return E_FAIL;


 
	return S_OK;
}



void CStaff::Priority_Update(_float fTimeDelta)
{
	static _bool bInit = { false };

    if (!bInit)
    {
        if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_STATIC, TEXT("Prototype_GameObject_Staff_Bullet"), TEXT("Layer_Staff_Bullet"), 30)))
            return ;
    
		bInit = true;
    }

}

void CStaff::Update(_float fTimeDelta)
{
    if (GetAsyncKeyState('W') & 0x8000 ||
        GetAsyncKeyState('A') & 0x8000 ||
        GetAsyncKeyState('S') & 0x8000 ||
        GetAsyncKeyState('D') & 0x8000)
    {
        t += speed;
    }

    float v = 20.0f;
    _float3 vNewPos;
    vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
    vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);


}

void CStaff::Attack(_float fTimeDelta)
{
    if (m_bFireLock)
        return;

        m_bAttackInput = true;

    __super::Attack(fTimeDelta);
}

void CStaff::Attack_WeaponSpecific(_float fTimeDelta)
{

}

void CStaff::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    // 마나가 충전되었는지 확인하고 FireLock 해제
    if (m_bFireLock)
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
        if (pPlayer && pPlayer->TryUseMana(0)) // 마나가 1 이상이면
            m_bFireLock = false;
    }


    m_fElapsedTime += fTimeDelta;

    switch (m_eState)
    {
    case State::Idle:
        if (m_bAttackInput)
        {
            m_eState = State::Charging;
            m_iCurrentFrame = m_TextureRanges["Charging"].first;
            m_fElapsedTime = 0.0f;
            m_bHasFired = false;
        }
        else
        {
            m_iCurrentFrame = m_TextureRanges["Idle"].first;
        }
        break;

    case State::Charging:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            if (m_fElapsedTime >= 0.02f)
            {
                m_fElapsedTime = 0.0f;
                if (m_iCurrentFrame < m_TextureRanges["Charging"].second)
                    ++m_iCurrentFrame;
                else
                {
                    m_eState = State::Charged;
                    m_iCurrentFrame = m_TextureRanges["Loop"].first;
                }
            }
        }
        else
        {
            m_eState = State::Firing;
            m_iCurrentFrame = m_TextureRanges["Attack"].first;
            m_fElapsedTime = 0.0f;
        }
        break;

    case State::Charged:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            if (m_fElapsedTime >= 0.02f)
            {
                m_fElapsedTime = 0.0f;
                m_iCurrentFrame = (m_iCurrentFrame + 1) % (m_TextureRanges["Loop"].second - m_TextureRanges["Loop"].first + 1) + m_TextureRanges["Loop"].first;
            }
        }
        else
        {
            m_eState = State::Firing;
            m_iCurrentFrame = m_TextureRanges["Attack"].first;
            m_fElapsedTime = 0.0f;
        }
        break;

    case State::Firing:
        if (m_fElapsedTime >= 0.02f)
        {
            m_fElapsedTime = 0.0f;
            if (m_iCurrentFrame < m_TextureRanges["Attack"].second)
            {
                ++m_iCurrentFrame;

                if (!m_bHasFired)
                {
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
                    if (!pPlayer || !pPlayer->TryUseMana(10))  
                    {
                        m_bFireLock = true;
                        if (auto pUI_Event = dynamic_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(L"UI_Event")))
                        {
                            pUI_Event->ShowEventText(0, L"Mp_End");
                        }
                        return;
                    }

                    if (!m_pGameInstance->Add_GameObject_FromPool(LEVEL_STATIC, LEVEL_STATIC, TEXT("Layer_Staff_Bullet")))
                    {
                        MSG_BOX("스태프총알 생성안됨");
                    }

                    m_bHasFired = true;
                }


            }
            else
            {
                m_eState = State::Idle;
                m_iCurrentFrame = m_TextureRanges["Idle"].first;
            }
        }
        break;
    }

    m_bAttackInput = false;
}


HRESULT CStaff::Render()
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

    if (FAILED(m_pTransformCom->Bind_Resource()))
        return E_FAIL;

    switch (m_eState)
    {
    case State::Charging:
    case State::Idle:
        if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
            return E_FAIL;
        break;

    case State::Charged:
        if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
            return E_FAIL;
        break;

    case State::Firing:
        if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
            return E_FAIL;
        break;
    }

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

HRESULT CStaff::On_Collision()
{
		

	return S_OK;
}

HRESULT CStaff::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Staff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
 		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
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

CStaff* CStaff::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStaff* pInstance = new CStaff(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("스태프 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CStaff::Clone(void* pArg)
{
	CStaff* pInstace = new CStaff(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("스태프 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CStaff::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pMaterialCom);
}

