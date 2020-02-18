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

#include "firLP.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float firLP_coefficients[24] =
{
	0.0000000, 0.0000000, 0.0000000, -0.0024400327, 0.018535163, -0.00026379662,
	-0.026167375, 0.000046880735, 0.048551599, 0.00033935547, -0.096447034, -0.00062657734,
	0.31498454, 0.50072684, 0.31498454, -0.00062657734, -0.096447034, 0.00033935547,
	0.048551599, 0.000046880735, -0.026167375, -0.00026379662, 0.018535163, -0.0024400327
};


firLPType* firLP_create(void)
{
	firLPType* result = (firLPType*)malloc(sizeof(firLPType));	// Allocate memory for the object
	firLP_init(result);											// Initialize it
	return result;																// Return the result
}

void firLP_destroy(firLPType* pObject)
{
	free(pObject);
}

void firLP_init(firLPType* pThis)
{
	firLP_reset(pThis);

}

void firLP_reset(firLPType* pThis)
{
	memset(&pThis->state, 0, sizeof(pThis->state)); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

int firLP_filterBlock(firLPType* pThis, float* pInput, float* pOutput, unsigned int count)
{
	float* pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;

	for (; count; --count)
	{
		pThis->pointer[firLP_length] = *pInput;						// Copy sample to top of history buffer
		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if (pThis->pointer >= pThis->state + firLP_length)				// Handle wrap-around
			pThis->pointer -= firLP_length;

		accumulator = 0;
		firLP_dotProduct(pThis->pointer, firLP_coefficients, &accumulator, firLP_length);
		*(pOutput++) = accumulator;	// Store the result
	}

	return pOutput - pOriginalOutput;

}

void firLP_dotProduct(float* pInput, float* pKernel, float* pAccumulator, short count)
{
	float accumulator = *pAccumulator;
	while (count--)
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}


