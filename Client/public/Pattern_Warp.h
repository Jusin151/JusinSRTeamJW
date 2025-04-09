#pragma once
#include "Client_Defines.h"
#include "Pattern_Base.h"
#include "HellBoss.h"

class CPattern_Warp : public CPattern_Base
{
public:
	CPattern_Warp();

	void Execute(CHellBoss* pBoss, float fDeltaTime);
	void Update(CHellBoss* pBoss, float fDeltaTime);
	bool Is_Finished() const { return !m_bStarted; }
private:
	_bool m_bStarted = { false };
	_bool m_bWarped = { false };
	_float m_fAccTime = { 0.f };
	_float m_fDuration = { 2.5f };



};

