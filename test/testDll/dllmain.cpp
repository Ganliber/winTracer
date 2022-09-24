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

//static size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userp) {
//	return 0;
//}

int sendApiInformation(json &info) {
	CURL* curl; // handler
	CURLcode code = CURLE_FAILED_INIT;

	std::string url = "127.0.0.1:9999/uploadApiInformation";
	std::string dump = info.dump();
	std::cout << dump << endl;
//
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
	return code;
}



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
	std::cout << "hWnd : " << hWnd << "\thdl : " << hdl << endl;
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
)
{
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
		info["CreateFile"]["err"] = NULL;
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
		info["CloseHandle"]["err"] = NULL;
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
		info["ReadFile"]["err"] = NULL;
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

	//if (lpNumberOfBytesWritten != NULL) {
	//	info["WriteFile"]["lpNumberOfBytesWrittenValue"] = *lpNumberOfBytesWritten;
	//	info["WriteFile"]["lpBufferValue"] = base64_encode((const unsigned char*)lpBuffer, (unsigned int)*lpNumberOfBytesWritten);
	//}
	//else {
	//	info["WriteFile"]["lpNumberOfBytesWrittenValue"] = 0;
	//	info["WriteFile"]["lpBufferValue"] = NULL;
	//}

	info["WriteFile"]["lpOverlapped"] = int(lpOverlapped);
	info["WriteFile"]["return"] = result;
	if (result == FALSE) {
		info["WriteFile"]["err"] = GetLastError();
	}
	else {
		info["WriteFile"]["err"] = NULL;
	}

	sendApiInformation(info);
	attachAllDetours();
	return result;
}









/* Regedit */






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
		info["HeapFree"]["err"] = "Error : Double Free -> Terminated!";
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

	DetourTransactionCommit();
}