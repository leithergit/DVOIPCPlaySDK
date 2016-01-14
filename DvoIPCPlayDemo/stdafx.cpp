
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// DvoIPCPlayDemo.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



HANDLE g_hStdout = NULL;
HANDLE g_hStdin = NULL;

// ��Э�����ݰ�Э���ʽ��ȷ���������������
void _TraceProtocol(byte *szBuffer, int nBufferLength)
{
	if (nBufferLength < 32)
		return;
	MSG_HEAD *pHeader = (MSG_HEAD *)szBuffer;
	char szHex[1024] = { 0 };

	Hex2AscStringA((byte *)pHeader->Magic1, sizeof(pHeader->Magic1), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Magic1), szHex);

	Hex2AscStringA((byte *)&pHeader->Pktlen, sizeof(pHeader->Pktlen), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Pktlen), szHex);

	Hex2AscStringA((byte *)&pHeader->Version, sizeof(pHeader->Version), szHex, 1024, ' ');
	printf("%s\t= %s\n", Var2Str(pHeader->Version), szHex);

	Hex2AscStringA((byte *)&pHeader->Hwtype, sizeof(pHeader->Hwtype), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Hwtype), szHex);

	Hex2AscStringA((byte *)&pHeader->Sn, sizeof(pHeader->Sn), szHex, 1024, ' ');
	printf("%s\t\t\t= %s\n", Var2Str(pHeader->Sn), szHex);

	Hex2AscStringA((byte *)&pHeader->CmdType, sizeof(pHeader->CmdType), szHex, 1024, ' ');
	printf("%s\t= %s\n", Var2Str(pHeader->CmdType), szHex);

	Hex2AscStringA((byte *)&pHeader->CmdSubType, sizeof(pHeader->CmdSubType), szHex, 1024, ' ');
	printf("%s\t= %s\n", Var2Str(pHeader->CmdSubType), szHex);

	Hex2AscStringA((byte *)&pHeader->DataType, sizeof(pHeader->DataType), szHex, 1024, ' ');
	printf("%s\t= %s\n", Var2Str(pHeader->DataType), szHex);

	Hex2AscStringA((byte *)&pHeader->Rev1, sizeof(pHeader->Rev1), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Rev1), szHex);

	Hex2AscStringA((byte *)&pHeader->Rev2, sizeof(pHeader->Rev2), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Rev2), szHex);

	Hex2AscStringA((byte *)&pHeader->Rev3, sizeof(pHeader->Rev3), szHex, 1024, ' ');
	printf("%s\t\t= %s\n", Var2Str(pHeader->Rev3), szHex);
}