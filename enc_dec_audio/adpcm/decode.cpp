#include <stdio.h>
#include <string.h>
#include "adpcm.h"

int main(int argc, char *argv[])
{
	FILE *pInFile = fopen(argv[1], "rb");
	FILE *pOutFile = fopen("decode_out.pcm", "wb");
	if (NULL == pInFile || NULL == pOutFile)
	{
		printf("open file failed\n");
		return 0;
	}
	
	//int iRet = 0;
	int iRead = 0;
	adpcm_state state;
	
	unsigned char ucInBuff[640] = {0};
	unsigned char ucOutBuff[1024] = {0};
	while (1)
	{
		iRead = fread(ucInBuff, 1, 40, pInFile);
		if (iRead > 0)
		{
			adpcm_decoder((char*)ucInBuff, (short*)ucOutBuff, 80, &state);
			printf("decode %d bytes\n", iRead);
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
