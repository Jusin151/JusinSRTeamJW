#include "Staff.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"

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

    // 초기화 시 설정
    m_TextureRanges["Idle"] = { 0,0 };
    m_TextureRanges["Charging"] = { 1, 16 };
    m_TextureRanges["Loop"] = { 17, 26 };
    m_TextureRanges["Attack"] = { 27, 31 };

    m_Weapon_INFO.WeaponID = WEAPON_ID::Staff;
    //m_Weapon_INFO.vPos = { }; // 스태프 위치
   // m_Weapon_INFO.vSize = { };// 스태프 크기 위에 두개는 일단 밖에서 하는중
    m_Weapon_INFO.Damage = 1;                // 데미지
    m_Weapon_INFO.AttackSpeed = 1.2f;           // 공격 속도 (ex. 초당 발사 가능 횟수)


    Ranged_INFO.CurrentAmmo = 50; //현총알
    Ranged_INFO.MaxAmmo = 50;    //샷건 최대 50발
    m_fAnimationSpeed = 0.05f; // 애니메이션속도


    CItem_Manager::GetInstance()->Add_Weapon(L"Staff", this);

    if (FAILED(Ready_Icon()))
        return E_FAIL;

	return S_OK;
}
HRESULT CStaff::Ready_Icon()
{

    CImage::Image_DESC Image_INFO = {};
    Image_INFO.vPos = { 0.f,150.f };
    Image_INFO.vSize = { 80.f,40.f };
    Image_INFO.IMAGE_TYPE = CImage::IMAGE_TYPE::WEAPON_ICON;
    Image_INFO.TextureKey = L"Prototype_Component_Texture_Weapon_Icon";
    Image_INFO.WeaponTag = L"Staff";
    Image_INFO.TextureImageNum = Staff;
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Image"),
        LEVEL_GAMEPLAY, TEXT("Layer_Image"), &Image_INFO)))
        return E_FAIL;

    return S_OK;
}


void CStaff::Priority_Update(_float fTimeDelta)
{
}

void CStaff::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_fElapsedTime += fTimeDelta;

    switch (m_eState)
    {
    case State::Idle:
        m_iCurrentFrame = m_TextureRanges["Idle"].first;
        if (m_bAttackInput)
        {
            m_eState = State::Charging;
            m_iCurrentFrame = m_TextureRanges["Charging"].first;
            m_fElapsedTime = 0.f;
            m_bHasFired = false;
        }
        break;

    case State::Charging:
        if (!m_bAttackInput)
        {
         
            m_eState = State::Firing;
            m_iCurrentFrame = m_TextureRanges["Attack"].first;
            m_fElapsedTime = 0.f;
            break;
        }

        if (m_fElapsedTime >= m_fAnimationSpeed)
        {
            m_fElapsedTime = 0.f;
            if (m_iCurrentFrame < m_TextureRanges["Charging"].second)
            {
                m_iCurrentFrame++;
            }
            else
            {
                m_eState = State::Charged;
                m_iCurrentFrame = m_TextureRanges["Loop"].first;
            }
        }
        break;


    case State::Charged:
        if (!m_bAttackInput)  // 버튼 뗐을 때 발사
        {
            m_eState = State::Firing;
            m_iCurrentFrame = m_TextureRanges["Attack"].first;
            m_fElapsedTime = 0.f;
            break;
        }

        if (m_fElapsedTime >= 0.02f)
        {
            m_fElapsedTime = 0.f;
            m_iCurrentFrame++;
            if (m_iCurrentFrame > m_TextureRanges["Loop"].second)
                m_iCurrentFrame = m_TextureRanges["Loop"].first;
        }
        break;

    case State::Firing:
        if (m_fElapsedTime >= m_fAnimationSpeed)
        {
            m_fElapsedTime = 0.f;
            if (m_iCurrentFrame < m_TextureRanges["Attack"].second)
            {
                m_iCurrentFrame++;
                if (!m_bHasFired)
                {
                    if (FAILED(m_pGameInstance->Add_GameObject(
                        LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Staff_Bullet"),
                        LEVEL_GAMEPLAY, TEXT("Layer_Staff_Bullet"))))
                        return;
                    Notify_Bullet();
                    Notify_MP();
                    Ranged_INFO.CurrentAmmo--;

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
void CStaff::Attack(_float fTimeDelta)
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        m_bAttackInput = true;

    __super::Attack(fTimeDelta); // 필요 시 호출 유지
}

void CStaff::Attack_WeaponSpecific(_float fTimeDelta)
{
}

HRESULT CStaff::Ready_Texture()
{
    return E_NOTIMPL;
}

void CStaff::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);


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
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Staff"), 
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
 		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CStaff* CStaff::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStaff* pInstance = new CStaff(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("스태프 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CStaff::Clone(void* pArg)
{
	CStaff* pInstace = new CStaff(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("스태프 UI 복제 실패");

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
}

