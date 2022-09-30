#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include "detour/detours.h"

using namespace std;

#pragma comment(lib, "detour/detours.lib")

wchar_t* fileName;//һ���ļ����Ϊ100���ַ�

void char2wchar(const char* cchar)
{
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	fileName = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), fileName, len);
	fileName[len] = '\0';
	return;
}

int main(int argc, char* argv[]) { //����һ��dll file ; ��������host file
	
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

	//��ͬһĿ¼��,��dllTest.dllע�뵽��������testProgram.exe��
	if (DetourCreateProcessWithDllsW(fileName, NULL, NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, NULL, &si, &pi, 1, dllName, NULL))
	{
		//W��׺������unicode����,����ַ���ǰӦ�ü���L
		//MessageBoxW(NULL, L"Inject succeed!", L"inject", NULL);
		cout << "Hooking succeed!\n";
		ResumeThread(pi.hThread);
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}

