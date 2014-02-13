#pragma once
#include <list>

class FpsCounter
{
public:
	FpsCounter():m_lcount(0),m_rcount(0)
	{

	}

	void IncLogicFrame()
	{
		m_lcount++;
		m_lframes.push_back(GetTickCount());
	}

	void IncRenderFrame()
	{
		m_rcount++;
		m_rframes.push_back(GetTickCount());
	}

	DWORD GetLogicFrameCount()
	{
		return m_lcount;
	}

	DWORD GetRenderFrameCount()
	{
		return m_rcount;
	}

	DWORD GetLogicFps()
	{
		DWORD now=GetTickCount();
		for(std::list<DWORD>::iterator it=m_lframes.begin();it!=m_lframes.end();)
		{
			if(*it<now-1000)
			{
				it=m_lframes.erase(it);
			}
			else
			{
				return m_lframes.size();
			}
		}
		return m_lframes.size();
	}

	DWORD GetRenderFps()
	{
		DWORD now=GetTickCount();
		for(std::list<DWORD>::iterator it=m_rframes.begin();it!=m_rframes.end();)
		{
			if(*it<now-1000)
			{
				it=m_rframes.erase(it);
			}
			else
			{
				return m_rframes.size();
			}
		}
		return m_rframes.size();
	}

private:
	DWORD m_lcount;
	DWORD m_rcount;
	std::list<DWORD> m_lframes;
	std::list<DWORD> m_rframes;
};