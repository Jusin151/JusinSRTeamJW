#pragma once

class CHellBoss;

class CPattern_Attack_Base
{
public:
	virtual ~CPattern_Attack_Base() = default;
	virtual void Execute(CHellBoss* pBoss, float fDeltaTime) = 0;
};
