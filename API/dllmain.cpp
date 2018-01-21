// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"
#include "Server.h"

void StartNewThread();
void ServerProc();

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		StartNewThread();
		break;
	}
	return TRUE;
}

void StartNewThread()
{
	if (!GetModuleHandle(L"samp.dll"))
		return;

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&ServerProc, 0, 0, 0);
}

void ServerProc()
{
	Server server;
	while (true)
		Sleep(100);
}