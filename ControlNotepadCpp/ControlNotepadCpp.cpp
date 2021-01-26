#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <psapi.h>
#include <ctype.h>
#include <iostream>
#include <string.h>.
#include <winternl.h>
#pragma comment(lib,"ntdll.lib")
using namespace std;
DWORD GetProcessByExeName(const wchar_t* ExeName)
{
	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (Process32FirstW(hProcessSnap, &pe32))
	{
		do
		{
			if (_wcsicmp(pe32.szExeFile, ExeName) == 0)
			{
				CloseHandle(hProcessSnap);
				return pe32.th32ProcessID;
			}
		} while (Process32NextW(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return 0;
}
int main()
{
	cout << GetProcessByExeName(L"notepad.exe");
	return 0;
}
