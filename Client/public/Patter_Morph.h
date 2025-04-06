#pragma once
#include "Client_Defines.h"
#include "Pattern_Attack_Base.h"
#include "HellBoss.h"

class CPatter_Morph : public CPattern_Attack_Base
{
public:
	CPatter_Morph();
	void Update(CHellBoss* pBoss, float fDeltaTime);
	void Execute(CHellBoss* pBoss, float fDeltaTime);
	bool Is_Finished() const { return !m_bStarted; }
private:
	_bool m_bStarted = { false };
	float m_fAccTime = { 0.f };
	float m_fDuration = { 2.5f };


};
