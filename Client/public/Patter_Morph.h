#pragma once
#include "Pattern_Attack_Base.h"
#include "HellBoss.h"

class CPatter_Morph : public CPattern_Attack_Base
{
public:
	CPatter_Morph();

	void Execute(CHellBoss* pBoss, float fDeltaTime)override;
	
private:
	_bool m_bStarted = { false };
	_float m_fAccTime = { 0.f };
	_float m_fDuration = { 2.5f };
};
