#pragma once
#include "Client_Defines.h"
#include "Pattern_Attack_Base.h"
#include "HellBoss.h"

class CPattern_EyeBlast : public CPattern_Attack_Base
{
public:
	CPattern_EyeBlast();

	void Execute(CHellBoss* pBoss, float fDeltaTime);
	void Update(CHellBoss* pBoss, float fDeltaTime);
	bool Is_Finished() const { return !m_bStarted; }
private:
	_bool m_bStarted = { false };
	_float m_fAccTime = { 0.f };
	_float m_fDuration = { 2.5f };
	_bool m_bHasFired = { false };


};

