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

// Begin header file, firComb.h

#ifndef FIRCOMB_H_ // Include guards
#define FIRCOMB_H_

static const int firComb_length = 10;

typedef struct
{
	float* pointer;
	float state[10];
	float output;
} firCombType;


firCombType* firComb_create(void);
void firComb_destroy(firCombType* pObject);
void firComb_init(firCombType* pThis);
void firComb_reset(firCombType* pThis);
#define firComb_writeInput( pThis, input )  \
	firComb_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define firComb_readOutput( pThis )  \
	(pThis)->output

int firComb_filterBlock(firCombType* pThis, float* pInput, float* pOutput, unsigned int count);
#define firComb_outputToFloat( output )  \
	(output)

#define firComb_inputFromFloat( input )  \
	(input)

#endif // FIRCOMB_H_

