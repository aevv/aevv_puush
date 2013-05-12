#undef UNICODE
#define WIN32_LEAN_AND_MEAN //so winsock.h doesnt mess with winsock2
#include <stdio.h>
#include <sstream>
#include <string>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "mhook-lib\mhook.h"
#include "Winsock2.h"
using namespace std;

typedef int (WINAPI *pWSAConnect)(SOCKET, const struct sockaddr *, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
int WINAPI MyWSAConnect(SOCKET, const struct sockaddr *, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
pWSAConnect pOrigWSA = NULL;

BOOL hooked;

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{	
	switch(Reason)
	{
	case DLL_PROCESS_ATTACH:
		pOrigWSA =  (pWSAConnect) GetProcAddress(GetModuleHandle("Ws2_32.dll"), "WSAConnect");
		hooked =	Mhook_SetHook((PVOID*)&pOrigWSA,MyWSAConnect);			
		if (hooked == FALSE)
		{
				MessageBoxA(NULL,"Injection failed", "Info", MB_ICONEXCLAMATION);
		}
		break;
	case DLL_PROCESS_DETACH:		
		Mhook_Unhook((PVOID*)&pOrigWSA);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

int WINAPI  MyWSAConnect(SOCKET socket, const struct sockaddr * sockAddr, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeDatas, LPQOS lpSQOSss, LPQOS lpSQOSas) {
	sockaddr_in * newAddr = (sockaddr_in *)sockAddr;
	newAddr->sin_port = htons(80);
	newAddr->sin_addr.S_un.S_addr = inet_addr("91.121.201.125");
	return pOrigWSA(socket, (sockaddr*)newAddr, namelen, lpCallerData, lpCalleeDatas, lpSQOSss, lpSQOSas);
}