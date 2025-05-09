﻿#pragma once
#include "Client_Defines.h"
#include "Pattern_Base.h"
#include "HellBoss.h"

class CPattern_Morph : public CPattern_Base
{
public:
	CPattern_Morph();
	void Update(CHellBoss* pBoss, float fDeltaTime);
	void Execute(CHellBoss* pBoss, float fDeltaTime);
	bool Is_Finished() const { return !m_bStarted; }
private:
	_bool m_bStarted = { false };
	float m_fAccTime = { 0.f };
	float m_fDuration = { 2.5f };


};
