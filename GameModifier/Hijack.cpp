// Hijack.cpp : ʵ���ļ�
//
#include "Hijack.h"


CHAR *g_pszHeader = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// ͷ�ļ�\r\n"
"#include <windows.h>\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n";


CHAR *g_pszHeader2 = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// �궨��\r\n"
"#define NAKED __declspec(naked)\r\n"
"#define EXPORT __declspec(dllexport)\r\n"
"#define MYEXPORTFUN EXPORT NAKED\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//ȫ�ֱ���\r\n"
"HMODULE hDll = NULL;\r\n"
"DWORD dwRetaddress[%d];							//��ŷ��ص�ַ\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// �ڲ����� ��ȡ��ʵ������ַ\r\n"
"FARPROC WINAPI GetAddress(PCSTR pszProcName)\r\n"
"{\r\n"
"\t\tFARPROC fpAddress;\r\n"
"\t\tCHAR szTemp[MAX_PATH] = { 0 };\r\n"
"\t\tfpAddress = GetProcAddress(hDll, pszProcName);\r\n"
"\t\tif (fpAddress == NULL)\r\n"
"\t\t{\r\n"
"\t\t\t\twsprintf(szTemp,\"�޷��ҵ����� :%s �ĵ�ַ \", pszProcName);\r\n"
"\t\t\t\tMessageBoxA(NULL, szTemp, \"����\", MB_OK);\r\n"
"\t\t\t\tExitProcess(-2);\r\n"
"\t\t}\r\n"
"\t\t//������ʵ��ַ\r\n"
"\t\treturn fpAddress;\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// DLL MAIN\r\n"
"BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)\r\n"
"{\r\n"
"\t\tswitch (fdwReason)\r\n"
"\t\t{\r\n"
"\t\tcase DLL_PROCESS_ATTACH:\r\n"
"\t\t\t\thDll = LoadLibrary(\"�޸�ΪҪ�ͽٳֵ�DLL\");\r\n"
"\t\t\t\tif (!hDll)\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tMessageBox(\"��Ҫ�ٳֵ�DLL�����ڣ�ע�����\");\r\n"
"\t\t\t\t\treturn FALSE;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t\tMessageBox(\"���ӳɹ�\");\r\n"
"\t\t\tbreak;\r\n"
"\t\tcase DLL_PROCESS_DETACH:\r\n"
"\t\t\t\tif (hDll != NULL)\r\n"
"\t\t\t\t\tFreeLibrary(hDll); \r\n"
"\t\t\tbreak; \r\n"
"\t\t}\r\n"
"\t\treturn TRUE;\r\n"

"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n";


CHAR *g_pszFun = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//�������� %d  ��Ҫ����ı����붨��Ϊȫ�֣�������ж�ջƽ������\r\n"
"MYEXPORTFUN My_%s()\r\n"
"{\r\n"
"\t\tGetAddress(\"%s\");\r\n"
"\t\t//��ʱջ�����ֵ��Ƿ��ص�ַ\r\n"
"\t\t__asm pop dwRetaddress[%d]				//����ԭ������ַ\r\n"
"\t\t//����ԭ����\r\n"
"\t\t__asm call eax\r\n"
"\t\t__asm jmp dword ptr dwRetaddress[%d]			//����ԭ����\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n";
// Hijack �Ի���





