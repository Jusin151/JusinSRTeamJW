#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_MP_Bar final : public CUI_Base,public CObserver
{
private:
	CUI_MP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_MP_Bar(const CUI_MP_Bar& Prototype);
	virtual ~CUI_MP_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	void Update_Mp_Bar();
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_MP_INFO{};

private:
	HRESULT Ready_Components();

public:
	void Set_MP(_int type)
	{
			m_iMp -= type;
			if (m_iMp < 0 || m_iMp > 100)
				m_iMp = 0; 

			Update_Mp_Bar();
	}
public:
	static CUI_MP_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

private:
	_uint m_iMp{};  

	virtual void OnNotify(void* pArg, const wstring& type) override
	{
		{
			if (type == L"MP")
				Set_MP(*reinterpret_cast<_uint*>(pArg));
		}
	}
};

END
