#pragma once
#include "Base.h"
BEGIN(Engine)
class CSound_Event final : public CBase
{
private:
	virtual ~CSound_Event() = default;

public:
    void Restart(); //이벤트 재시작
    void Stop(bool allowFadeOut = true); // 이벤트를 정지한다
public: // Setters
    void SetPaused(bool pause);
    void SetVolume(float value);
    void SetPitch(float value);
    void SetParameter(const std::string& name, float value);
    void Set3DAttributes(const class CTransform& worldTrans);
public: // Getters
    _bool       IsValid();
    _bool       Is3D() const;
    _bool       GetPaused() const;
    _float      GetVolume() const;
    _float      GetPitch()  const;
    _float      GetParameter(const std::string& name);
    
private:
    friend class CSound_Manager;
	CSound_Event(class CSound_Manager* pSound_Manager, _uint iID);
private:
	class CSound_Manager* m_pSound_Manager;
	_uint			m_iID;
public:
	void Free();
};

END
