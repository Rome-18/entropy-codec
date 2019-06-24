//
//  CabacEngine.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/23/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "CabacEngineBasic.hpp"
#include "math.h"
#include "memory.h"

cCabacEngine_Basic::cCabacEngine_Basic()
{
    resetEngine();
}

cCabacEngine_Basic::~cCabacEngine_Basic()
{
    delete m_bsAll;
    delete m_pByteLead;
    delete m_pBinaryLow;
    delete m_pBinaryHigh;
    delete m_pBinaryFLow;
    delete m_pBinaryFHigh;
    delete m_pByteBinary;
}

void cCabacEngine_Basic::resetEngine()
{
    m_iLow = 0;
    m_iHigh = 0x01FE;
    m_iLPS = g_LpsMPS[0] * 0x01FE;
    m_iMPS = g_LpsMPS[1] * 0x01FE;
    m_iRange = 0x01FE;
    
    m_fLow = 0;
    m_fHigh = 1.0;
    m_fLPS = g_LpsMPS[0];
    m_fMPS = g_LpsMPS[1];
    m_fRange = 1.0;
    
    m_iEncodeBins = 0;
    m_iShiftBits = 0;
    m_iTotalShiftBits = 0;
    m_iLeftBits = 23;

    m_iMaxBsLen = MAX_BS_LEN;
    m_iBsIdx = 0;
    m_bsAll = new u_int8_t(m_iMaxBsLen);
    m_bs = m_bsAll + 1;

    m_iBinaryLenI = sizeof(u_int32_t) * 8;
    m_iBinaryLenD = sizeof(double) * 8;

    m_pByteLead = new char(m_iBinaryLenI);
    m_pBinaryLow = new char(m_iBinaryLenI);
    m_pBinaryHigh = new char(m_iBinaryLenI);
    m_pBinaryFLow = new char(m_iBinaryLenD);
    m_pBinaryFHigh = new char(m_iBinaryLenD);
    m_pByteBinary = new char(sizeof(u_int8_t) * 8 * m_iMaxBsLen);

    
    memset(m_bs, 0, m_iMaxBsLen);
    memset(m_pByteLead, 0, m_iBinaryLenI);
    memset(m_pBinaryLow, 0, m_iBinaryLenI);
    memset(m_pBinaryHigh, 0, m_iBinaryLenI);
    memset(m_pBinaryFLow, 0, m_iBinaryLenD);
    memset(m_pBinaryFHigh, 0, m_iBinaryLenD);
    memset(m_pByteBinary, 0, sizeof(u_int8_t));

}

void cCabacEngine_Basic::outputBinaryStatus()
{
    intToBinaryString(m_iLow, m_pBinaryLow, m_iBinaryLenI);
    intToBinaryString(m_iHigh, m_pBinaryHigh, m_iBinaryLenI);
    decimalToBinaryString(m_fLow, m_pBinaryFLow, m_iBinaryLenD);
    decimalToBinaryString(m_fHigh, m_pBinaryFHigh, m_iBinaryLenD);

    outputBinary(m_pBinaryLow, m_iBinaryLenI, 0);
    outputBinary(m_pBinaryHigh, m_iBinaryLenI, 1);
    outputBinary(m_pBinaryFLow, m_iBinaryLenD, 2);
    outputBinary(m_pBinaryFHigh, m_iBinaryLenD, 3);
}

void cCabacEngine_Basic::outputCabacStatus()
{
    printf("        ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%-9.8f, %-9.8f], [%-9.8f, %-9.8f], fR=%-9.8f, encBin=%3d, shft=%d, totalShft=%2d, leftbits=%2d, BsIdx=%2d\n",
           m_iLow, m_iHigh, m_iLPS, m_iMPS, m_iRange,  m_fLow, m_fHigh,  m_fLPS, m_fMPS, m_fRange, m_iEncodeBins, m_iShiftBits, m_iTotalShiftBits, m_iLeftBits, m_iBsIdx);
    
}

