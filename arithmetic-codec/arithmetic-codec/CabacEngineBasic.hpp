//
//  CabacEngine.hpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/23/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#ifndef CabacEngine_BASIC_hpp
#define CabacEngine_BASIC_hpp

#include <stdio.h>
#include "Common.h"

class cCabacEngine_Basic {
    
public:
    cCabacEngine_Basic();
    ~cCabacEngine_Basic();
    
    void resetEngine();
    
    void encodeBinsTest();
    void encodeBin(u_int8_t iBin);
    void decodeBins(char* pBin, const u_int32_t kLen);
    void renormal();
    void testWrite();
    void outputBinaryStatus();
    void outputCabacStatus();
    void byteToBinary(u_int8_t uiSymbol, const int32_t kByteIdx);
    void bsToBinary();
    
    void decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen);
    void intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen);
    void outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx);
    u_int32_t getShiftBits(int32_t iRange);
    u_int32_t getActualShiftBits();
    void putBits(const u_int32_t kSymbol, const u_int32_t kLen);

private:
    u_int32_t m_iLow;
    u_int32_t m_iHigh;
    u_int32_t m_iLPS;
    u_int32_t m_iMPS;
    u_int32_t m_iRange;
    
    double m_fLow;
    double m_fHigh;
    double m_fLPS;
    double m_fMPS;
    double m_fRange;
    
    u_int32_t m_iEncodeBins;
    u_int32_t m_iShiftBits;
    u_int32_t m_iTotalShiftBits;
    u_int32_t m_iLeftBits;
    u_int32_t m_iOBits;
    u_int32_t m_iOBNum;

    u_int32_t m_iMaxBsLen;
    u_int32_t m_iBsIdx;
    u_int8_t* m_bs;
    u_int8_t* m_bsAll;

    u_int32_t m_iBinaryLenI; //for int
    u_int32_t m_iBinaryLenD; //for double

    char* m_pByteLead;
    char* m_pBinaryLow;
    char* m_pBinaryHigh;
    char* m_pBinaryFLow;
    char* m_pBinaryFHigh;
    char* m_pByteBinary;
    u_int32_t* m_pOBIdx;

};


#endif /* CabacEngine_hpp */
