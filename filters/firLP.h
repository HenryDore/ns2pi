/******************************* SOURCE LICENSE *********************************
Copyright (c) 2019 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

// Begin header file, firLP.h

#ifndef FIRLP_H_ // Include guards
#define FIRLP_H_

static const int firLP_length = 24;
extern float firLP_coefficients[24];

typedef struct
{
	float* pointer;
	float state[48];
	float output;
} firLPType;


firLPType* firLP_create(void);
void firLP_destroy(firLPType* pObject);
void firLP_init(firLPType* pThis);
void firLP_reset(firLPType* pThis);
#define firLP_writeInput( pThis, input )  \
	firLP_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define firLP_readOutput( pThis )  \
	(pThis)->output

int firLP_filterBlock(firLPType* pThis, float* pInput, float* pOutput, unsigned int count);
#define firLP_outputToFloat( output )  \
	(output)

#define firLP_inputFromFloat( input )  \
	(input)

void firLP_dotProduct(float* pInput, float* pKernel, float* pAccumulator, short count);
#endif // FIRLP_H_

