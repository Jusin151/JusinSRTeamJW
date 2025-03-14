#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CUI_Base : public CGameObject
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

protected:
    CUI_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
    CUI_Base(const CUI_Base& Prototype);
    virtual ~CUI_Base() = default;

public:
    virtual HRESULT Initialize_Prototype() = 0;
    virtual HRESULT Initialize(void* pArg) = 0;
    virtual void Update(_float fTimeDelta);
    virtual HRESULT Render() = 0;
    virtual void Free();

protected:
    HRESULT Ready_Components();

protected:
    UI_Parent_Desc m_UIBase_INFO{};
    _float2 m_vPos = {};  // UI 화면 좌표
    _float2 m_vSize = {};  // UI 크기
    _float  m_fAlpha = {};        // 투명도?

    CTexture* m_pTextureCom = nullptr;
    CTransform* m_pTransformCom = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = nullptr;

    _float2  m_vRelativePosition = {};

public:
    CUI_Base* Parent = nullptr;
    unordered_map<wstring, CUI_Base*> Childs;


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
    _bool isPick(HWND hWnd)
    {
        POINT ptMouse{};
        GetCursorPos(&ptMouse);
        ScreenToClient(hWnd, &ptMouse);

      
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        LONG screenWidth = rcClient.right - rcClient.left;
        LONG screenHeight = rcClient.bottom - rcClient.top;

        // 내 코드는 화면 중앙의 0,0을 기준으로 하는게 더 편한거 같아서
        // 마우스커서의 좌표도 0,0으로 바꿔준거임
        _float2 vMousePos =
        {
            static_cast<_float>(ptMouse.x - screenWidth / 2),
            static_cast<_float>(screenHeight / 2 - ptMouse.y)
        };

        RECT rcUI =
        {
           _long(m_vPos.x - m_vSize.x * 0.5f),
           _long(m_vPos.y - m_vSize.y * 0.5f),
           _long(m_vPos.x + m_vSize.x * 0.5f),
           _long(m_vPos.y + m_vSize.y * 0.5f)
        };

        return PtInRect(&rcUI, POINT{ static_cast<LONG>(vMousePos.x), static_cast<LONG>(vMousePos.y) });
    }

   
    void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

    _float2 Get_Position() const { return m_vPos; }
    _float2 Get_Size() const { return m_vSize; }
    _float Get_Alpha() const { return m_fAlpha; }

    //명훈이형이 말한 Layer ID 가져오는 함수 (추후 ID로 접근??)
    virtual wstring GetLayerID() { return L"Default"; }
};

END
