#include "DebuggerDetector.h"
#include <windows.h>
#include <stdio.h>

using namespace std;

#define NTAPI              __stdcall
typedef long              NTSTATUS;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_SUCCESS    ((NTSTATUS)0L)

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION
{
	BOOLEAN DebuggerEnabled;
	BOOLEAN DebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

typedef struct _PROCESS_DEBUG_PORT_INFO
{
	HANDLE DebugPort;
}    PROCESS_DEBUG_PORT_INFO;

enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 };
enum THREAD_INFO_CLASS        { ThreadHideFromDebugger          = 17 };
enum PROCESS_INFO_CLASS      { ProcessDebugPort                = 7  };

typedef NTSTATUS  (NTAPI *ZW_QUERY_SYSTEM_INFORMATION)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength);
typedef NTSTATUS  (NTAPI *ZW_SET_INFORMATION_THREAD)(IN HANDLE ThreadHandle, IN THREAD_INFO_CLASS ThreadInformationClass, IN PVOID ThreadInformation, IN ULONG ThreadInformationLength);
typedef NTSTATUS  (NTAPI *ZW_QUERY_INFORMATION_PROCESS)(IN HANDLE ProcessHandle, IN PROCESS_INFO_CLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength); 
DebuggerDetector::DebuggerDetector()
{
}

DebuggerDetector::~DebuggerDetector()
{
}



bool DebuggerDetector::NGuard_IsDebuggerPresent()
{
	//check softbreak
	//if(*(BYTE*)Func_addr==0xcc)
	//	return true;
	//check hook
	//if(*(BYTE*)Func_addr!=0x64)
	//	return true;

	return IsDebuggerPresent();
}

bool DebuggerDetector::NGuard_CheckByOutputDebugString()
{
	OutputDebugString(L"Test");
	DWORD tmp=GetLastError();
	if(tmp==0)
		return true;
	return false;
}

bool DebuggerDetector::NGuard_NtQueryInfoProc_DbgPort()
{
	HMODULE hModule = GetModuleHandleA("ntdll.dll"); 
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess"); 
	if (ZwQueryInformationProcess == NULL) 
		return false;
	PROCESS_DEBUG_PORT_INFO ProcessInfo; 
	if (STATUS_SUCCESS != ZwQueryInformationProcess(GetCurrentProcess( ), ProcessDebugPort, &ProcessInfo, sizeof(ProcessInfo), NULL)) 
		return false;
	else 
		if(ProcessInfo.DebugPort)
			return true;
		else
			return false;
}

bool DebuggerDetector::NGuard_NtQueryInfoProc_SYSTEM_INFORMATION()
{
	//��������Լ��ϵͳ�������Ƿ���ڣ���SoftICE�ƺ����ã�����ֻ��΢���Լҵ�WinDBG��Ч���������Ŀɲ���һ�¡� 
	HMODULE hModule = GetModuleHandleA("ntdll.dll"); 
//------------------------------------------------------------------------------------
	ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation;
	ZwQuerySystemInformation = (ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation");
	if (ZwQuerySystemInformation == NULL)
	{
		printf("Failed: GetProcAddress ZwQuerySystemInformation\n");
		return false;
	}

	SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;
	if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL))
	{
		if (Info.DebuggerEnabled)
		{
			printf("System debugger enabled\n");
			if (Info.DebuggerNotPresent)
			{
				printf("System debugger not present\n");;
			}
			else
			{
				printf("System debugger present\n");
			}
			return true;

		}
		else
			printf("System debugger disabled\n");
			return false;
	}
	else
	{
		printf("Failed: ZwQuerySystemInformation\n");
		return false;
	}
}

bool DebuggerDetector::NGuard_NtQueryInfoProc_SET_INFORMATION_THREAD()
{
	//������������Խ�ĳ���̵߳ĵ��Զ˿���Ϊ0��ʹ��Win32�������޷����յ����̵߳ĵ����¼���ʹ�������޷��ٵ��Ը��̡߳������Ҫ�����VC++������ring3�������ġ� 
	//---------------------------------------------------------------------------------------
		HMODULE hModule = GetModuleHandleA("ntdll.dll"); 
	ZW_SET_INFORMATION_THREAD ZwSetInformationThread;
	ZwSetInformationThread = (ZW_SET_INFORMATION_THREAD)GetProcAddress(hModule, "ZwSetInformationThread");
	if (ZwSetInformationThread == NULL)
	{
		printf("Failed: GetProcAddress ZwSetInformationThread\n");
		return false;
	}

	if (STATUS_SUCCESS != ZwSetInformationThread(GetCurrentThread( ), ThreadHideFromDebugger, NULL, 0))
	{
		printf("Failed: ZwSetInformationThread\n");
	}
	return true;
}

bool DebuggerDetector::NGuard_NtQueryInfoProc_ZW_QUERY_INFORMATION_PROCESS()
{
	//������Լ��ĳ�������Ƿ�����ring3�����������ԡ� 
	HMODULE hModule = GetModuleHandleA("ntdll.dll"); 
	//---------------------------------------------------------------------------------------
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess;
	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess");
	if (ZwQueryInformationProcess == NULL)
	{
		printf("Failed: GetProcAddress ZwQueryInformationprocess\n");
		return false;
	}

	PROCESS_DEBUG_PORT_INFO ProcessInfo;
	if (STATUS_SUCCESS != ZwQueryInformationProcess(GetCurrentProcess( ), ProcessDebugPort, &ProcessInfo, sizeof(ProcessInfo), NULL))
	{
		printf("Failed: ZwQueryInformationProcess\n");
		return false;
	}
	else
	{
		if (ProcessInfo.DebugPort)
		{
			printf("Process debugger present\n" );
			return true;
		}
		else
		{
			printf("Process debugger not present\n");
			return false;
		}
	} 
}

bool DebuggerDetector::Detect()
{
	printf("IsDebug AD_IsDebuggerPresent %d\n" , NGuard_IsDebuggerPresent());
	printf(" AD_CheckByOutputDebugString %d\n" , NGuard_CheckByOutputDebugString());
	printf("FD_NtQueryInfoProc_DbgPort %d\n" , NGuard_NtQueryInfoProc_DbgPort());
	printf("FD_NtQueryInfoProc_SYSTEM_INFORMATION %d\n" , NGuard_NtQueryInfoProc_SYSTEM_INFORMATION());
	//printf("FD_NtQueryInfoProc_SET_INFORMATION_THREAD %d\n" , NGuard_NtQueryInfoProc_SET_INFORMATION_THREAD());
	printf("FD_NtQueryInfoProc_ZW_QUERY_INFORMATION_PROCESS %d\n" , NGuard_NtQueryInfoProc_ZW_QUERY_INFORMATION_PROCESS());
	return true;
}