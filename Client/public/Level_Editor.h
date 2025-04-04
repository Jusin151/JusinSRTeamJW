#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CPickingSys;
END

BEGIN(Client)

class CLevel_Editor final : public CLevel
{
private:
	CLevel_Editor(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

private:
	CPickingSys* m_pPickingSys = { nullptr };
	class CMyImGui* m_pImgui = { nullptr };

public:
	static CLevel_Editor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END