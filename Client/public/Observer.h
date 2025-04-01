
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CObserver

{
public:
    virtual void OnNotify(void* pArg, const wstring& type)PURE;
    //virtual void OnNotify(const wstring& pArg, const wstring& type)PURE;
    virtual ~CObserver() = default;
};
END