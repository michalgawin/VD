#include "CriticalSection.h"

LPCRITICAL_SECTION CCriticalSection::s_pcriticalSection = NULL;


void CCriticalSection::Init()
{
	if (!CCriticalSection::s_pcriticalSection) CCriticalSection::s_pcriticalSection = new CRITICAL_SECTION;
	InitializeCriticalSection(CCriticalSection::s_pcriticalSection);
}


CCriticalSection::CCriticalSection()
{
	if (CCriticalSection::s_pcriticalSection) EnterCriticalSection(CCriticalSection::s_pcriticalSection);
}


CCriticalSection::~CCriticalSection()
{
	if (CCriticalSection::s_pcriticalSection) LeaveCriticalSection(CCriticalSection::s_pcriticalSection);
}


void CCriticalSection::Leave()
{
	if (CCriticalSection::s_pcriticalSection)
	{
		DeleteCriticalSection(CCriticalSection::s_pcriticalSection);
		delete CCriticalSection::s_pcriticalSection;
		CCriticalSection::s_pcriticalSection = NULL;
	}
}
