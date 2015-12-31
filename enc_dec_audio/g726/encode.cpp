#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "g726.h"

int main(int argc, char *argv[])
{

	g726_state_t *g_state726_16 = NULL; //for g726_16 
	g726_state_t *g_state726_24 = NULL; //for g726_24 
	g726_state_t *g_state726_32 = NULL; //for g726_32 
	g726_state_t *g_state726_40 = NULL; //for g726_40 
	
	//g_state726_16 = (g726_state_t *)malloc(sizeof(g726_state_t));
	//g_state726_16 = g726_init(g_state726_16, 8000*2);
	
	g_state726_24 = (g726_state_t *)malloc(sizeof(g726_state_t));
	g_state726_24 = g726_init(g_state726_24, 8000*3);
	
	//g_state726_32 = (g726_state_t *)malloc(sizeof(g726_state_t));
	//g_state726_32 = g726_init(g_state726_32, 8000*4);
	
	//g_state726_40 = (g726_state_t *)malloc(sizeof(g726_state_t));
	//g_state726_40 = g726_init(g_state726_40, 8000*5);

	FILE *pInFile = fopen(argv[1], "rb");
	FILE *pOutFile = fopen("encode_out.g726", "wb");
	if (NULL == pInFile || NULL == pOutFile)
	{
		printf("open file failed\n");
		return 0;
	}
	
	int iRet = 0;
	int iRead = 0;

	
	unsigned char ucInBuff[640] = {0};
	unsigned char ucOutBuff[120] = {0};
	while (1)
	{
		iRead = fread(ucInBuff, 1, 160, pInFile);
		if (iRead > 0)
		{
			//iRet = g726_encode(g_state726_16, ucOutBuff, (short*)ucInBuff, 40);
			iRet = g726_encode(g_state726_24, ucOutBuff, (short*)ucInBuff, 80);
			//iRet = g726_encode(g_state726_32, ucOutBuff, (short*)ucInBuff, 40);
			//iRet = g726_encode(g_state726_40, ucOutBuff, (short*)ucInBuff, 80);
			printf("iRet = %d\n", iRet);
			fwrite(ucOutBuff, 1, iRet, pOutFile);
			memset(ucInBuff, 0, sizeof(ucInBuff));
			memset(ucOutBuff, 0, sizeof(ucOutBuff));
		}
		else
		{
			printf("read the end\n");
			break;
		}
	}
	
	fclose(pInFile);
	fclose(pOutFile);
	
}
