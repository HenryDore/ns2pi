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

#include "firComb.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset


firCombType* firComb_create(void)
{
	firCombType* result = (firCombType*)malloc(sizeof(firCombType));	// Allocate memory for the object
	firComb_init(result);											// Initialize it
	return result;																// Return the result
}

void firComb_destroy(firCombType* pObject)
{
	free(pObject);
}

void firComb_init(firCombType* pThis)
{
	firComb_reset(pThis);

}

void firComb_reset(firCombType* pThis)
{
	memset(&pThis->state, 0, sizeof(pThis->state)); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

int firComb_filterBlock(firCombType* pThis, float* pInput, float* pOutput, unsigned int count)
{
	float accumulator;
	int originalCount = count;
	while (count--)
	{
		accumulator = *pInput;		// The input sample
		accumulator += *(pThis->pointer) * 0.89247311827957;		// The oldest sample (multiplied by alpha if applicable)

		*(pOutput++) = accumulator * 0.5;	// Output the result

		*(pThis->pointer++) = *(pInput++);							// Store the new sample in the circular history buffer
		if (pThis->pointer >= pThis->state + firComb_length)		// Handle wrap-around
			pThis->pointer -= firComb_length;
	}
	return originalCount;

}


