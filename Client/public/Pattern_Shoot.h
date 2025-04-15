#pragma once
#include "Pattern_Base.h"
#include "HellBoss.h"

class CPattern_Shoot : public CPattern_Base
{
public:
	CPattern_Shoot();

	bool Is_Finished() const;

	void Execute(CHellBoss* pBoss, float fDeltaTime);
	virtual void Update(CHellBoss* pBoss, float fDeltaTime);
private:
	_bool m_bStarted = { false };
	_float m_fAccTime = { 0.f };
	_float m_fDuration = { 2.5f };
	_bool m_bHasFired = { false };
	_bool m_bFinished = { false };
	_int     m_iFiredCount;   // 이미 발사한 총알 수
	_float   m_fNextFireTime; // 다음 총알 발사까지의 누적 시간 기준


};
