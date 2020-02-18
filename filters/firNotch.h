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

// Begin header file, firNotch.h

#ifndef FIRNOTCH_H_ // Include guards
#define FIRNOTCH_H_

static const int firNotch_length = 8;
extern float firNotch_coefficients[8];

typedef struct
{
	float * pointer;
	float state[16];
	float output;
} firNotchType;


firNotchType *firNotch_create( void );
void firNotch_destroy( firNotchType *pObject );
 void firNotch_init( firNotchType * pThis );
 void firNotch_reset( firNotchType * pThis );
#define firNotch_writeInput( pThis, input )  \
	firNotch_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define firNotch_readOutput( pThis )  \
	(pThis)->output

 int firNotch_filterBlock( firNotchType * pThis, float * pInput, float * pOutput, unsigned int count );
#define firNotch_outputToFloat( output )  \
	(output)

#define firNotch_inputFromFloat( input )  \
	(input)

 void firNotch_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count );
#endif // FIRNOTCH_H_
	
