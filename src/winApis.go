package main

type GeneralAttributes struct {
	name 		string
	retValue 	string
	err			string
}

/* MessageBox */
type ApiMessageBoxA struct {
	GeneralAttributes
}

/* File Handling */
type ApiCreateFileA struct {
	GeneralAttributes

}

type ApiWriteFile struct {
	GeneralAttributes
}

type ApiReadFile struct {
	GeneralAttributes

}

type ApiCloseHandle struct {
	GeneralAttributes

}

/* Registry */
type ApiCreateKeyExA struct {
	GeneralAttributes
}

type ApiRegOpenKeyEx struct {
	GeneralAttributes
}

type ApiRegSetValueEx struct {
	GeneralAttributes
}

type ApiRegCloseKeyEx struct {
	GeneralAttributes
}

type ApiRegDeleteValue struct {
	GeneralAttributes
}

/* Heap */
type ApiHeapCreate struct {
	GeneralAttributes
}

type ApiHeapAlloc struct {
	GeneralAttributes
}

type ApiHeapDestroy struct {
	GeneralAttributes
}

type ApiHeapFree struct {
	GeneralAttributes
}

/* Socket : WinSock2.h */
type ApiSocket struct {
	// 初始化socket
	GeneralAttributes
}

type ApiWSAStartup struct {
	//初始化Ws2_32.dll
	GeneralAttributes
}

type ApiConnect struct {
	// 连接
	GeneralAttributes
}

type ApiSend struct {
	// 发送报文
	GeneralAttributes
}

type ApiRecv struct {
	// 接收报文
	GeneralAttributes
}

type ApiCloseSocket struct {
	//关闭socket
	GeneralAttributes
}

type ApiWSACleanup struct {
	//中断动态链接库Ws2_32
	GeneralAttributes
}