// Hijack ��Ϣ�������
CHAR * g_pszMyCode = "D:\\MyDll.c";
void Hijack::StartHijack()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	HANDLE hCFile = NULL;
	DWORD dwWritten = 0;
	CHAR szTemp[10000] = { 0 };

	hCFile = CreateFileA(g_pszMyCode, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hCFile == INVALID_HANDLE_VALUE)
	{
		TipBox("�����ļ�ʧ��");
		return;
	}

	//д��ͷ��
	if (!WriteFile(hCFile, g_pszHeader, strlen(g_pszHeader), &dwWritten, NULL))
	{
		TipBox("д���ļ�ʧ��");
		CloseHandle(hCFile);
		return;
	}

	//д��#pragma comment����
	// �������Ԫ��
	std::list<std::string>::iterator iter = m_FunNameList.begin();
	DWORD dwIndex = 0;
	for (; iter != m_FunNameList.end();iter++)
	{
		ZeroMemory(szTemp, 10000);
		std::string str = *iter;
		sprintf(szTemp, "#pragma comment(linker, \"/EXPORT:%s=_My_%s,@%d\")\r\n", str.c_str(), str.c_str(), dwIndex);
		dwIndex++;

		//д��#pragma comment����
		if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
		{
			TipBox("д���ļ�ʧ��");
			CloseHandle(hCFile);
			return;
		}
	}


	ZeroMemory(szTemp, 10000);
	sprintf(szTemp, g_pszHeader2, dwIndex, "%s");
	dwWritten = 0;
	//����д��
	if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
	{
		TipBox("д���ļ�ʧ��");
		CloseHandle(hCFile);
		return;
	}

	DWORD dwCount = 1;
	iter = m_FunNameList.begin();
	for (; iter != m_FunNameList.end();iter++)
	{
		std::string str = *iter;
		ZeroMemory(szTemp, 10000);
		sprintf(szTemp, g_pszFun, dwCount, str.c_str(), str.c_str(), dwCount, dwCount);


		//д��#pragma comment����
		if (!WriteFile(hCFile, szTemp, strlen(g_pszFun), &dwWritten, NULL))
		{
			TipBox("д���ļ�ʧ��");
			CloseHandle(hCFile);
			return;
		}
		dwCount++;
	}

	CloseHandle(hCFile);
	ZeroMemory(szTemp, 10000);
	sprintf(szTemp, "Դ�����ɳɹ�������·��Ϊ%s", g_pszMyCode);
	TipBox(szTemp);
}


void Hijack::OnDropFiles(HDROP hDropInfo)
{

	LoadFileR((LPTSTR)m_strInjectFile.c_str(), m_pstMapFile);
	if (IsPEFile(m_pstMapFile->ImageBase) == FALSE)
	{
		UnLoadFile(m_pstMapFile);
		TipBox("������Ч��PE�ļ�");
		return;
	}

	GetAndShowExports();

}

/*******************************************************
*��������:��DLL�л�ȡ�����õ������
*��������:��
*��������:��
*ע������:��
*����޸�ʱ��:2017/5/16
*******************************************************/
BOOL Hijack::GetAndShowExports()
{
	CHAR         cBuff[10];
	CHAR		 *szFuncName = NULL;
	UINT                    iNumOfName = 0;
	PDWORD                  pdwRvas, pdwNames;
	PWORD                   pwOrds;
	UINT                    i = 0, j = 0, k = 0;
	BOOL                    bIsByName = FALSE;;


	PIMAGE_NT_HEADERS       pNtH = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;

	pNtH = GetNtHeaders(m_pstMapFile->ImageBase);
	if (!pNtH)
		return FALSE;
	pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetExportDirectory(m_pstMapFile->ImageBase);
	if (!pExportDir)
		return FALSE;


	pwOrds = (PWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfNameOrdinals);
	pdwRvas = (PDWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfFunctions);
	pdwNames = (PDWORD)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pExportDir->AddressOfNames);

	if (!pdwRvas)
		return FALSE;


	iNumOfName = pExportDir->NumberOfNames;

	for (i = 0; i<pExportDir->NumberOfFunctions; i++)
	{
		if (*pdwRvas)
		{
			for (j = 0; j<iNumOfName; j++)
			{
				if (i == pwOrds[j])
				{
					bIsByName = TRUE;
					szFuncName = (char*)MyRvaToPtr(pNtH, m_pstMapFile->ImageBase, pdwNames[j]);
					break;
				}

				bIsByName = FALSE;
			}

			//show funcs to listctrl


			wsprintfA(cBuff, "%04lX", (UINT)(pExportDir->Base + i));

			//m_mylist.InsertItem(k, cBuff);

			wsprintfA(cBuff, "%08lX", (*pdwRvas));
			//m_mylist.SetItemText(k, 1, cBuff);

			if (bIsByName)
			{
				//m_FunNameList.AddTail(szFuncName);
			//	m_mylist.SetItemText(k, 2, szFuncName);
				//szFuncName == NULL;
			}
			else
			{
			//	m_mylist.SetItemText(k, 2, "-");
			}

			//
			++k;

		}

		++pdwRvas;

	}
	return TRUE;
}

