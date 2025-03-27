#include "Image.h"
#include "GameInstance.h"

CImage::CImage(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{pGraphic_Device}
{
}

CImage::CImage(const CImage& Prototype)
	:CGameObject( Prototype ),
	m_pTextureCom(Prototype.m_pTextureCom),
	m_pVIBufferCom(Prototype.m_pVIBufferCom),
	m_pTransformCom(Prototype.m_pTransformCom) 
{
}

HRESULT CImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImage::Initialize(void* pArg)
{
	
	if (pArg != nullptr)
	{
 		m_Image_INFO = *reinterpret_cast<Image_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_Image_INFO.vSize.x, m_Image_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Image_INFO.vPos.x, m_Image_INFO.vPos.y, 0.f));

	return S_OK;
}

void CImage::Priority_Update(_float fTimeDelta)
{

}

void CImage::Update(_float fTimeDelta)
{
	m_bIsActive=true;
}

void CImage::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CImage::Render()
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
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CImage::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY,m_Image_INFO.TextureKey,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

CImage* CImage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CImage* pInstance = new CImage(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("이미지 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CImage::Clone(void* pArg)
{
	CImage* pInstace = new CImage(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("이미지 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CImage::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
