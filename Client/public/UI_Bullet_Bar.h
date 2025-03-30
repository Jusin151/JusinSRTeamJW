#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Base.h"
#include "Observer.h"
#include <Ranged_Weapon.h>

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로
END

BEGIN(Client)


class CUI_Bullet_Bar final : public CUI_Base,public CObserver
{
private:
	CUI_Bullet_Bar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Bullet_Bar(const CUI_Bullet_Bar& Prototype);
	virtual ~CUI_Bullet_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	void Update_Bullet_Bar();
	virtual HRESULT Render()override;

private:
	UI_Child_Desc m_Bullet_Bar_INFO{};

private:
	HRESULT Ready_Components();


public:
	static CUI_Bullet_Bar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
public:
	void Set_Bullet(_int Bullet)
	{
     	m_iBullet -= Bullet;
		if (m_iBullet < 0 || m_iBullet > 100)
			m_iBullet = 0; // 최소 체력 제한

		Update_Bullet_Bar();	
	}
    void OnNotify(void* pArg, const wstring& type) override
    {
        if (type == L"BULLET")
        {
            Ranged_INFO = *reinterpret_cast<CRanged_Weapon::Ranged_DESC*>(pArg);

			m_iBullet= Ranged_INFO.CurrentAmmo;
			m_iMxBullet=Ranged_INFO.MaxAmmo;
            Update_Bullet_Bar();
        }
		else if (type == L"BULLET_NO")
		{
			m_iBullet = 0;
			m_iMxBullet = 0;
			Update_Bullet_Bar();
		}
    }
private:
	_int m_iBullet{};
	_int m_iMxBullet{};
	CRanged_Weapon::Ranged_DESC Ranged_INFO{};
};
END


