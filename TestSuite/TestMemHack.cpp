#include "TestMemHack.h"
TestMemhack::TestMemhack()
{
	g_dwAddList = new DWORD[PAGE*PAGE];
	g_dwCount = 0;
	g_hProcess = 0;
	g_dwId = 0;
}

TestMemhack::~TestMemhack()
{
}

void TestMemhack::Test()
{
	UINT uIndex = 0;
	printf("start getProcessIDByName ! \n");
	GetProcessIDByName(L"GameGuard.exe");

	printf("start editValue :%d \n", g_dwId);
	editValue(g_dwId);

	printf("start closeProcess !\n");
	//closeProcess(g_dwId);
	system("pause");
	return ;
}
void TestMemhack::showMenu()
{
	system("cls");
	printf("1.�����б�\n");
	printf("2.�޸��ڴ�\n");
	printf("3.��������\n");
	printf("4.�˳�ϵͳ\n");
	printf("5.�õ���ַ\n");
	printf("������ѡ��");
}

DWORD TestMemhack::getProcessId()
{
	DWORD dwId;
	printf("���������ID:");
	scanf("%d", &dwId);
	g_dwId = dwId;
	return dwId;
}

#include <windows.h>
#include <stdint.h>
#include <memory>

#pragma comment(lib, "version.lib" )

bool TestMemhack::GetOSVersionString(VS_FIXEDFILEINFO *vInfo)
{
	WCHAR path[_MAX_PATH];
	if (!GetSystemDirectoryW(path, _MAX_PATH))
		return false;

	wcscat_s(path, L"\\kernel32.dll");

	//
	// Based on example code from this article
	// http://support.microsoft.com/kb/167597
	//

	DWORD handle;
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	DWORD len = GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, path, &handle);
#else
	DWORD len = GetFileVersionInfoSizeW(path, &handle);
#endif
	if (!len)
		return false;

	std::unique_ptr<uint8_t> buff(new (std::nothrow) uint8_t[len]);
	if (!buff)
		return false;

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	if (!GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, path, 0, len, buff.get()))
#else
	if (!GetFileVersionInfoW(path, 0, len, buff.get()))
#endif
		return false;


	UINT infoSize;

	if (!VerQueryValueW(buff.get(), L"\\", reinterpret_cast<LPVOID*>(&vInfo), &infoSize))
		return false;

	if (!infoSize)
		return false;

	wchar_t szVersion[MAX_PATH] = { 0 };
	swprintf_s(szVersion, MAX_PATH, L"%u.%u.%u.%u",
		HIWORD(vInfo->dwFileVersionMS),
		LOWORD(vInfo->dwFileVersionMS),
		HIWORD(vInfo->dwFileVersionLS),
		LOWORD(vInfo->dwFileVersionLS));
	printf("OS version: %s\n", Utils_WideChar_To_Utf8(szVersion).c_str());

	return true;
}
void TestMemhack::editValue(DWORD dwId)
{
	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);
	if (!g_hProcess)
	{
		printf("�򿪽���%sʧ��\n", dwId);
		return;
	}

	DWORD dwValue;
	printf("�������һ�ε�ֵ:");
	scanf("%d", &dwValue);

	FindFirst(dwValue);

	ShowAddList();

	printf("������ڶ��ε�ֵ��");
	scanf("%d", &dwValue);

	FindNext(dwValue);
	ShowAddList();

	printf("������Ҫ��ֵ:");
	scanf("%d", &dwValue);
	WriteMemory(dwValue);
}

BOOL TestMemhack::CompareAPage(DWORD dwBaseAddr, DWORD dwValue)
{
	BYTE bytes[PAGE];
	if (!ReadProcessMemory(g_hProcess, (LPCVOID)dwBaseAddr, bytes, PAGE, NULL))
	{
		//printf("��ȡ�ڴ�ʧ��\n");
		return FALSE;
	}

	DWORD *pdw = (DWORD*)bytes;

	for (int i = 0; i < KILO; i++)
	{
		if (pdw[i] == dwValue)
		{
			g_dwAddList[g_dwCount++] = dwBaseAddr + i*sizeof(DWORD);
		}
	}
	return TRUE;
}

