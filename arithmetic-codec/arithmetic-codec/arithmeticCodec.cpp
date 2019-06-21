//
//  arithmeticCodec.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/21/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "arithmeticCodec.hpp"
#include "math.h"

const double g_LpsMPS[2] = {0.25, 0.75};

u_int32_t getShiftBits(int32_t iRange)
{
    if (iRange >= (MAX_RANGE >> 1)) {
        return 0;
    }

    return  (log(MAX_RANGE / iRange) / log(2));
}

void arithmeticTest()
{
    u_int8_t iBin = 0;
    u_int32_t iTestSymbol = 0xFFu;
    u_int32_t iShiftBits = 0;

    int32_t iLPS = 0, iMPS = 0;
    int32_t iLow = 0, iHigh = MAX_RANGE, iRange = MAX_RANGE;
    double fLPS = g_LpsMPS[0], fMPS = g_LpsMPS[1];
    double fLow = 0, fHigh = 0, fRange = 1.0;

    iLPS = g_LpsMPS[0] * MAX_RANGE;
    iMPS = g_LpsMPS[1] * MAX_RANGE;
    for (int32_t i = 0; i < 8; i++) {
        iBin = iTestSymbol & (1 << i);
        if (iBin == 0) {
            iRange = iLPS;
            fRange = fLPS;
        } else {
            iRange = iMPS;
            iLow += iLPS;
    
            fRange = fMPS;
            fLow += fLPS;
        }
        
        fHigh = fLow + fRange;
        fLPS = fRange * g_LpsMPS[0];
        fMPS = fRange * g_LpsMPS[1];
        
        iShiftBits = getShiftBits(iRange);
        
        iLow <<= iShiftBits;
        iRange <<= iShiftBits;
        iHigh = iLow + iRange;
        
        iLPS = iRange * g_LpsMPS[0];
        iMPS = iRange - iLPS;
    }

}
