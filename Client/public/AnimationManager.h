#pragma once

#include "Client_Defines.h"
#include "Animation.h"


class CAnimationManager
{
public:
	CAnimationManager() : m_pCurAnim(nullptr) {}
	~CAnimationManager()
	{
		for (auto& pair : m_mapAnimations)
			delete pair.second;
	}

	void AddAnimation(const string& strKey, int iStartFrame, int iEndFrame, float fFrameTime = 0.05f)
	{
		m_mapAnimations[strKey] = new CAnimation(iStartFrame, iEndFrame, fFrameTime);
	}

	void SetCurrentAnimation(const string& strKey)
	{
		auto it = m_mapAnimations.find(strKey);
		if (it != m_mapAnimations.end())
		{
			if (m_pCurAnim != it->second)
			{
				m_pCurAnim = it->second;
				m_pCurAnim->Reset();
			}
		}
	}
	bool IsFinished() const
	{
		// 현재 애니메이션이 없으면 false, 있으면 현재 애니메이션의 IsFinished를 호출
		return m_pCurAnim ? m_pCurAnim->IsFinished() : false;
	}
	void Update(float fDeltaTime)
	{
		if (m_pCurAnim)
			m_pCurAnim->Update(fDeltaTime);
	}

	int GetCurrentFrame() const { return m_pCurAnim ? m_pCurAnim->GetCurrentFrame() : 0; }

private:
	map<string, CAnimation*> m_mapAnimations;
	CAnimation* m_pCurAnim;
};
