# Micromodeler fir filters
    #include "filters/firNotch.h"
    #include "filters/firNotch.cpp"
    #include "filters/firLP.h"
    #include "filters/firLP.cpp"
    #include "filters/firComb.h"
    #include "filters/firComb.cpp"
## initialise
    firNotchType* fir1 = firNotch_create();
    firLPType* fir2 = firLP_create();   
    firCombType* fir3 = firComb_create();
## use
    firNotch_writeInput(fir1, reading);
    reading = firNotch_readOutput(fir1);
    firLP_writeInput(fir2, reading);
    reading = firLP_readOutput(fir2);
    firComb_writeInput(fir3, reading);
    reading = firComb_readOutput(fir3);
# FILTH filthers (no notch available)
    #include "filters/filt.h"
    #include "filters/filt.cpp"
## initialise
    Filter* firFilter = new Filter(LPF, 10, 1, 0.2);
## use
    reading = firFilter->do_sample((double)reading);
# Biquad.cpp filters
    #include "filters/Biquad.cpp"
    #include "filters/Biquad.h"
## initialise
    Biquad* notchFilter = new Biquad(bq_type_notch, 50.0 / 1000, 1, 60);
    Biquad* lowPassFilter = new Biquad(bq_type_lowpass, 35.0 / 1000, 1, 60);
## use
    reading = notchFilter->process(reading);
    reading = lowPassFilter->process(reading);
