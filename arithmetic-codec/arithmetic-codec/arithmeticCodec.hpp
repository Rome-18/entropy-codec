//
//  arithmeticCodec.hpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/21/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#ifndef arithmeticCodec_hpp
#define arithmeticCodec_hpp

#include <stdio.h>
#include "Common.h"

void arithmeticTest();
void decimalToBinaryString(double fDecimal,  char* pString, const int32_t kLen);
void intToBinaryString(int32_t iSymbol, char* pString, const int32_t kLen);
void outputBinary(char* pString, const int32_t kLen, int32_t iPreFixIdx);

u_int32_t getShiftBits(int32_t iRange);

#endif /* arithmeticCodec_hpp */
