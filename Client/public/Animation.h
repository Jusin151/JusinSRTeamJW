
#pragma once

class CAnimation
{
public:
	CAnimation(int iStartFrame, int iEndFrame, float fFrameTime = 0.05f)
		: m_iStartFrame(iStartFrame), m_iEndFrame(iEndFrame), m_iCurrentFrame(iStartFrame), m_fFrameTime(fFrameTime), m_fAccTime(0.f) 
	{
	}

	void Update(float fDeltaTime)
	{
		m_fAccTime += fDeltaTime;
		if (m_fAccTime >= m_fFrameTime)
		{
			++m_iCurrentFrame;
			if (m_iCurrentFrame > m_iEndFrame)
				m_iCurrentFrame = m_iStartFrame;
			m_fAccTime = 0.f;
		}
	}
	bool IsFinished() const
	{
		// 현재 프레임이 끝 프레임인지
		return m_iCurrentFrame >= m_iEndFrame;
	}

	int GetCurrentFrame() const { return m_iCurrentFrame; }
	void Reset() { m_iCurrentFrame = m_iStartFrame; m_fAccTime = 0.f; }

private:
	int m_iStartFrame, m_iEndFrame, m_iCurrentFrame;
	float m_fFrameTime, m_fAccTime;
};
