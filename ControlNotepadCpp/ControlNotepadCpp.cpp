#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h
#include <psapi.h>
#include <ctype.h>
#include <iostream>
#include <string.h>.
#include <stdio.h>
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib,"ntdll.lib")
using namespace std;


char* FindPattern(char* src, size_t srcLen, const char* pattern, size_t patternLen)
{
	char* cur = src;
	size_t curPos = 0;

	while (curPos < srcLen)
	{
		if (memcmp(cur, pattern, patternLen) == 0)
		{
			return cur;
		}

		curPos++;
		cur = &src[curPos];
	}
	return nullptr;
}

char* FindBytePatternInProcessMemory(HANDLE process, const char* pattern, size_t patternLen)
{
	MEMORY_BASIC_INFORMATION memInfo;
	char* basePtr = (char*)0x0;

	while (VirtualQueryEx(process, (void*)basePtr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)))
	{
		const DWORD mem_commit = 0x1000;
		const DWORD page_readwrite = 0x04;
		if (memInfo.State == mem_commit && memInfo.Protect == page_readwrite)
		{
			char* remoteMemRegionPtr = (char*)memInfo.BaseAddress;
			char* localCopyContents = (char*)malloc(memInfo.RegionSize);

			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(process, memInfo.BaseAddress, localCopyContents, memInfo.RegionSize, &bytesRead))
			{
				char* match = FindPattern(localCopyContents, memInfo.RegionSize, pattern, patternLen);

				if (match)
				{
					uint64_t diff = (uint64_t)match - (uint64_t)(localCopyContents);
					char* processPtr = remoteMemRegionPtr + diff;
					return processPtr;
				}
			}
			free(localCopyContents);
		}
		basePtr = (char*)memInfo.BaseAddress + memInfo.RegionSize;
	}
	return nullptr;
}

DWORD GetProcessByExeName(const wchar_t* ExeName)
{
	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//MessageBoxW(NULL, L"Error CreateToolhelp32Snapshot", MB_OK | MB_ICONERROR);
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

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
