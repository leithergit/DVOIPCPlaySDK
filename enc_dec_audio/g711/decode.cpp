#include <stdio.h>
#include <string.h>
#include "g711.h"

int main(int argc, char *argv[])
{
	FILE *pInFile = fopen(argv[1], "rb");
	FILE *pOutFile = fopen("711a_out.pcm", "wb");
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
		iRead = fread(ucInBuff, 1, 80, pInFile);
		if (iRead > 0)
		{
			iRet = g711a_decode((short*)ucOutBuff, ucInBuff, 80);
			printf("iRet = %d\n", iRet);
			fwrite(ucOutBuff, 1, 160, pOutFile);
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
	
	return 0;
	
}
