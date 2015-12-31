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
	FILE *pOutFile = fopen("decode_out.pcm", "wb");
	if (NULL == pInFile || NULL == pOutFile)
	{
		printf("open file failed\n");
		return 0;
	}
	
	int iRet = 0;
	int iRead = 0;

	
	unsigned char ucInBuff[640] = {0};
	unsigned char ucOutBuff[1024] = {0};
	while (1)
	{
		iRead = fread(ucInBuff, 1, 30, pInFile);
		if (iRead > 0)
		{
			//iRet = g726_decode(g_state726_16, (short*)ucOutBuff, ucInBuff, 20);
			iRet = g726_decode(g_state726_24, (short*)ucOutBuff, ucInBuff, 30);
			//iRet = g726_decode(g_state726_32, (short*)ucOutBuff, ucInBuff, 20);
			//iRet = g726_decode(g_state726_40, (short*)ucOutBuff, ucInBuff, 20);
			
			printf("iRet = %d\n", iRet);
			fwrite(ucOutBuff, sizeof(short), iRet, pOutFile);
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
