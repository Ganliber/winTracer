#include "pch.h"
#include "framework.h"
#include "stdio.h"
#include "stdarg.h"
#include "windows.h"
#include "nlohmann/json.hpp"
#include "detour/detours.h"
#include "libcurl/include/curl/curl.h"
#include "base64.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unordered_map>
#include <WinSock2.h>
#include <set>

#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "libcurl/lib/libcurl_a_debug.lib")
#pragma comment(lib, "detour/detours.lib")
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll

using json = nlohmann::json;
using namespace std;

const int APINUM = 22;
//set<int> isExistedHeapHandler;
map<int, int> isExistedHeapHandler;
SYSTEMTIME st;

void attachAllDetours();
void detachAllDetours();
int sendApiInformation(json&);


/* MessageBox 
*	由于VS2022下MessageBoxW/MessageBoxA的版本是要申请堆内存的，故在截获时应该
*	先取消所有的堆相关的 attachDetours 然后再作进一步的处理
*/

/* MessageBoxA */
static int(WINAPI* OldMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) = MessageBoxA;
extern "C" __declspec(dllexport) int WINAPI NewMessageBoxA(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType)
{
	detachAllDetours();
	printf_s("MessageBoxA hooked!");
	
	json info;
//初始化
	info["MessageBoxA"]["Name"] = "MessageBoxA";
//参数
	//所有者窗口的句柄
	char hdl[20];
	sprintf_s(hdl, "%08X", hWnd);
	std::cout << "hWnd : " << hWnd << "\thdl : " << hdl << endl;
	info["MessageBoxA"]["hWnd"] = string("0x") + string(hdl);

	//消息框中的文本地址
	if (lpText != NULL)
		info["MessageBoxA"]["lpText"] = string(lpText);
	else
		info["MessageBoxA"]["lpText"] = "";

	//消息框标题的地址
	if (lpCaption != NULL)
		info["MessageBoxA"]["lpCaption"] = string(lpCaption);
	else
		info["MessageBoxA"]["lpCaption"] = "";

	//消息框样式
	info["MessageBoxA"]["uType"] = uType;

	info["MessageBoxA"]["err"] = NULL;

	//返回值
	//auto result = OldMessageBoxA(hWnd, lpText, lpCaption, uType); //原结果
	int result = 1;

	info["MessageBoxA"]["return"] = result;
	cout << "MessageBoxA Hooked Successfully!" << endl;
	//cout << info.dump();
	sendApiInformation(info);//发送http报文给本地服务器

	attachAllDetours();
	return result;
}
/* MessageBoxW */
static int(WINAPI* OldMessageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) = MessageBoxW;
extern "C" __declspec(dllexport) int WINAPI NewMessageBoxW(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType)
{
	detachAllDetours();
	printf_s("MessageBoxW hooked!");
	json info;
	//初始化	
	info["MessageBoxW"]["Name"] = "MessageBoxW";
	//窗口句柄
	char hdl[20];
	sprintf_s(hdl, "%08X", hWnd);
	info["MessageBoxW"]["hWnd"] = string("0x") + string(hdl);
	//info["MessageBoxW"]["hWnd"] = int(hWnd);

	//消息框中的文本地址
	if (lpText != NULL) {
		char tmpT[100];
		sprintf_s(tmpT, "%ws", lpText);
		info["MessageBoxW"]["lpText"] = string(tmpT);
	}
	else {
		info["MessageBoxW"]["lpText"] = "";
	}
		

	//消息框标题的地址
	if (lpCaption != NULL) {
		char tmp[100];
		sprintf_s(tmp, "%ws", lpCaption);
		info["MessageBoxW"]["lpCaption"] = string(tmp);
	}
	else {
		info["MessageBoxW"]["lpCaption"] = "";
	}
		
	info["MessageBoxW"]["err"] = "NULL";

	//消息框样式
	info["MessageBoxW"]["uType"] = uType;

	//返回值
	//auto result = OldMessageBoxW(hWnd, lpText, lpCaption, uType); //原结果
	int result = 1;
	info["MessageBoxW"]["return"] = result;

	cout << "MessageBoxW Hooked Successfully!" << endl;
	cout << info.dump();
	sendApiInformation(info);//发送http报文给本地服务器

	attachAllDetours();
	return result;
}





/* File Handling */

