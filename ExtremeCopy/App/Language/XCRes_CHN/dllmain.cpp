// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//#pragma data_seg("XCWndPositionSlot") // 
//unsigned int g_nWndPositionSlot_ShelExtDLL = 0;
//#pragma data_seg()
//#pragma comment(linker,"/SECTION:XCWndPositionSlot,RWS")


//extern "C" unsigned int g_nWndPositionSlot_ShelExtDLL ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