void cCabacEngine_Basic::encodeBinsTest()
{
    u_int8_t iBin = 0;

    for (int32_t i = 0; i < 400; i++) {
        m_iShiftBits = 0;
        iBin = (i % 20 == 19) ? 0 : 1;  //(iTestSymbol & (1 << i)) >> i;
        encodeBin(iBin);
        
        testWrite();
        bsToBinary();
        m_iEncodeBins++;
        
        printf("\n");
    }
}

void cCabacEngine_Basic::encodeBin(u_int8_t iBin)
{
    outputCabacStatus();

    if (iBin == 0) {
        m_iRange = m_iLPS;
        m_fRange = m_fLPS;
    } else {
        m_iRange = m_iMPS;
        m_iLow += m_iLPS;
        
        m_fRange = m_fMPS;
        m_fLow += m_fLPS;
    }
    
    m_iLPS = m_iRange * g_LpsMPS[0];
    m_iMPS = m_iRange - m_iLPS;
    
    m_fHigh = m_fLow + m_fRange;
    m_fLPS = m_fRange * g_LpsMPS[0];
    m_fMPS = m_fRange * g_LpsMPS[1];
    
    
    outputCabacStatus();
    outputBinaryStatus();

    renormal();

    outputCabacStatus();
    outputBinaryStatus();
}

void cCabacEngine_Basic::decodeBins(char* pBin, const u_int32_t kLen)
{
    //to do
}

void cCabacEngine_Basic::renormal()
{
    m_iShiftBits = getShiftBits(m_iRange);
    m_iLow <<= m_iShiftBits;
    m_iRange <<= m_iShiftBits;

    m_iHigh = m_iLow + m_iRange;
    m_iLPS = m_iRange * g_LpsMPS[0];
    m_iMPS = m_iRange - m_iLPS;

    m_iTotalShiftBits += m_iShiftBits;
    m_iLeftBits -= m_iShiftBits;
}

void cCabacEngine_Basic::testWrite()
{
    if(m_iShiftBits > 0 )
    {
        u_int32_t iShiftBin = (m_iLow << m_iLeftBits) >> (32 - m_iShiftBits);
        
        printf("   ----testWrite(), uiLeadByte=%d \n", iShiftBin);
        intToBinaryString(iShiftBin, m_pByteLead, m_iBinaryLenI);
        outputBinary(m_pByteLead, m_iBinaryLenI, 4);
        
        intToBinaryString(m_iLow, m_pBinaryLow, m_iBinaryLenI);
        outputBinary(m_pBinaryLow, m_iBinaryLenI, 0);
        
        m_iLeftBits += m_iShiftBits;
        m_iLow &= 0xFFFFFFFFu >> m_iLeftBits;
        m_iHigh = m_iLow + m_iRange;
       
        intToBinaryString(m_iLow, m_pBinaryLow, m_iBinaryLenI);
        outputBinary(m_pBinaryLow, m_iBinaryLenI, 0);

        intToBinaryString(m_iHigh, m_pBinaryHigh, m_iBinaryLenI);
        outputBinary(m_pBinaryHigh, m_iBinaryLenI, 1);
        
        m_iBsIdx = m_iTotalShiftBits >> 3;

        if (m_iBsIdx > 0) {
            u_int8_t iCarryNum = 8 - (m_iTotalShiftBits - m_iShiftBits) & 0x7;
            u_int8_t iCurBinNum = m_iShiftBits - iCarryNum;
            u_int8_t iCurOffset = (m_iTotalShiftBits - iCurBinNum) & 0x7;
            u_int8_t iCurVal = iShiftBin << (8 - iCurBinNum - iCurOffset);
            
            printf(" ==>    TotalShftBit=%3d, BsIdx=%2d, iCarryNum=%d, iCurOffset=%2d, iCurBinNum=%d, ",
                   m_iTotalShiftBits, m_iBsIdx, iCarryNum,iCurOffset, iCurBinNum);
            u_int8_t iCarryVal = iShiftBin >> (m_iShiftBits - iCarryNum);
            m_bs[m_iBsIdx - 1] += iCarryVal;
            printf("iCarryVal=%d ", iCarryVal);
            printf("curVal=%3d, ", m_bs[m_iBsIdx]);
            m_bs[m_iBsIdx] += iCurVal ;
            
            printf("==>newVal=%3d, curVal=%3d, \n", iCurVal,  m_bs[m_iBsIdx]);
        } else {
            u_int8_t iCurBinNum = m_iShiftBits;
            u_int8_t iCurOffset = (m_iTotalShiftBits - iCurBinNum) & 0x7;
            u_int8_t iCurVal = iShiftBin << (8 - iCurBinNum - iCurOffset);
            
            printf(" ==>    TotalShftBit=%3d, BsIdx=%2d, iCarryNum=%d, iCurOffset=%2d, iCurBinNum=%d, ",
                   m_iTotalShiftBits, m_iBsIdx, 0,iCurOffset, iCurBinNum);
           
            printf("curVal=%3d, ", m_bs[m_iBsIdx]);
            m_bs[m_iBsIdx] += iCurVal ;

            printf("==>newVal=%3d, curVal=%3d, \n", iCurVal,  m_bs[m_iBsIdx]);
        }
        
        intToBinaryString(m_bs[m_iBsIdx], m_pByteBinary, m_iBinaryLenI);
        outputBinary(m_pByteBinary, m_iBinaryLenI, 4);
        
        printf("\n");
    }
}

