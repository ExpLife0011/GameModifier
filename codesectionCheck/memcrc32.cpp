/*******************************************************
 /*�����������(������)������ʵ��
 /*    14.3.3 �ڴ�ӳ��У��
 /*   memcrc32.cpp -- �ڴ�ӳ��У�飬��У���������
/*    code by kanxue
/*(c)  ��ѩ�����ȫ��վ www.pediy.com 2000-2008
********************************************************/
#include <windows.h>

DWORD CRC32(BYTE*,DWORD);
BOOL CodeSectionCRC32( );
	


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	HMODULE hmodule  = LoadLibraryA("Test.dll");
	if(CodeSectionCRC32())
		MessageBox(NULL,TEXT ("CRC32 Check OK!"),TEXT ("OK"),MB_ICONEXCLAMATION);
	else
    		MessageBox(NULL,"File corrupted! !","CRC error",MB_ICONEXCLAMATION);
	return 0;
}

////////////////////////////////////////////////////////////////
// ������������CRC32ֵ
//

BOOL CodeSectionCRC32( )
{

	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
    PIMAGE_NT_HEADERS       pNtHeader=NULL;
    PIMAGE_SECTION_HEADER   pSecHeader=NULL;

	DWORD					ImageBase,OriginalCRC32;

    ImageBase=(DWORD)GetModuleHandle("Test.dll");//ȡ��ַ����ʵ����Ҳ��ֱ����0x4000000���ֵ
	//ImageBase=(DWORD)GetModuleHandle(NULL);//ȡ��ַ����ʵ����Ҳ��ֱ����0x4000000���ֵ

	 pDosHeader=(PIMAGE_DOS_HEADER)ImageBase;	 	 
     pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
	 //��λ��PE�ļ�ͷ�����ִ���PE\0\0������ǰ4���ֽڴ��������������������CRC-32ֵ��
	 OriginalCRC32 =*((DWORD *)((DWORD)pNtHeader-4));

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
	 //�����һ��������Ǵ�������
	 //�ǴӴ����ļ��õ������ĵ�ַ�ʹ�С������CRC32ֵ
	 if(OriginalCRC32==CRC32((BYTE*) (ImageBase+pSecHeader->VirtualAddress),pSecHeader->Misc.VirtualSize)) 
		return TRUE;
	else
		return FALSE;


	//�������Ӵ����ļ���ȡ��һ��������ݣ��ӿǺ����ݽ����ܲ�׼ȷ����˶���Ҫ�ӿǵĳ�����˵��
	//ֱ�����ϴ�������ĵ�ַ�ʹ�С���������������һ���ݣ��������PE���߲鿴�����ļ��Ĵ��������RVA�ʹ�С��
	//����������±���
	/*
	if(OriginalCRC32==CRC32((BYTE*) 0x401000,0x36AE)) 
		return TRUE;
	else
		return FALSE;
*/

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