/* CreateFile (the same as CreateFileW) */
static HANDLE(WINAPI* OldCreateFile)(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) = CreateFile;
extern "C" __declspec(dllexport)HANDLE WINAPI NewCreateFile(
	LPCWSTR               lpFileName,				//指向文件名的指针
	DWORD                 dwDesiredAccess,			//访问（读写）模式
	DWORD                 dwShareMode,				//共享模式
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,		//指向安全属性的指针
	DWORD                 dwCreationDisposition,	//如何创建
	DWORD                 dwFlagsAndAttributes,		//文件属性
	HANDLE                hTemplateFile				//处理要复制的属性的文件
) {
	detachAllDetours();
	HANDLE hFile = OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (GetFileType(hFile) != FILE_TYPE_DISK)
		return hFile;
	json info;
	info["CreateFile"]["Name"] = "CreateFile";

	char tmpT[260];// 文件名最长 256 bytes
	sprintf_s(tmpT, "%ws", lpFileName);
	info["CreateFile"]["lpFileName"] = string(tmpT);

	info["CreateFile"]["dwDesiredAccess"] = dwDesiredAccess;
	info["CreateFile"]["dwShareMode"] = dwShareMode;

	info["CreateFile"]["lpSecurityAttributes"] = int(lpSecurityAttributes);//指向安全属性的指针
	info["CreateFile"]["dwCreationDisposition"] = dwCreationDisposition;
	info["CreateFile"]["dwFlagsAndAttributes"] = dwFlagsAndAttributes;

	char tmp[20];
	sprintf_s(tmp, "%08X", hTemplateFile);
	info["CreateFile"]["hTemplateFile"] = string("0x") + string(tmp);

	sprintf_s(tmp, "%08X", hFile);
	info["CreateFile"]["return"] = string("0x") + string(tmp);

	if (hFile == INVALID_HANDLE_VALUE) {
		info["CreateFile"]["err"] = GetLastError();
	}
	else {
		info["CreateFile"]["err"] = "NULL";
	}

	sendApiInformation(info);
	attachAllDetours();
	return hFile;
}



/* OpenFile
*	如果函数成功，返回值指定一个文件句柄
*	如果函数失败，返回值为HFILE_ERROR
*/
static HFILE(WINAPI* OldOpenFile)(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) = OpenFile;
extern "C" __declspec(dllexport) HFILE WINAPI NewOpenFile(
	LPCSTR lpFileName,			//指向文件名的指针
	LPOFSTRUCT lpReOpenBuff,	//指向缓冲区的文件信息
	UINT uStyle					//动作和属性
) 
{


	detachAllDetours();
	auto hFile = OldOpenFile(lpFileName, lpReOpenBuff, uStyle);

	json info;
	info["OpenFile"]["Name"] = "OpenFile";
	info["OpenFile"]["lpFileName"] = string(lpFileName);

	char tmp[20];
	sprintf_s(tmp, "%08X", lpReOpenBuff);
	info["OpenFile"]["lpReOpenBuff"] = string("0x") + string(tmp);
	info["OpenFile"]["uStyle"] = uStyle;

	if (hFile == HFILE_ERROR) {
		info["OpenFile"]["return"] = "HFILE_ERROR";
		info["OpenFile"]["err"] = GetLastError();
	}
	else {
		info["OpenFile"]["return"] = hFile;
		info["OpenFile"]["err"] = NULL;
	}
	sendApiInformation(info);
	attachAllDetours();
	return hFile;
}

static BOOL(WINAPI* OldCloseHandle)(HANDLE hObject) = CloseHandle;
extern "C" __declspec(dllexport) BOOL WINAPI NewCloseHandle(HANDLE hObject) {
	BOOL result = OldCloseHandle(hObject);
	if (GetFileType(hObject) != FILE_TYPE_DISK)
		return result;
	json info;
	info["CloseHandle"]["Name"] = "CloseHandle";
	char tmp[20];
	sprintf_s(tmp, "%08X", hObject);
	info["CloseHandle"]["hObject"] = string("0x") + string(tmp);
	info["CloseHandle"]["return"] = result;
	if (result == 0) {
		info["CloseHandle"]["err"] = GetLastError();
	}
	else {
		info["CloseHandle"]["err"] = "NULL";
	}
	sendApiInformation(info);
	return result;
}

