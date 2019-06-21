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

void decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen)
{
    u_int8_t iBin = 0;
    double fRemain = fDecimal;
    double fRefVal = 0;

    pString[0] = '.';
    for (int32_t i = 1; i < kLen; i++) {
        fRefVal = pow(2, -i);
        iBin = fRemain >= fRefVal ? 1 : 0;
        fRemain -= iBin * fRefVal;
        
        pString[i] = iBin == 1 ? '1' : '0';
    }
    
    pString[kLen - 1] = '\0';
}


void arithmeticTest()
{
    u_int8_t iBin = 0;
    u_int32_t iTestSymbol = 0xFFu;
    u_int32_t iShiftBits = 0;
    u_int32_t iTotalBits = 0;

    int32_t iLPS = 0, iMPS = 0;
    int32_t iLow = 0, iHigh = MAX_RANGE, iRange = MAX_RANGE;
    double fLPS = g_LpsMPS[0], fMPS = g_LpsMPS[1];
    double fLow = 0, fHigh = 0, fRange = 1.0;

    int32_t iLen = sizeof(int32_t) * 8;
    char* p0 = new char(iLen);
    char* p1 = new char(iLen);

    iLPS = g_LpsMPS[0] * MAX_RANGE;
    iMPS = g_LpsMPS[1] * MAX_RANGE;
    iTestSymbol = 0xFFFFFFu;
    for (int32_t i = 0; i < 24; i++) {
        iBin = (iTestSymbol & (1 << i)) >> i;
        printf("i=%2d, bin=%d, int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%9.8f, %9.8f], [%9.8f, %9.8f], fR=%-9.8f,\n",
               i, iBin, iLow, iHigh, iLPS, iMPS, iRange,  fLow, fHigh,  fLPS, fMPS, fRange);
        
        if (iBin == 0) {
            iRange = iLPS;
            fRange = fLPS;
        } else {
            iRange = iMPS;
            iLow += iLPS;
    
            fRange = fMPS;
            fLow += fLPS;
        }
        iLPS = iRange * g_LpsMPS[0];
        iMPS = iRange - iLPS;
        
        fHigh = fLow + fRange;
        fLPS = fRange * g_LpsMPS[0];
        fMPS = fRange * g_LpsMPS[1];
        
        iShiftBits = getShiftBits(iRange);
        iTotalBits += iShiftBits;

        printf("          ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%9.8f, %9.8f], [%9.8f, %9.8f], fR=%-9.8f,\n",
               iLow, iHigh, iLPS, iMPS, iRange,  fLow, fHigh,  fLPS, fMPS, fRange);
        
        decimalToBinaryString(fLow, p0, iLen);
        decimalToBinaryString(fHigh, p1, iLen);

        printf("            ****>L_H[0x%4X, 0x%4X], fL-H[%s, %s],  shiftbits=%d, totalbits=%2d, \n",
               iLow, iHigh, p0, p1, iShiftBits, iTotalBits);
        
        iLow <<= iShiftBits;
        iRange <<= iShiftBits;
        iHigh = iLow + iRange;
        
        iLPS = iRange * g_LpsMPS[0];
        iMPS = iRange - iLPS;
        
        printf("          ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%9.8f, %9.8f], [%9.8f, %9.8f], fR=%-9.8f,\n",
               iLow, iHigh, iLPS, iMPS, iRange,  fLow, fHigh,  fLPS, fMPS, fRange);
        
        printf("            ****>L-H[0x%4X, 0x%4X],  fL-H[%s, %s] \n", iLow, iHigh, p0, p1);

        printf("\n");
    }

    delete p0;
    delete p1;

    printf("\n");
}
