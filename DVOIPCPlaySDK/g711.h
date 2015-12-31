#include <stdio.h>
#include <string.h>

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */

#define	BIAS		(0x84)		/* Bias for linear code. */
#define CLIP            8159
#define AUDIO_BLOCK_SIZE 1024

short search(short val, short *table, short size);
unsigned char linear2alaw(short	pcm_val);
short alaw2linear(unsigned char	a_val);

unsigned char 	linear2ulaw(short pcm_val);
short 		ulaw2linear(unsigned char u_val);
unsigned char 	alaw2ulaw(unsigned char	aval);
unsigned char 	ulaw2alaw(unsigned char	uval);

/// @brief G711-ulaw“Ù∆µΩ‚¬Î
int G711UDecode(OUT char* pWaveData,IN const unsigned char* pBuffer,IN int nBufferSize);
/// @brief G711-alaw“Ù∆µΩ‚¬Î
int G711ADecode(OUT char* pWaveData, IN const unsigned char* pBuffer, IN int nBufferSize);