static BOOL(WINAPI* OldReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = ReadFile;
extern "C" __declspec(dllexport) BOOL WINAPI NewReadFile(
	HANDLE hFile,					//要处理的文件
	LPVOID lpBuffer,				//接收数据的缓冲区的地址
	DWORD nNumberOfBytesToRead,		//要读取的字节数
	LPDWORD lpNumberOfBytesRead,	//读取的字节数的地址,指向读取的字节数
	LPOVERLAPPED lpOverlapped		//数据结构的地址
) {
	detachAllDetours();
	auto result = OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (GetFileType(hFile) != FILE_TYPE_DISK)
		return result;

	json info;
	info["ReadFile"]["Name"] = "ReadFile";

	char tmp[20];
	sprintf_s(tmp, "%08X", hFile);
	info["ReadFile"]["hFile"] = string("0x") + string(tmp);

	sprintf_s(tmp, "%08X", lpBuffer);
	info["ReadFile"]["lpBuffer"] = string("0x") + string(tmp);//要读取的字节数

	info["ReadFile"]["nNumberOfBytesToRead"] = nNumberOfBytesToRead;

	//if (lpNumberOfBytesRead != NULL) {
	//	info["ReadFile"]["ReadFile"]["lpNumberOfBytesReadValue"] = *lpNumberOfBytesRead;
	//	info["ReadFile"]["ReadFile"]["lpBufferValue"] = base64_encode((const unsigned char*)lpBuffer, (unsigned int)*lpNumberOfBytesRead);
	//}
	//else {
	//	info["ReadFile"]["lpNumberOfBytesReadValue"] = 0;
	//	info["ReadFile"]["lpBufferValue"] = NULL;
	//}

	info["ReadFile"]["lpOverlapped"] = int(lpOverlapped);
	info["ReadFile"]["return"] = result;
	if (result == FALSE) {
		info["ReadFile"]["errorCode"] = GetLastError();
	}
	else {
		info["ReadFile"]["err"] = "NULL";
	}

	sendApiInformation(info);
	attachAllDetours();
	return result;
}


static BOOL(WINAPI* OldWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;
extern "C" __declspec(dllexport) BOOL WINAPI NewWriteFile(
	HANDLE hFile,						//处理要写入的文件
	LPCVOID lpBuffer,					//指向要写入文件的数据的指针
	DWORD nNumberOfBytesToWrite,		//要写入的字节数	
	LPDWORD lpNumberOfBytesWritten,		//指向写入字节数的指针
	LPOVERLAPPED lpOverlapped			//指向重叠 I/O 所需的结构的指针
) {
	detachAllDetours();
	BOOL result = OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	if (GetFileType(hFile) != FILE_TYPE_DISK)
		return result;
	json info;

	info["WriteFile"]["Name"] = "WriteFile";
	char tmp[20];
	sprintf_s(tmp, "%08X", hFile);
	info["WriteFile"]["hFile"] = string("0x") + string(tmp);

	sprintf_s(tmp, "%08X", lpBuffer);
	info["WriteFile"]["lpBuffer"] = string("0x") + string(tmp);
	
	info["WriteFile"]["nNumberOfBytesToWrite"] = nNumberOfBytesToWrite;

	sprintf_s(tmp, "%08X", lpNumberOfBytesWritten);
	info["WriteFile"]["lpNumberOfBytesWritten"] = string("0x") + string(tmp);

	info["WriteFile"]["lpOverlapped"] = int(lpOverlapped);
	info["WriteFile"]["return"] = result;

	if (result == FALSE) {
		info["WriteFile"]["err"] = GetLastError();
	}
	else if (nNumberOfBytesToWrite==5) {
		info["WriteFile"]["err"] = "Try to change .exe file";
	}
	else if (nNumberOfBytesToWrite == 6) {
		info["WriteFile"]["err"] = "Try to change .dll file";
	}
	else if (nNumberOfBytesToWrite == 7) {
		info["WriteFile"]["err"] = "Try to change .ocx file";
	}
	else {
		info["WriteFile"]["err"] = "NULL";
	}
	sendApiInformation(info);
	attachAllDetours();
	return result;
}






/* Regedit */

/* regCreateKeyEx
* 该函数用于创建指定的key，如果该key已经存在于注册表中，则打开它
*/
static LSTATUS(WINAPI* OldRegCreateKeyEx)(
	HKEY                        hKey,	//该函数用于创建指定的key，如果该key已经存在于注册表中，则打开它
	LPCWSTR                     lpSubKey,//String，欲创建的新子项的名字
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,	//Long，指定用于装载新子项句柄的一个变量
	LPDWORD                     lpdwDisposition	
										//用于装载下列某个常数的一个变量：
										//REG_CREATED_NEW_KEY——新建的一个子项REG_OPENED_EXISTING_KEY——打开一个现有的项
) = RegCreateKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCreateKeyEx(
	HKEY                        hKey,
	LPCWSTR                     lpSubKey,
	DWORD                       Reserved,
	LPWSTR                      lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
) {
	auto result = OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	json info;
	
	//名
	info["RegCreateKeyEx"]["Name"] = "RegCreateKeyEx";
	
	//返回值
	info["RegCreateKeyEx"]["return"] = result;

	if (dwOptions == REG_OPTION_VOLATILE)
		info["RegCreateKeyEx"]["err"] = "Self-starting executable file item";
	else if (result == 0)
		info["RegCreateKeyEx"]["err"] = "NULL";
	else
		info["RegCreateKeyEx"]["err"] = "RegCreateKeyEx failed!";
	
	//参数
	char hexNumber[20];
	char tmp[100];

	sprintf_s(tmp, "%ws", lpSubKey);
	info["RegCreateKeyEx"]["lpSubKey"] = string(tmp);

	sprintf_s(hexNumber, "%08X", Reserved);
	info["RegCreateKeyEx"]["Reserved"] = string("0x") + string(hexNumber);

	sprintf_s(tmp, "%ws", lpClass);
	info["RegCreateKeyEx"]["lpClass"] = string(tmp);

	sprintf_s(hexNumber, "%08X", dwOptions);
	info["RegCreateKeyEx"]["dwOptions"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", samDesired);
	info["RegCreateKeyEx"]["samDesired"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", lpSecurityAttributes);
	info["RegCreateKeyEx"]["lpSecurityAttributes"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", phkResult);
	info["RegCreateKeyEx"]["phkResult"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", lpdwDisposition);
	info["RegCreateKeyEx"]["lpdwDisposition"] = string("0x") + string(hexNumber);

	sendApiInformation(info);

	return result;
}



/* 关闭 RegCloseKey */
static LSTATUS(WINAPI* OldRegCloseKey)(HKEY hKey) = RegCloseKey;
extern "C" __declspec(dllexport) LSTATUS WINAPI NewRegCloseKey(HKEY hKey) 
{
	//Long，零（ERROR_SUCCESS）表示成功。其他任何值都代表一个错误代码
	auto result = OldRegCloseKey(hKey);
	
	json info;
	
	if (result == 0)
		info["RegCloseKey"]["err"] = "NULL";
	else
		info["RegCloseKey"]["err"] = "RegCloseKey failed!";

	info["RegCloseKey"]["Name"] = "RegCloseKey";

	char hexNumber[20];
	sprintf_s(hexNumber, "%08X", hKey);
	info["RegCloseKey"]["hKey"] = string("0x") + string(hexNumber);

	info["RegCloseKey"]["return"] = result;

	sendApiInformation(info);

	return result;
}



/* 读 RegOpenKeyEx */
static LSTATUS(WINAPI* OldRegOpenKeyEx)(
	HKEY    hKey,
	LPCWSTR lpSubKey,
	DWORD   ulOptions,
	REGSAM  samDesired,
	PHKEY   phkResult
	) = RegOpenKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegOpenKeyEx(
	HKEY    hKey,		//Long，一个已打开项的句柄，或指定一个标准项名
	LPCWSTR lpSubKey,	//String，欲打开注册表项的名字
	DWORD   ulOptions,	//Long，未用，设为零
	REGSAM  samDesired,	//Long，带有前缀KEY_??的一个或多个常数。它们的组合描述了允许对这个项进行哪些操作
	PHKEY   phkResult	//Long，用于装载打开项的名字的一个变量
) {
	auto result = OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	json info;
	char tmp[100];
	char hexNumber[20];

	if (result == 0) {
		info["RegOpenKeyEx"]["err"] = "NULL";
	}
	else {
		info["RegOpenKeyEx"]["err"] = "RegOpenKeyEx failed!";
	}
	
	info["RegOpenKeyEx"]["Name"] = "RegOpenKeyEx";

	sprintf_s(hexNumber, "%08X", hKey);
	info["RegOpenKeyEx"]["hKey"] = string("0x") + string(hexNumber);

	sprintf_s(tmp, "%ws", lpSubKey);
	info["RegOpenKeyEx"]["lpSubKey"] = string(tmp);

	sprintf_s(hexNumber, "%08X", ulOptions);
	info["RegOpenKeyEx"]["ulOptions"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", samDesired);
	info["RegOpenKeyEx"]["samDesired"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", phkResult);
	info["RegOpenKeyEx"]["phkResult"] = string("0x") + string(hexNumber);

	sendApiInformation(info);

	return result;
}



/* 写 RegSetValueEx */
static LSTATUS(WINAPI* OldRegSetValueEx)(
	HKEY       hKey,
	LPCWSTR    lpValueName,
	DWORD      Reserved,
	DWORD      dwType,
	const BYTE* lpData,
	DWORD      cbData
	) = RegSetValueEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegSetValueEx(
	HKEY       hKey,			//Long，一个已打开项的句柄，或指定一个标准项名
	LPCWSTR    lpValueName,		//String，要设置值的名字
	DWORD      Reserved,		//Long，未用，设为零
	DWORD      dwType,			//Long，要设置的数量类型
	const BYTE * lpData,		//Any，包含数据的缓冲区中的第一个字节
	DWORD      cbData			//Long，lpData缓冲区的长度
) {
	auto result = OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
	
	json info;
	
	char tmp[100];
	char hexNumber[20];

	info["RegSetValueEx"]["Name"] = "RegSetValueEx";

	info["RegSetValueEx"]["return"] = result;

	if (result == 0) {
		info["RegSetValueEx"]["err"] = "Registry Value Changed!";
	}
	else {
		info["RegSetValueEx"]["err"] = "RegSetValueEx failed!";
	}

	sprintf_s(hexNumber, "%08X", hKey);
	info["RegSetValueEx"]["hKey"] = string("0x") + string(hexNumber);

	sprintf_s(tmp, "%ws", lpValueName);
	info["RegSetValueEx"]["lpSubKey"] = string(tmp);

	sprintf_s(hexNumber, "%08X", Reserved);
	info["RegSetValueEx"]["Reserved"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", dwType);
	info["RegSetValueEx"]["dwType"] = string("0x") + string(hexNumber);

	sprintf_s(tmp, "%ws", lpData);
	info["RegSetValueEx"]["lpSubKey"] = string(tmp);

	sprintf_s(hexNumber, "%08X", cbData);
	info["RegSetValueEx"]["dwType"] = string("0x") + string(hexNumber);

	sendApiInformation(info);

	return result;
}



/* 删除 RegDeleteValue */
static LSTATUS(WINAPI* OldRegDeleteValue)(
	HKEY    hKey,
	LPCWSTR lpValueName
) = RegDeleteValue;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegDeleteValue(
	HKEY    hKey,
	LPCWSTR lpValueName
) {
	auto result = OldRegDeleteValue(hKey, lpValueName);
	json info;

	char tmp[100];
	char hexNumber[20];

	info["RegDeleteValue"]["Name"] = "RegDeleteValue";

	info["RegDeleteValue"]["return"] = result;

	if (result == 0) {
		info["RegDeleteValue"]["err"] = "NULL";
	}
	else {
		info["RegDeleteValue"]["err"] = "RegDeleteValue failed!";
	}

	sprintf_s(hexNumber, "%08X", hKey);
	info["RegDeleteValue"]["hKey"] = string("0x") + string(hexNumber);

	sprintf_s(tmp, "%ws", lpValueName);
	info["RegDeleteValue"]["lpSubKey"] = string(tmp);

	sendApiInformation(info);

	return result;
}









/* Heap */
/*
* HeapCreate
* 函数创建一个专用堆对象，调用进程可以使用堆分配函数从中分配内存块
* 1.如果函数成功，则返回值是新创建的堆的句柄
* 2.如果函数失败，则返回值为 NULL
* dwInitialSize : 堆的初始大小，以字节为单位。此值确定为堆提交的初始内存量
* dwMaximumSize : 堆的最大大小，以字节为单位。堆创建函数将 dwMaximSize 向上舍入为系统页大小的倍数，然后在进程的虚拟地址空间中为堆保留该大小的块
*/
static HANDLE(WINAPI* OldHeapCreate)(DWORD fIOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;
extern "C" __declspec(dllexport)HANDLE WINAPI NewHeapCreate(DWORD fIOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
	HANDLE hdlHeap = OldHeapCreate(fIOptions, dwInitialSize, dwMaximumSize);
	isExistedHeapHandler[int(hdlHeap)] = 1;
	//if (fIOptions == HEAP_NO_SERIALIZE)
	//	isExistedHeapHandler.insert(int(hdlHeap));//私有堆访问关闭串行访问，可以访问的更快
	//else {
	//	//attachAllDetours();
	//	return hdlHeap;
	//}

	json info;
	info["HeapCreate"]["Name"] = "HeapCreate";

	char tmp[20];
	sprintf_s(tmp, "%08X", hdlHeap);
	info["HeapCreate"]["return"] = string("0x") + string(tmp);
	
// parameters
	info["HeapCreate"]["fIOptions"] = fIOptions;
	info["HeapCreate"]["dwInitialSize"] = dwInitialSize;
	info["HeapCreate"]["dwMaximumSize"] = dwMaximumSize;

	sprintf_s(tmp, "%08X", GetProcessHeap());
	info["HeapCreate"]["current_hHeap"] = string("0x") + string(tmp);//获取当前进程堆的句柄

	if (hdlHeap == NULL) {
		info["HeapCreate"]["err"] = GetLastError();
			//针对之前调用的api函数，用这个函数取得扩展错误信息
			//返回值类型为Long，由api函数决定。请参考api32.txt文件，其中列出了一系列错误常数；都以ERROR_前缀起头
	}
	else
		info["HeapCreate"]["err"] = "NULL";

	sendApiInformation(info);

	return hdlHeap;
}

static BOOL(WINAPI* OldHeapDestroy)(HANDLE hHeap) = HeapDestroy;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapDestroy(HANDLE hHeap) {

	std::cout << "HeapDestroy Hooked!" << endl;
	BOOL result;
	json info;
	//API名
	info["HeapDestroy"]["Name"] = "HeapDestroy";
	//参数
	char tmp[20];
	sprintf_s(tmp, "%08X", hHeap);
	//info["HeapCreate"]["return"] = int(hdlHeap);
	//info["HeapCreate"]["return"] = string("0x") + string(tmp);
	info["HeapDestroy"]["hHeap"] = string("0x") + string(tmp);

	auto iteration = isExistedHeapHandler.find(int(hHeap));
	if ( iteration != isExistedHeapHandler.end()) {
		result = 1;
		isExistedHeapHandler.erase(iteration);
	}
	else {
		result = 0;
	}

	if (result == 0) {
		info["HeapDestroy"]["err"] = "Error!";
	}
	else {
		info["HeapDestroy"]["err"] = "NULL";
	}	
	//返回值
	info["HeapDestroy"]["return"] = result;

	sendApiInformation(info);
	return OldHeapDestroy(hHeap);
}

static LPVOID(WINAPI* OldHeapAlloc)(HANDLE hHeap, DWORD dwFLAGS, SIZE_T dwBytes) = HeapAlloc;
extern "C" __declspec(dllexport)LPVOID WINAPI NewHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {

	if (isExistedHeapHandler.find(int(hHeap)) == isExistedHeapHandler.end()) {
		return OldHeapAlloc(hHeap, dwFlags, dwBytes);
	}

	LPVOID address = OldHeapAlloc(hHeap, dwFlags, dwBytes);

	std::cout << "HeapAlloc Hooked!" << endl;
	json info;
//API名
	info["HeapAlloc"]["Name"] = "HeapAlloc";
//参数
	char tmp[20];
	sprintf_s(tmp, "%08X", hHeap);
	info["HeapAlloc"]["hHeap"] = string("0x") + string(tmp);
	info["HeapAlloc"]["dwFlags"] = dwFlags;
	info["HeapAlloc"]["dwBytes"] = int(dwBytes);

	if (address == NULL) {
		info["HeapAlloc"]["err"] = "Heap Alloc Failed!";
	}
	else {
		info["HeapAlloc"]["err"] = "NULL";
	}
	//返回值
	sprintf_s(tmp, "%08X", address);
	info["HeapAlloc"]["return"] = string("0x") + string(tmp);
	
	sendApiInformation(info);

	return address;
}

static BOOL(WINAPI* OldHeapFree)(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) = HeapFree;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) {

	if (isExistedHeapHandler.find(int(hHeap)) == isExistedHeapHandler.end()) {
		return OldHeapFree(hHeap, dwFlags, lpMem);
	}

	std::cout << "HeapFree Hooked!" << endl;
	BOOL result;
	json info;
	if (HeapValidate(hHeap, 0, lpMem)==0) {
		//检查指定句柄hHeap的堆的连续内存块lpMem是否有效
		//此处是无效，可以断定为存在广义的<Heap double free>行为
		info["HeapFree"]["err"] = "Double Free: Terminated";
		std::cout << "Double Free";
		//返回值
		result = 0;
	}
	else {
		//返回值
		result = 1;
		
		info["HeapFree"]["err"] = "NULL";	
	}
	//API名
	info["HeapFree"]["Name"] = "HeapFree";
	//参数
	char tmp[20];
	sprintf_s(tmp, "%08X", hHeap);
	info["HeapFree"]["hHeap"] = string("0x") + string(tmp);
	info["HeapFree"]["dwFlags"] = dwFlags;

	sprintf_s(tmp, "%08X", lpMem);
	info["HeapFree"]["dwBytes"] = string("0x") + string(tmp);
	//返回值
	info["HeapFree"]["return"] = result;
	sendApiInformation(info);

	if (result == 0)
		return NULL;
	else
		return OldHeapFree(hHeap, dwFlags, lpMem);
}




/* Socket */

/* socket 
* The socket function creates a socket that is bound to a specific transport service provider.
| af       | int，标识的网络地址协议族。一般为AF_INET，为IPv4地址簇。AF_INET6为IPv6地址族 |
| type     | int，被创建的套接字类型 |
| protocol | int，使用的协议 |
*/
static SOCKET(WINAPI* Oldsocket)(int af, int type, int protocol) = socket;
extern "C" __declspec(dllexport) SOCKET WSAAPI Newsocket(int af, int type, int protocol) {
	auto socket = Oldsocket(af, type, protocol);
	//auto socket = 0;
	json info;
	info["socket"]["Name"] = "socket";
	info["socket"]["af"] = af;
	info["socket"]["type"] = type;
	info["socket"]["protocol"] = protocol;
	info["socket"]["return"] = socket;

	if (socket == INVALID_SOCKET) {
		info["socket"]["err"] = WSAGetLastError();
	}
	else {
		info["socket"]["err"] = "NULL";
	}
	sendApiInformation(info);
	return socket;
}



/* WSAStartup
* 
函数作用：初始化进程中的套接字组件。
函数返回值：int，成功，0。失败，返回错误码。
*
| wVersionRequested | WORD，指定调用者可以使用的Windows套按字最高版本。高8位标识次版本号，底8位标识主版本号。 |
| lpWSAData         | LPWSADATA，结构体WSADATA类型指针。用来接收套按字的详细信息 |
*/
static int(WSAAPI* OldWSAStartup)(WORD wVersionRequired, LPWSADATA lpWSAData) = WSAStartup;
extern "C" __declspec(dllexport) int WSAAPI NewWSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData) {
	auto status = OldWSAStartup(wVersionRequired, lpWSAData);
	//auto status = 0;
	json info;
	char hexNumber[20];
	char tmp[100];

	info["WSAStartup"]["Name"] = "WSAStartup";

	sprintf_s(hexNumber, "%08X", wVersionRequired);
	info["WSAStartup"]["wVersionRequired"] = string("0x") + string(hexNumber);

	info["WSAStartup"]["wVersionRequiredPrimaryVersion"] = LOBYTE(wVersionRequired);//主版本号
	info["WSAStartup"]["wVersionRequiredSecondaryVersion"] = HIBYTE(wVersionRequired);//次版本号

	sprintf_s(hexNumber, "%08X", lpWSAData);
	info["WSAStartup"]["lpWSAData"] = string("0x") + string(hexNumber);

	if (status == 0) {
		info["WSAStartup"]["err"] = "NULL";
	}
	else {
		info["WSAStartup"]["err"] = "WSAStartup initializes socket failed!";
	}

	info["WSAStartup"]["return"] = status;
	sendApiInformation(info);
	return status;
}



/* connect
* 函数作用：为一个套接字建立一个连接。
* 函数返回值：SOCKET，零（ERROR_SUCCESS）表示成功。其他任何值都代表一个错误代码
| s       | SOCKET，一个未建立连接的套接字标识符。                       |
| name    | const struct sockaddr *，一个sockaddr结构体指针，保存连接的信息。 |
	* ADDRESS_FAMILY sa_family;           // Address family.
| namelen | int，name结构体的大小，单位为字节                            |
*/
static int(WSAAPI* Oldconnect)(SOCKET s, const sockaddr* name, int namelen) = connect;
extern "C" __declspec(dllexport) int WSAAPI Newconnect(SOCKET s, const sockaddr * name, int namelen) {
	//auto status = Oldconnect(s, name, namelen);
	auto status = 0;

	json info;
	char hexNumber[20];
	char tmp[100];

	info["connect"]["Name"] = "connect";

	info["connect"]["s"] = s;

	sprintf_s(hexNumber, "%08X", name);
	info["connect"]["sockaddr"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", namelen);
	info["connect"]["namelen"] = string("0x") + string(hexNumber);

	struct sockaddr_in* sock = (struct sockaddr_in*)name;
	int port = ntohs(sock->sin_port);
	char IP[INET_ADDRSTRLEN];//INET_ADDRSTRLEN这个宏系统默认定义 16
	inet_ntop(AF_INET,&sock->sin_addr,IP,sizeof(IP));//成功的话此时IP地址保存在str字符串中
	
	info["connect"]["ip"] = string(IP);
	info["connect"]["port"] = port;

	info["connect"]["return"] = status;

	if (status == SOCKET_ERROR) {
		info["connect"]["err"] = WSAGetLastError();
	}
	else {
		info["connect"]["err"] = "NULL";
	}
	sendApiInformation(info);
	return status;
}


/* recv
*****The recv function receives data from a connected socket or a bound connectionless socket.*****
| s     | SOCKET，一个已建立连接的套接字标识符。 |
| buf   | char *，将要接收数据的缓冲区指针。     |
| len   | int，buf指向参数的大小，单位为字节     |
| flags | int，指定发送的方式，一般为0。         |
*/
static int(WSAAPI* Oldrecv)(SOCKET s, char* buf, int len, int flags) = recv;
extern "C" __declspec(dllexport) int WSAAPI Newrecv(SOCKET s, char* buf, int len, int flags) {
	//auto status = Oldrecv(s, buf, len, flags);
	auto status = 0;

	json info;
	char hexNumber[20];
	char tmp[100];
	info["recv"]["Name"] = "recv";
	info["recv"]["s"] = s;

	sprintf_s(hexNumber, "%08X", buf);
	info["recv"]["buf"] = string("0x") + string(hexNumber);

	if (buf != NULL && status > 0) {
		info["recv"]["bufValue"] = base64_encode((const unsigned char*)buf, status);
	}

	sprintf_s(hexNumber, "%08X", len);
	info["recv"]["len"] = string("0x") + string(hexNumber);

	sprintf_s(hexNumber, "%08X", flags);
	info["recv"]["flags"] = string("0x") + string(hexNumber);

	info["recv"]["return"] = status;
	
	if (status == SOCKET_ERROR) {
		info["recv"]["err"] = WSAGetLastError();
	}
	else {
		info["recv"]["err"] = "NULL";
	}
	
	sendApiInformation(info);
	return status;
}


/* send
*
| s     | SOCKET，一个已建立连接的套接字标识符。                       |
| buf   | char *，包含将要发送数据的指针。                             |
| len   | int，buf指向参数的大小，单位为字节                           |
| flags | int，指定发送的方式，一般为0。可用值如下：<br/>MSG_DONTROUTE 指明数据不选径。一个WINDOWS套接口供应商可以忽略此标志；<br/>MSG_OOB 发送带外数据（仅适用于SO_STREAM；）。 |
*/
static int(WSAAPI* Oldsend)(SOCKET s, const char* buf, int len, int flags) = send;
extern "C" __declspec(dllexport) int WSAAPI Newsend(SOCKET s, const char* buf, int len, int flags) {
	//auto status = Oldsend(s, buf, len, flags);
	auto status = 0;
	
	json info;
	char hexNumber[20];
	info["send"]["Name"] = "send";
	info["send"]["s"] = s;

	sprintf_s(hexNumber, "%08X", buf);
	info["send"]["buf"] = string("0x") + string(hexNumber);

	if (buf != NULL && status > 0) {
		info["send"]["bufValue"] = base64_encode((const unsigned char*)buf, status);
	}
	sprintf_s(hexNumber, "%08X", len);
	info["send"]["len"] = string("0x") + string(hexNumber);
	
	sprintf_s(hexNumber, "%08X", flags);
	info["send"]["flags"] = string("0x") + string(hexNumber);
	
	info["send"]["return"] = status;

	if (status == SOCKET_ERROR) {
		info["send"]["err"] = WSAGetLastError();
	}
	else {
		info["send"]["err"] = "NULL";
	}
	sendApiInformation(info);
	return status;
}


/* closesocket
* 函数作用：关闭一个已经存在的套接字。
* 函数返回值：int，成功返回0。失败，返回 SOCKET_ERROR 。
| sSOCKET，将要关闭的套接字描述符。
*/
static int(WSAAPI* Oldclosesocket)(SOCKET s) = closesocket;
extern "C" __declspec(dllexport) int WSAAPI Newclosesocket(SOCKET s) {
	auto result = Oldclosesocket(s); // old return value
	//auto result = 0;
	json info;
	info["closesocket"]["Name"] = "closesocket";

	char hexNumber[20];
	sprintf_s(hexNumber, "%08X", s);
	info["closesocket"]["s"] = string("0x") + string(hexNumber);

	if (result == SOCKET_ERROR)
	{
		info["closesocket"]["err"] = WSAGetLastError();
	}
	else {
		info["closesocket"]["err"] = "NULL";
	}

	info["closesocket"]["return"] = result;
	sendApiInformation(info);
	return result;
}


/* WSACleanup
* 函数作用：结束Ws2_32.dll的使用，释放相关资源。
* 函数返回值：类型：int，成功，返回0。失败，返回SOCKET_ERROR。
* 函数参数列表：无参数
*/
static int(WSAAPI* OldWSACleanup)() = WSACleanup;
extern "C" __declspec(dllexport) int WSAAPI NewWSACleanup() {
	//auto status = OldWSACleanup();
	auto status = 0;
	json info;
	info["WSACleanup"]["Name"] = "WSACleanup";
	info["WSACleanup"]["return"] = status;
	if (status == SOCKET_ERROR) {
		info["WSACleanup"]["err"] = WSAGetLastError();
	}
	else {
		info["WSACleanup"]["err"] = "NULL";
	}
	sendApiInformation(info);
	return status;
}






/* 动态链接库核心函数 DllMain */
BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		attachAllDetours();
		
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		detachAllDetours();
		//sendApiInformation();//发送http报文给本地服务器
		break;
	}
	}
	return true;
}

void attachAllDetours() {
	/* MessageBox */
	DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
	DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);

	/* Heap */
	DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
	DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
	DetourAttach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
	DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);

	/* File */
	DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
    DetourAttach(&(PVOID&)OldOpenFile, NewOpenFile);
    DetourAttach(&(PVOID&)OldReadFile, NewReadFile);
    DetourAttach(&(PVOID&)OldWriteFile, NewWriteFile);

	/* Registry */
	DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);	//创建
	DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);	//删除
	DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);		//打开
	DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);			//关闭
	DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);		//写

	/* Socket */
	//DetourAttach(&(PVOID&)OldWSAStartup, NewWSAStartup);
	DetourAttach(&(PVOID&)Oldsocket, Newsocket);
	DetourAttach(&(PVOID&)Oldconnect, Newconnect);
	DetourAttach(&(PVOID&)Oldrecv, Newrecv);
	DetourAttach(&(PVOID&)Oldsend, Newsend);
	DetourAttach(&(PVOID&)Oldclosesocket, Newclosesocket);
	//DetourAttach(&(PVOID&)OldWSACleanup, NewWSACleanup);

	DetourTransactionCommit();
}

