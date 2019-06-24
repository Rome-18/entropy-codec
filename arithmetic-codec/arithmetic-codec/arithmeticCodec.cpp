//
//  arithmeticCodec.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/21/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "arithmeticCodec.hpp"
#include "math.h"

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

    for (int32_t i = 1; i <= kLen; i++) {
        fRefVal = pow(2, -i);
        iBin = fRemain >= fRefVal ? 1 : 0;
        
        fRemain -= iBin * fRefVal;
        
        pString[i-1] = iBin == 1 ? '1' : '0';
    }
}

void intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen) {
    int32_t iBin = 0;
    
    for(int32_t i = 0; i < kLen; i++) {
        iBin = (iSymbol << i) >> (kLen - 1) & 1;
        pString[i] = iBin == 1 ? '1' : '0';
    }
}

void outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx)
{
    if (iPreFixIdx== 0) {
        printf("            iL  =");
    } else if (iPreFixIdx == 1) {
        printf("            iH  =");
    } else if (iPreFixIdx == 2) {
        printf("            fL  =");
    } else {
        printf("            fH  =");
    }

    for(int32_t i = 0; i < kLen; i++) {
        printf("%c", pString[i]);
        if ((i+1) % 4 == 0) {
            printf("-");
        }
    }
    printf("\n");
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
    char* p0 = new char(iLen + 1);
    char* p1 = new char(iLen + 1);
    char* p2 = new char(iLen + 1);
    char* p3 = new char(iLen + 1);
    

    iLPS = g_LpsMPS[0] * MAX_RANGE;
    iMPS = g_LpsMPS[1] * MAX_RANGE;
    iTestSymbol = 0xFFFFFFu;
    for (int32_t i = 0; i < 100; i++) {
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
        
        intToBinaryString(iLow, p0, iLen);
        intToBinaryString(iHigh, p1, iLen);
        decimalToBinaryString(fLow, p2, iLen);
        decimalToBinaryString(fHigh, p3, iLen);

        outputBinary(p0, iLen, 0);
        outputBinary(p1, iLen, 1);
        outputBinary(p2, iLen, 2);
        outputBinary(p3, iLen, 3);

        iLow <<= iShiftBits;
        iRange <<= iShiftBits;
        iHigh = iLow + iRange;
        
        iLPS = iRange * g_LpsMPS[0];
        iMPS = iRange - iLPS;
        
        printf("          ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%9.8f, %9.8f], [%9.8f, %9.8f], fR=%-9.8f, shiftbits=%d, totalbits=%2d\n",
               iLow, iHigh,
               iLPS, iMPS, iRange,
               fLow, fHigh,
               fLPS, fMPS, fRange,
               iShiftBits, iTotalBits);
        
        intToBinaryString(iLow, p0, iLen);
        intToBinaryString(iHigh, p1, iLen);
        outputBinary(p0, iLen, 0);
        outputBinary(p1, iLen, 1);

        printf("\n");
    }

    delete p0;
    delete p1;
    delete p2;
    delete p3;

    printf("\n");
}
