//=========================================================================
//Micromodeler fir filters
//=========================================================================
'''#include "filters/firNotch.h"'''
#include "filters/firNotch.cpp"
#include "filters/firLP.h"
#include "filters/firLP.cpp"
#include "filters/firComb.h"
#include "filters/firComb.cpp"

//use
firComb_writeInput(fir3, reading);
reading = firComb_readOutput(fir3);

firLP_writeInput(fir2, reading);
reading = firLP_readOutput(fir2);

//=========================================================================
//filth filthers (no notch available)
//=========================================================================
#include "filters/filt.h"
#include "filters/filt.cpp"

//initialise
Filter* firFilter = new Filter(LPF, 10, 1, 0.2);

//use
reading = firFilter[i]->do_sample((double)reading);


//=========================================================================
//Biquad.cpp filters
//=========================================================================
#include "filters/Biquad.cpp"
#include "filters/Biquad.h"

//inititalise
Biquad* notchFilter = new Biquad(bq_type_notch, 50.0 / 1000, 1, 60);
Biquad* lowPassFilter = new Biquad(bq_type_lowpass, 35.0 / 1000, 1, 60);

//use
reading = notchFilter->process(reading);
reading = lowPassFilter->process(reading);