void detachAllDetours() {
	/* MessageBox */
	DetourDetach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
	DetourDetach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);

	/* Heap */
	DetourDetach(&(PVOID&)OldHeapCreate, NewHeapCreate);
	DetourDetach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
	DetourDetach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
	DetourDetach(&(PVOID&)OldHeapFree, NewHeapFree);

	/* File */
	DetourDetach(&(PVOID&)OldCreateFile, NewCreateFile);
	DetourDetach(&(PVOID&)OldOpenFile, NewOpenFile);
	DetourDetach(&(PVOID&)OldReadFile, NewReadFile);
	DetourDetach(&(PVOID&)OldWriteFile, NewWriteFile);

	/* Registry */
	DetourDetach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);	//创建
	DetourDetach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);	//删除
	DetourDetach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);		//打开
	DetourDetach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);			//关闭
	DetourDetach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);		//写

	/* Socket */
	DetourDetach(&(PVOID&)Oldsocket, Newsocket);
	//DetourDetach(&(PVOID&)OldWSAStartup, NewWSAStartup);
	DetourDetach(&(PVOID&)Oldconnect, Newconnect);
	DetourDetach(&(PVOID&)Oldrecv, Newrecv);
	DetourDetach(&(PVOID&)Oldsend, Newsend);
	DetourDetach(&(PVOID&)Oldclosesocket, Newclosesocket);
	//DetourDetach(&(PVOID&)OldWSACleanup, NewWSACleanup);

	DetourTransactionCommit();
}

int sendApiInformation(json& info) {
	//取消hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	detachAllDetours();

	CURL* curl; // handler
	CURLcode code = CURLE_FAILED_INIT;
	//std::string url = "127.0.0.1:9999/uploadApiInformation";
	std::string url = "10.12.173.133:9999/uploadApiInformation";
	std::string dump = info.dump();
	std::cout << dump << endl;
	std::cout << "Why u bully me?" << endl;

	curl = curl_easy_init(); // initialize handler

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L); // POST
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dump.c_str());
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
//
		code = curl_easy_perform(curl);

		if (code != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(code));
			return code;
		}
		else {
			cout << "ApiInformation send successfully!" << endl;
		}
		curl_easy_cleanup(curl);
	}

	//重新 Hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	attachAllDetours();

	return code;
}

