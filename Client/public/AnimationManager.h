#pragma once

#include "Client_Defines.h"
#include "Animation.h"

class CAnimationManager
{
public:
    CAnimationManager() : m_pCurAnim(nullptr) {}
    ~CAnimationManager()
    {
        ClearAnimations();
    }

    void AddAnimation(const string& strKey, int iStartFrame, int iEndFrame, float fFrameTime = 0.05f)
    {
    
        auto it = m_mapAnimations.find(strKey);
        if (it != m_mapAnimations.end())
        {
            delete it->second;
            m_mapAnimations.erase(it);
        }
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
       
        return m_pCurAnim ? m_pCurAnim->IsFinished() : false;
    }

    void Update(float fDeltaTime)
    {
        if (m_pCurAnim)
            m_pCurAnim->Update(fDeltaTime);
    }

    int GetCurrentFrame() const { return m_pCurAnim ? m_pCurAnim->GetCurrentFrame() : 0; }

    void RemoveAnimation(const string& strKey)
    {
        auto it = m_mapAnimations.find(strKey);
        if (it != m_mapAnimations.end())
        {
            if (m_pCurAnim == it->second)
                m_pCurAnim = nullptr;
            delete it->second;
            m_mapAnimations.erase(it);
        }
    }


    void ClearAnimations()
    {
        for (auto& pair : m_mapAnimations)
        {
            delete pair.second;
        }
        m_mapAnimations.clear();
        m_pCurAnim = nullptr;
    }

private:
    map<string, CAnimation*> m_mapAnimations;
    CAnimation* m_pCurAnim;
};
