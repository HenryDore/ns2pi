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

#include "firNotch.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float firNotch_coefficients[8] = 
{
	-0.024864046, 0.083522330, -0.18707572, 0.62841744, 0.62841744, -0.18707572,
	0.083522330, -0.024864046
};


firNotchType *firNotch_create( void )
{
	firNotchType *result = (firNotchType *)malloc( sizeof( firNotchType ) );	// Allocate memory for the object
	firNotch_init( result );											// Initialize it
	return result;																// Return the result
}

void firNotch_destroy( firNotchType *pObject )
{
	free( pObject );
}

 void firNotch_init( firNotchType * pThis )
{
	firNotch_reset( pThis );

}

 void firNotch_reset( firNotchType * pThis )
{
	memset( &pThis->state, 0, sizeof( pThis->state ) ); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

 int firNotch_filterBlock( firNotchType * pThis, float * pInput, float * pOutput, unsigned int count )
{
	float *pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;
 
 	for( ;count; --count )
 	{
 		pThis->pointer[firNotch_length] = *pInput;						// Copy sample to top of history buffer
 		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if( pThis->pointer >= pThis->state + firNotch_length )				// Handle wrap-around
			pThis->pointer -= firNotch_length;
		
		accumulator = 0;
		firNotch_dotProduct( pThis->pointer, firNotch_coefficients, &accumulator, firNotch_length );
		*(pOutput++) = accumulator;	// Store the result
 	}
 
	return pOutput - pOriginalOutput;

}

 void firNotch_dotProduct( float * pInput, float * pKernel, float * pAccumulator, short count )
{
 	float accumulator = *pAccumulator;
	while( count-- )
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}


