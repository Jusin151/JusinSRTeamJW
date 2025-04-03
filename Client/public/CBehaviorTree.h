#pragma once
#include "CCompositeNode.h"
#include "CTaskNode.h"

class CBehaviorTree 
{
public:
	~CBehaviorTree()
	{
		Safe_Delete(m_tRoot);
	}
public:
    void Initialize();
	void Run()
	{
		if (m_tRoot)
		{
			m_tRoot->Execute();
		}
	}
	void Set_Root(INode* root) { m_tRoot = root; }
private:
	INode* m_tRoot;
};

