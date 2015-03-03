/**
* @author Micha³ Gawin
*/


#ifndef _CRITICAL_SECTION_H_VD_
#define _CRITICAL_SECTION_H_VD_

#include "VirtualDesktop.h"


class CCriticalSection
{
	static LPCRITICAL_SECTION s_pcriticalSection;
public:
	static void Init();
	CCriticalSection();
	~CCriticalSection();
	static void Leave();
};

#endif //_CRITICAL_SECTION_H_VD_