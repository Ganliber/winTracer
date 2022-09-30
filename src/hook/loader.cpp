#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include "detour/detours.h"

using namespace std;

#pragma comment(lib, "detour/detours.lib")

wchar_t* fileName;//一个文件名最长为100个字符

void char2wchar(const char* cchar)
{
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	fileName = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), fileName, len);
	fileName[len] = '\0';
	return;
}

int main(int argc, char* argv[]) { //参数一：dll file ; 参数二：host file
	
	if (argc != 3) {
		cout << "err!" << endl;
		return 0;
	}
	
	char2wchar(argv[2]);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	//LPCSTR dllName[] = { "testDll.dll" };
	LPCSTR dllName[] = { argv[1] };

	//在同一目录下,将dllTest.dll注入到宿主程序testProgram.exe中
	if (DetourCreateProcessWithDllsW(fileName, NULL, NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, NULL, &si, &pi, 1, dllName, NULL))
	{
		//W后缀代表是unicode编码,因此字符串前应该加上L
		//MessageBoxW(NULL, L"Inject succeed!", L"inject", NULL);
		cout << "Hooking succeed!\n";
		ResumeThread(pi.hThread);
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}

