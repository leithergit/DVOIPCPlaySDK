#include <stdio.h>
#include <string.h>
#include "g711.h"

int main(int argc, char *argv[])
{
	FILE *pInFile = fopen(argv[1], "rb");
	FILE *pOutFile = fopen("encode_out.g711a", "wb");
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
			iRet = g711a_encode(ucOutBuff, (short*)ucInBuff, 80);
			printf("iRet = %d\n", iRet);
			fwrite(ucOutBuff, 1, 80, pOutFile);
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
