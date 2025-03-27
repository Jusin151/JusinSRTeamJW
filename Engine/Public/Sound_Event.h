#pragma once
#include "Base.h"
class CSound_Manager;
BEGIN(Engine)
class CSound_Event final : public CBase
{
private:
	CSound_Event(CSound_Manager* pSound_Manager);
	virtual ~CSound_Event() = default;

public:
    void Restart(); //이벤트 재시작
    void Stop(bool allowFadeOut = true); // 이벤트를 정지한다
public: // Setters
    void SetPaused(bool pause);
    void SetVolume(float value);
    void SetPitch(float value);
    void SetParameter(const std::string& name, float value);
    void Set3DAttributes(const _float4x4& worldTrans);
public: // Getters
    _bool       IsValid();
    _bool       Is3D() const;
    _bool       GetPaused() const;
    _float      GetVolume() const;
    _float      GetPitch()  const;
    _float      GetParameter(const std::string& name);
    
protected:
    friend class CSound_Manager;
	CSound_Event(CSound_Manager* pSound_Manager, _uint id);
protected:
	CSound_Manager* m_pSound_Manager;
	_uint			m_iId;
public:
	void Free();
};

END
