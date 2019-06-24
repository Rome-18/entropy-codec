//
//  CabacEngine.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/23/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "CabacEngine.hpp"
#include "math.h"
#include "memory.h"

cCabacEngine::cCabacEngine()
{
    resetEngine();
}

cCabacEngine::~cCabacEngine()
{
    delete m_bs;
    delete m_pBinaryLow;
    delete m_pBinaryHigh;
    delete m_pBinaryFLow;
    delete m_pBinaryFHigh;
    delete m_pByteBinary;
}

void cCabacEngine::resetEngine()
{
    m_iLow = 0;
    m_iHigh = MAX_RANGE;
    m_iLPS = g_LpsMPS[0] * MAX_RANGE;
    m_iMPS = g_LpsMPS[1] * MAX_RANGE;
    m_iRange = MAX_RANGE;
    
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
    
    m_iMaxBsLen = MAX_BS_LEN;
    m_iBsIdx = 0;
    m_bs = new u_int8_t(m_iMaxBsLen);
    
    m_iBinaryLenI = sizeof(u_int32_t) * 8;
    m_iBinaryLenD = sizeof(double) * 8;

    m_pBinaryLow = new char(m_iBinaryLenI);
    m_pBinaryHigh = new char(m_iBinaryLenI);
    m_pBinaryFLow = new char(m_iBinaryLenD);
    m_pBinaryFHigh = new char(m_iBinaryLenD);
    m_pByteBinary = new char(sizeof(u_int8_t));

    memset(m_bs, 0, m_iMaxBsLen);
    memset(m_pBinaryLow, 0, m_iBinaryLenI);
    memset(m_pBinaryHigh, 0, m_iBinaryLenI);
    memset(m_pBinaryFLow, 0, m_iBinaryLenD);
    memset(m_pBinaryFHigh, 0, m_iBinaryLenD);
    memset(m_pByteBinary, 0, sizeof(u_int8_t));

}

void cCabacEngine::outputBinaryStatus()
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

void cCabacEngine::outputCabacStatus()
{
    printf("          ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%-9.8f, %-9.8f], [%-9.8f, %-9.8f], fR=%-9.8f, encBin=%3d, shft=%d, totalShft=%2d, leftbits=%2d, BsIdx=%2d, NumBy=%d\n",
           m_iLow, m_iHigh, m_iLPS, m_iMPS, m_iRange,  m_fLow, m_fHigh,  m_fLPS, m_fMPS, m_fRange, m_iEncodeBins, m_iShiftBits, m_iTotalShiftBits, m_iLeftBits, m_iBsIdx,m_iNumBytes);
    
}

void cCabacEngine::encodeBinsTest()
{
    u_int32_t iTestSymbol = 0xFFFFFFu;
    u_int8_t iBin = 0;

    for (int32_t i = 0; i < 100; i++) {
        iBin = 1;  //(iTestSymbol & (1 << i)) >> i;
        encodeBin(iBin);
        
        testWrite();
        bsToBinary();
        m_iEncodeBins++;
        
        printf("\n");
    }
}

void cCabacEngine::encodeBin(u_int8_t iBin)
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

void cCabacEngine::decodeBins(char* pBin, const u_int32_t kLen)
{
    //to do
}

void cCabacEngine::renormal()
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

void cCabacEngine::testWrite()
{
    if (m_iLeftBits < 12) {
        //LeadByte is the top 8 bits
        u_int32_t uiLeadByte = m_iLow >> (24 - m_iLeftBits);
        m_iLeftBits += 8;
        m_iLow     &= 0xFFFFFFFFu >> m_iLeftBits;
        
        //whole byte is Outstanding bits
        if (uiLeadByte == 0xff) {
            ++m_iNumBytes;
        } else {
            //output the previous byte, delay output for carry the outstanding bits
            u_int32_t uiCarry = uiLeadByte >> 8;
            m_bs[-1] += uiCarry;
            
            //more than one bytes are outstanding bits
            u_int8_t uiByte = ( 0xff + uiCarry ) & 0xff;
            while (m_iNumBytes > 0) {
                m_bs[m_iBsIdx] = uiByte;
                m_iBsIdx++;
                --m_iNumBytes;
            }
            
            m_bs[m_iBsIdx] = uiLeadByte;
            m_iBsIdx++;
        }
    }
}

void cCabacEngine::byteToBinary(u_int8_t uiSymbol)
{
    int32_t iBin = 0;
    for (int32_t i = 0; i < 8; i++) {
        iBin = (uiSymbol << i) >> (7 - i);
        m_pByteBinary[i] = iBin == 1 ? '1' : '0';
    }
}

void cCabacEngine::bsToBinary()
{
    printf("\n     bs binary, m_iBsIdx=%2d\n", m_iBsIdx);

    for(int32_t i = 0; i < m_iBsIdx; i++) {
        byteToBinary(m_bs[i]);
        
        printf("%s-", m_pByteBinary);
    }
    printf("\n");
}

u_int32_t cCabacEngine::getShiftBits(int32_t iRange)
{
    if (iRange >= (MAX_RANGE >> 1)) {
        return 0;
    }
    
    return  (log(MAX_RANGE / iRange) / log(2));
}

void cCabacEngine::decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen)
{
    u_int8_t iBin = 0;
    double fRemain = fDecimal;
    double fRefVal = 0;
    
    int32_t iTempIdx = m_iBsIdx;
    for (int32_t i = 1; i <= kLen; i++) {
        fRefVal = pow(2, -i);
        iBin = fRemain >= fRefVal ? 1 : 0;
        
        fRemain -= iBin * fRefVal;
        
        pString[i-1] = iBin == 1 ? '1' : '0';
    }
}

void cCabacEngine::intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen) {
    int32_t iBin = 0;
    
    for(int32_t i = 0; i < kLen; i++) {
        iBin = (iSymbol << i) >> (kLen - 1) & 1;
        pString[i] = iBin == 1 ? '1' : '0';
    }
}

void cCabacEngine::outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx)
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


