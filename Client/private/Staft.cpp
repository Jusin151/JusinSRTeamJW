#include "Staft.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "Inventory.h"


CStaft::CStaft(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CStaft::CStaft(const CStaft& Prototype)
	: CRanged_Weapon(Prototype),
	m_Staft_INFO{Prototype.m_Staft_INFO }
{
}

HRESULT CStaft::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CStaft::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
		m_Staft_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_Staft_INFO.vSize.x, m_Staft_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Staft_INFO.vPos.x, m_Staft_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Staft_INFO.vPos;

   // CInventory::GetInstance()->Add_Weapon(L"Staft", this);
    
	return S_OK;
}

void CStaft::Priority_Update(_float fTimeDelta)
{
}

void CStaft::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		t += speed;  
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		t += speed;
	}

	float v = 20.0f;  // 폭을 설정 하는변수
	_float3 vNewPos;
	vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
	vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

	Attack(fTimeDelta);

	return;
}
void CStaft::Attack(_float fTimeDelta)
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        if (m_eState == State::Idle || m_eState == State::Firing)
        {
            m_eState = State::Charging;
            m_iCurrentFrame = 0;
            m_fElapsedTime = 0.0f;
        }
    }
    else
    {
        if (m_eState == State::Charging || m_eState == State::Charged)
        {
            m_eState = State::Firing;
            m_iCurrentFrame = 0;
            m_fElapsedTime = 0.0f;
        }
    }

    m_fElapsedTime += fTimeDelta;

    switch (m_eState)
    {
    case State::Charging:
        if (m_fElapsedTime >= 0.02f)
        {
            m_fElapsedTime = 0.0f;
            if (m_iCurrentFrame < 16)
            {
                m_iCurrentFrame++;
            }
            else
            {
                m_eState = State::Charged;
                m_iCurrentFrame = 0;
            }
        }
        break;

    case State::Charged:
        if (m_fElapsedTime >= 0.02f)
        {
            m_fElapsedTime = 0.0f;
            m_iCurrentFrame = (m_iCurrentFrame + 1) % 10;
        }
        break;

    case State::Firing:
        if (m_fElapsedTime >= 0.02f)
        {
            m_fElapsedTime = 0.0f;
            if (m_iCurrentFrame < 5)
            {
                m_iCurrentFrame++;
            }
            else
            {
                m_eState = State::Idle;
                m_iCurrentFrame = 0;
            }
        }
        break;

    case State::Idle:
        m_iCurrentFrame = 0;
        break;
    }
}

void CStaft::Late_Update(_float fTimeDelta)
{
	
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CStaft::Render()
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
        if (FAILED(m_pTextureCom_Second->Bind_Resource(m_iCurrentFrame)))
            return E_FAIL;
        break;

    case State::Firing:
        if (FAILED(m_pTextureCom_Third->Bind_Resource(m_iCurrentFrame)))
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

HRESULT CStaft::On_Collision()
{
		

	return S_OK;
}

HRESULT CStaft::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Staft_1"), // 차징 단계 총 16장
		TEXT("Com_Texture_Staft_1"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
 		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Staft_2"), // 차징이 다 되었는데도
		TEXT("Com_Texture_Staft_2"), reinterpret_cast<CComponent**>(&m_pTextureCom_Second))))    // 계속 누르고 있으면 
		return E_FAIL;                                                                       // 10장 무한루프

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Staft_3"), // 발사 텍스쳐 총 5장
		TEXT("Com_Texture_Staft_3"), reinterpret_cast<CComponent**>(&m_pTextureCom_Third))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Staft"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Staft"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CStaft* CStaft::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStaft* pInstance = new CStaft(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("스태프 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CStaft::Clone(void* pArg)
{
	CStaft* pInstace = new CStaft(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("스태프 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CStaft::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