BOOL TestMemhack::FindFirst(DWORD dwValue)
{

	VS_FIXEDFILEINFO versionInfo;
	if (!GetOSVersionString(&versionInfo))
	{
		return FALSE;
	}
	DWORD dwBase;

	//if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	//{
	//	printf("Windows 98\n");
	//	dwBase = 4 * KONEK * KONEK;
	//}
	////else if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	//{
	printf("Windows NT\n");
	dwBase = 64 * KILO;
	//}
	g_dwCount = 0;

	DWORD dwOld = 0;
	DWORD dwNew = 0;

	struct timeb start;
	struct timeb end;

	printf("������...\n%%%02d", 0.0);

	ftime(&start);

	dwBase = GetBaseAddress(g_dwId);
	//2G /4k =2 k* k*k*k 
	for (; dwBase <(DWORD)2 * MEGE; dwBase += PAGE)
	{
		dwNew = dwBase / (MEGE / 50);
		if (dwNew != dwOld)
		{
			printf("\b\b%02d", dwNew);
			dwOld = dwNew;
		}

		CompareAPage(dwBase, dwValue);
	}
	ftime(&end);
	printf("\b\b100\n�������\n");

	printf("��ʱ%d����\n", (end.time - start.time) * 1000 + end.millitm - start.millitm);

	return TRUE;
}

BOOL TestMemhack::FindNext(DWORD dwValue)
{
	DWORD dwCount = 0;
	DWORD dwValue1 = 0;

	for (unsigned int i = 0; i < g_dwCount; i++)
	{
		if (!ReadProcessMemory(g_hProcess, (LPCVOID)g_dwAddList[i], &dwValue1, sizeof(DWORD), NULL))
		{
			printf("��ȡ�ڴ�ʧ��\n");
			return FALSE;
		}
		if (dwValue1 == dwValue)
		{
			g_dwAddList[dwCount++] = g_dwAddList[i];
		}
	}
	g_dwCount = dwCount;

	return TRUE;
}

BOOL TestMemhack::WriteMemory(DWORD dwValue)
{
	for (unsigned int i = 0; i < g_dwCount; i++)
	{
		if (!WriteProcessMemory(g_hProcess, (LPVOID)g_dwAddList[i], (LPCVOID)&dwValue, sizeof(DWORD), NULL))
		{
			printf("Write process mem failed %#010x", g_dwAddList[i]);
			return FALSE;
		}

	}
	return TRUE;
}


void TestMemhack::ShowAddList(void)
{
	printf("��ַ�б�...\n");
	for (unsigned int i = 0; i < g_dwCount; i++)
	{
		printf("%#010x\n", g_dwAddList[i]);
	}
}

BOOL closeProcess(DWORD dwId)
{
	BOOL bRet = FALSE;
	HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);

	if (hHandle != NULL)
	{
		bRet = TerminateProcess(hHandle, 0);
	}
	CloseHandle(hHandle);
	return bRet;
}

DWORD TestMemhack::GetProcessIDByName(wchar_t * pName)
{
	PROCESSENTRY32 pc;
	pc.dwSize = sizeof(pc);

	HANDLE dProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == dProcessSnap)
	{
		printf("��ý���ʧ��");
		system("pause");
		return 0;
	}

	BOOL bMore = Process32First(dProcessSnap, &pc);

	while (bMore)
	{
		if (wcscmp(pc.szExeFile, pName) == 0)
		{
			g_dwId = pc.th32ProcessID;
			break;
		}
		bMore = Process32Next(dProcessSnap, &pc);
	}

	CloseHandle(dProcessSnap);
	return 0;
}

void TestMemhack::showAllProcess()
{
	PROCESSENTRY32 pc;
	pc.dwSize = sizeof(pc);

	HANDLE dProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == dProcessSnap)
	{
		printf("��ý���ʧ��");
		system("pause");
		return;
	}

	BOOL bMore = Process32First(dProcessSnap, &pc);

	while (bMore)
	{
		printf("����ID:%4d | �������ƣ�%s\n", pc.th32ProcessID, Utils_WideChar_To_Utf8(pc.szExeFile).c_str());
		bMore = Process32Next(dProcessSnap, &pc);
	}

	CloseHandle(dProcessSnap);

}

//////////////////////////////////////////////////////////////////////////
//   �������ܣ� ��ȡexeģ��ļ��ص�ַ
//   ��   ���� dwPID�����̵�pid�� 
//   �� �� ֵ�� ����exeģ���ַ��
//////////////////////////////////////////////////////////////////////////
DWORD TestMemhack::GetBaseAddress(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		printf("ʧ��!");
		return 0;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return 0;
	}
	DWORD Value = (DWORD)me32.modBaseAddr;
	CloseHandle(hModuleSnap);
	return Value;
}