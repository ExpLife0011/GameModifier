/*******************************************************
 /*�����������(������)������ʵ��
 /*    14.3.3 �ڴ�ӳ��У��
 /*  add2crc32.cpp -- �����ļ���CRC32ֵ
/*    code by kanxue
/*(c)  ��ѩ�����ȫ��վ www.pediy.com 2000-2008
********************************************************/
#include <windows.h>

int patch();
DWORD CRC32(BYTE*,DWORD);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	
	patch();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////



int patch()
{
	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
    PIMAGE_NT_HEADERS       pNtHeader=NULL;
    PIMAGE_SECTION_HEADER   pSecHeader=NULL;
	
	TCHAR szFileName[MAX_PATH ] ;
	DWORD szTemp;
	static OPENFILENAME ofn ;
	HANDLE hFile;	
	long FileSize;
	DWORD szCRC32;
	TCHAR  *pBuffer ;

	ZeroMemory(&ofn, sizeof(ofn));                             // ��ʼ��OPENFILENAME�ṹ

	static TCHAR szFilter[] =TEXT ("EXE Files (*.exe)\0*.exe\0") \
		TEXT ("All Files (*.*)\0*.*\0\0") ;
	
	szFileName[0] = '\0';                         
	ZeroMemory(&ofn, sizeof(ofn));                             // ��ʼ��OPENFILENAME�ṹ
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrFile         = szFileName ;         
	ofn.nMaxFile          = MAX_PATH ;   
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;           
	
	if(GetOpenFileName (&ofn))
	{
		
		hFile = CreateFile(
			szFileName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		
		
		if( hFile != INVALID_HANDLE_VALUE )
		{
			FileSize=GetFileSize(hFile,&szTemp);  // ��ȡ�ļ���С
			if (FileSize == 0xFFFFFFFF) return FALSE;
			pBuffer = new TCHAR [FileSize];     // �����ڴ�
			if(ReadFile(hFile, pBuffer, FileSize, &szTemp, NULL)==NULL) return FALSE;// ��ȡ����
			
		}
		else
		{
			MessageBox(NULL,"I can't access file!","CRC error",MB_ICONEXCLAMATION);
			return 0;
		}
		
	}
	else
		return 0;
	
	
	pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;	 	 
	pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
	pSecHeader=IMAGE_FIRST_SECTION(pNtHeader);   //�õ���һ���������ʼ��ַ  
	IMAGE_FILE_HEADER* pFileHeader =	&pNtHeader->FileHeader;
	for (int i = 0; i < pFileHeader->NumberOfSections;i++)
	{
		if((strcmp((char*)pSecHeader->Name,".text") == 0))
		{
			break;
		}
		pSecHeader++;
	}


	szCRC32=CRC32((BYTE*)(pBuffer+pSecHeader->PointerToRawData),pSecHeader->Misc.VirtualSize);
	
	DWORD Writeadd=DWORD(pDosHeader->e_lfanew-4);//��λ��PE�ļ�ͷ�����ִ���PE\0\0������ǰ4���ֽڴ���׼������дCRC-32ֵ��
	SetFilePointer(hFile,Writeadd,NULL,FILE_BEGIN);//�����ļ�ָ��
	
	if(!WriteFile(hFile,&szCRC32,4,&szTemp,NULL))
	{
		MessageBox(NULL,"Error while patching !","Patch aborted",MB_ICONEXCLAMATION);
		CloseHandle(hFile);
		return 0;
	}
	
	
	delete pBuffer;  // �ͷ��ڴ�
	CloseHandle(hFile);	
	MessageBox(NULL,"Patch successfull !","Patch",MB_ICONINFORMATION);
	return 1;
	
}


////////////////////////////////////////////////////////////////
// �����ַ�����CRC32ֵ
// ������������CRC32ֵ�ַ������׵�ַ�ʹ�С
// ����ֵ: ����CRC32ֵ

DWORD CRC32(BYTE* ptr,DWORD Size)
{
	
	DWORD crcTable[256],crcTmp1;
	
	//��̬����CRC-32��
	for (int i=0; i<256; i++)
	{
		crcTmp1 = i;
		for (int j=8; j>0; j--)
		{
			if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
			else crcTmp1 >>= 1;
		}
		
		crcTable[i] = crcTmp1;
	}
	//����CRC32ֵ
	DWORD crcTmp2= 0xFFFFFFFF;
	while(Size--)
	{
		crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
		ptr++;
	}
	
	return (crcTmp2^0xFFFFFFFF);
}