void cCabacEngine_Basic::byteToBinary(u_int8_t uiSymbol, const int32_t kByteIdx)
{
    int32_t iBin = 0;
    int32_t iOffset = kByteIdx * sizeof(u_int8_t) * 8;

//    printf("uiSymbol=%d,", uiSymbol);

    for (int32_t i = 0; i < 8; i++) {
        iBin = ((uiSymbol << i) & 0xFF) >> 7;
        
//        printf(" i=%d, bin=%d (uiSymbol << i)=%d \n", i, iBin, (u_int8_t)(uiSymbol << i));
        m_pByteBinary[iOffset + i] = iBin == 1 ? '1' : '0';
    }
    
//    printf("\n");
}

void cCabacEngine_Basic::bsToBinary()
{
    printf("\n     bs binary, m_iBsIdx=%2d, bins=", m_iBsIdx);

    for(int32_t i = 0; i < m_iBsIdx; i++) {
        byteToBinary(m_bs[i], i);
    }
    
    for (int32_t i = 0; i < m_iBsIdx * 8; i++) {
        printf("%c", m_pByteBinary[i]);
        if ((i+1) % 4 == 0) {
            printf("-");
        }
    }
    
    printf("\n");
}

u_int32_t cCabacEngine_Basic::getShiftBits(int32_t iRange)
{
    if (iRange >= (MAX_RANGE >> 1)) {
        return 0;
    }
    
    return  (log(MAX_RANGE / iRange) / log(2));
}

void cCabacEngine_Basic::decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen)
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

void cCabacEngine_Basic::intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen) {
    int32_t iBin = 0;
    
    for(int32_t i = 0; i < kLen; i++) {
        iBin = (iSymbol << i) >> (kLen - 1) & 1;
        pString[i] = iBin == 1 ? '1' : '0';
    }
}

void cCabacEngine_Basic::outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx)
{
    if (iPreFixIdx== 0) {
        printf("            iL  =");
    } else if (iPreFixIdx == 1) {
        printf("            iH  =");
    } else if (iPreFixIdx == 2) {
        printf("            fL  =");
    } else if (iPreFixIdx == 3) {
        printf("            fH  =");
    } else {
        printf("      leadByte  =");
    }
    
    for(int32_t i = 0; i < kLen; i++) {
        printf("%c", pString[i]);
        if ((i+1) % 4 == 0) {
            printf("-");
        }
    }
    printf("\n");
}


