#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CUI_Base abstract : public CGameObject
{

public:
    typedef struct tagUI_Parent
    {
        _float4x4 ProjMatrix;
        _float4x4 WorldMatrix;
        _float4x4 ViewMatrix;
        _float2 vPos;
        _float2 vSize;
        _float fAlpha;
    }UI_Parent_Desc;

    typedef struct tagUI_Child
    {
        _float2 vPos;
        _float2 vSize;
        _float fAlpha;
        _bool bFlag={false};
    }UI_Child_Desc;

    typedef struct tagBackGround
    {
        UI_Parent_Desc BackGround_Desc{};
        wstring strTextureTag{};

        _float fmoveSpeed = {}; // 초당 100 단위 이동
        _float fMoveDistance = {}; // 이미지의 너비
        _float fStack_MoveDistance = {}; // 누적 이동 
		_float fNextx = {};
		_float fNexty = {};

    }BackGround_DESC;

    typedef struct tagDefault_UI
    {
        _float2 vPos;
        _float2 vSize;
    }UI_Desc;

protected:
    CUI_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CUI_Base(const CUI_Base& Prototype);
    virtual ~CUI_Base() = default;

public:
    virtual HRESULT Initialize_Prototype()PURE;
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta)PURE;
    virtual void Update(_float fTimeDelta)PURE;
    virtual void Late_Update(_float fTimeDelta)PURE;
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components(); 
    void Free();


protected:

    void Set_Parent(CUI_Base* pParent)
    {
        Parent = pParent;
        m_vRelativePosition = 
        { m_vPos.x - pParent->Get_Position().x,
          m_vPos.y - pParent->Get_Position().y
        };
        pParent->Childs.insert({ GetLayerID(), this });
    }
    void Set_Position(_float2 vPos) 
    {
        m_vPos = vPos;
        if (m_pTransformCom)
        {
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(vPos.x, vPos.y, 0.f));
        }
       // Update_ChildPositions();  // 부모 위치 변경하면 자식 위치도 바꾸게 할 예정(근데 하기 싫엉)
    }
    void Set_Size(_float2 vSize)
    {
        m_vSize = vSize;
        if (m_pTransformCom)
        {
            m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);
        }
    }
 
protected:
    CTexture* m_pTextureCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;
    CTransform* m_pTransformCom = nullptr;


public:   
    void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
    _float2 Get_Position() const { return m_vPos; }
    _float2 Get_Size() const { return m_vSize; }
    _float Get_Alpha() const { return m_fAlpha; }
    _bool  Get_MouseClick() const { return m_bIsMouseClick; }
    void   Set_MouseClick(_bool type) { m_bIsMouseClick = type; }
    virtual wstring GetLayerID() { return L"Default"; }

public:
    _float2  m_vRelativePosition = {};
    CUI_Base* Parent = nullptr;
    unordered_map<wstring, CUI_Base*> Childs;
    UI_Desc m_Shop_INFO{};
protected:
        UI_Parent_Desc m_UIBase_INFO{};
        _float2 m_vPos = {};  // UI 화면 좌표
        _float2 m_vSize = {};  // UI 크기
        _float  m_fAlpha = {};        // 투명도?
        _uint iCurrent_Image{};
        
        bool m_bIsMouseOver = {};
        bool m_bIsMouseClick = {};
protected:
        // 상태별 시작 인덱스 관리  텍스쳐컴객체
     map<string, pair<int, int>> m_TextureRanges{};
  
};

END
