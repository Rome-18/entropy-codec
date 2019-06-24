//
//  CabacEngine.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/23/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include "arithmeticCodec.hpp"
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
    delete m_pCharBinary;
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
    m_pCharBinary = new char(sizeof(char));

    memset(m_bs, 0, m_iMaxBsLen);
    memset(m_pBinaryLow, 0, m_iBinaryLenI);
    memset(m_pBinaryHigh, 0, m_iBinaryLenI);
    memset(m_pBinaryFLow, 0, m_iBinaryLenD);
    memset(m_pBinaryFHigh, 0, m_iBinaryLenD);
    memset(m_pCharBinary, 0, sizeof(char));

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
    printf("          ==>int::[%4d, %4d], [%4d, %4d], iR=%4d ---- double::[%-9.8f, %-9.8f], [%-9.8f, %-9.8f], fR=%-9.8f, encBin=%3d, shft=%d, totalShft=%2d, leftbits=%2d \n",
           m_iLow, m_iHigh, m_iLPS, m_iMPS, m_iRange,  m_fLow, m_fHigh,  m_fLPS, m_fMPS, m_fRange, m_iEncodeBins, m_iShiftBits, m_iTotalShiftBits, m_iLeftBits);
    
}

void cCabacEngine::encodeBinsTest()
{
    u_int32_t iTestSymbol = 0xFFFFFFu;
    u_int8_t iBin = 0;

    for (int32_t i = 0; i < 10; i++) {
        iBin = (iTestSymbol & (1 << i)) >> i;
        encodeBin(iBin);
        
        testWrite();
        
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
                *(m_bs++) = uiByte;
                
                m_iBsIdx++;
                --m_iNumBytes;
            }
            
            *(m_bs++) = uiLeadByte;
            m_iBsIdx++;
        }
    }
}

void cCabacEngine::charToBinary(char* pSymbol)
{
    for (int32_t i = 0; i < 8; i++) {
        
    }
    
}



