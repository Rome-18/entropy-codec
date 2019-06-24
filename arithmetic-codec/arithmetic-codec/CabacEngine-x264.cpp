//
//  CabacEngine.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/23/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "CabacEngine-x264.hpp"
#include "math.h"
#include "memory.h"

cCabacEngine_X264::cCabacEngine_X264()
{
    resetEngine();
}

cCabacEngine_X264::~cCabacEngine_X264()
{
    delete m_bsAll;
    delete m_pByteLead;
    delete m_pBinaryLow;
    delete m_pBinaryHigh;
    delete m_pBinaryFLow;
    delete m_pBinaryFHigh;
    delete m_pByteBinary;
}

void cCabacEngine_X264::resetEngine()
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
    
    m_iNumBytes = 0;
    m_iEncodeBins = 0;
    m_iShiftBits = 0;
    m_iTotalShiftBits = 0;
    m_iLeftBits = 23;
    m_iQueueBits = -9;
    
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

void cCabacEngine_X264::outputBinaryStatus()
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

void cCabacEngine_X264::outputCabacStatus()
{
    printf("        ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%-9.8f, %-9.8f], [%-9.8f, %-9.8f], fR=%-9.8f, encBin=%3d, shft=%d, totalShft=%2d, leftbits=%2d, BsIdx=%2d, NumBy=%d, QueBit = %d\n",
           m_iLow, m_iHigh, m_iLPS, m_iMPS, m_iRange,  m_fLow, m_fHigh,  m_fLPS, m_fMPS, m_fRange, m_iEncodeBins, m_iShiftBits, m_iTotalShiftBits, m_iLeftBits, m_iBsIdx,m_iNumBytes, m_iQueueBits);
    
}

void cCabacEngine_X264::encodeBinsTest()
{
    u_int8_t iBin = 0;

    for (int32_t i = 0; i < 400; i++) {
        iBin = (i % 20 == 19) ? 0 : 1;  //(iTestSymbol & (1 << i)) >> i;
        encodeBin(iBin);
        
        testWrite();
        bsToBinary();
        m_iEncodeBins++;
        
        printf("\n");
    }
}

void cCabacEngine_X264::encodeBin(u_int8_t iBin)
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

void cCabacEngine_X264::decodeBins(char* pBin, const u_int32_t kLen)
{
    //to do
}

void cCabacEngine_X264::renormal()
{
    m_iShiftBits = getShiftBits(m_iRange);
    m_iLow <<= m_iShiftBits;
    m_iRange <<= m_iShiftBits;

    m_iHigh = m_iLow + m_iRange;
    m_iLPS = m_iRange * g_LpsMPS[0];
    m_iMPS = m_iRange - m_iLPS;

    m_iTotalShiftBits += m_iShiftBits;
    m_iLeftBits -= m_iShiftBits;
    
    m_iQueueBits += m_iShiftBits;
}

void cCabacEngine_X264::testWrite()
{
    if(m_iQueueBits >= 0 )
    {
        int out = m_iLow >> (m_iQueueBits + 10);

        printf("   ----testWrite(), uiLeadByte=%d \n", out);
        intToBinaryString(out, m_pByteLead, m_iBinaryLenI);
        outputBinary(m_pByteLead, m_iBinaryLenI, 4);

        intToBinaryString(m_iLow, m_pBinaryLow, m_iBinaryLenI);
        outputBinary(m_pBinaryLow, m_iBinaryLenI, 0);
    
        m_iLow  &= (0x400<< m_iQueueBits) -1;
        m_iHigh = m_iLow + m_iRange;
        m_iQueueBits -= 8;
        m_iLeftBits += 8;

        intToBinaryString(m_iLow, m_pBinaryLow, m_iBinaryLenI);
        outputBinary(m_pBinaryLow, m_iBinaryLenI, 0);
    
        if( (out & 0xff) == 0xff )
            m_iNumBytes++;
        else
        {
            int carry = out >> 8;
            int32_t iIdx = m_iBsIdx;
            // this can't modify before the beginning of the stream because
            // that would correspond to a probability > 1.
            // it will write before the beginning of the stream, which is ok
            // because a slice header always comes before cabac data.
            // this can't carry beyond the one byte, because any 0xff bytes
            // are in bytes_outstanding and thus not written yet.
            m_bs[iIdx -1] += carry;
            
            printf("    carry=%d, carry - 1 = %d \n", carry, carry-1);
    
            while( m_iNumBytes > 0 )
            {
                m_bs[m_iBsIdx] = carry-1;
                m_iBsIdx++;
                --m_iNumBytes;
            }
           
            m_bs[m_iBsIdx] = out;
            m_iBsIdx++;
        }
    }
}

void cCabacEngine_X264::byteToBinary(u_int8_t uiSymbol, const int32_t kByteIdx)
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

void cCabacEngine_X264::bsToBinary()
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

u_int32_t cCabacEngine_X264::getShiftBits(int32_t iRange)
{
    if (iRange >= (MAX_RANGE >> 1)) {
        return 0;
    }
    
    return  (log(MAX_RANGE / iRange) / log(2));
}

void cCabacEngine_X264::decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen)
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

void cCabacEngine_X264::intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen) {
    int32_t iBin = 0;
    
    for(int32_t i = 0; i < kLen; i++) {
        iBin = (iSymbol << i) >> (kLen - 1) & 1;
        pString[i] = iBin == 1 ? '1' : '0';
    }
}

void cCabacEngine_X264::outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx)
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


