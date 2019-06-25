//
//  main.cpp
//  arithmetic-codec
//
//  Created by edward.shi on 6/21/19.
//  Copyright © 2019 edward.shi. All rights reserved.
//

#include <iostream>
#include "arithmeticCodec.hpp"
#include "CabacEngine.hpp"
#include "CabacEngine-x264.hpp"
#include "CabacEngineBasic.hpp"


int main(int argc, const char * argv[]) {
  
//    arithmeticTest();
    
    cCabacEngine_Basic sCabacTestBasic;
    sCabacTestBasic.encodeBinsTest();
    
//    cCabacEngine_X264 sCabacTestX264;
//    sCabacTestX264.encodeBinsTest();
    
//    cCabacEngine sCabacTest;
//    sCabacTest.encodeBinsTest();
    
//    bins=1111-1110-1111-1101-1000-0111-1111-1011-1110-1111-1111-1001-0100-0111-1111-0100-1100-1111-1110-1101-0101-1111-1110-0000-1111-1111-1100-1101-0011-1111-1010-1100-0111-1111-0111-0100-1111-1111-0001-1011-1111-1110-1000-0111-1111-1101-1001-1011-1111-1100-0000-0011-1111-1001-0110-1111-1111-0101-0000-1111-1110-1101-1100-1111-1110-0001-1011-1111-1100-1110-0111-1111-1010-1101-
    
//     bs binary, m_iBsIdx=37, bins=1111-1110-1111-1101-1000-0111-1111-1011-1110-1111-1111-1001-0100-0111-1111-0100-1100-1111-1110-1101-0101-1111-1110-0000-1111-1111-1100-1101-0011-1111-1010-1100-0111-1111-0111-0100-1111-1111-0001-1011-1111-1110-1000-0111-1111-1101-1001-1011-1111-1100-0000-0011-1111-1001-0110-1111-1111-0101-0000-1111-1110-1101-1100-1111-1110-0001-1011-1111-1100-1110-0111-1111-1010-1101-
//
    
//     bs binary, m_iBsIdx=37, bins=1111-1110-1111-1101-1000-0111-1111-1011-1110-1111-1111-1001-0100-0111-1111-0100-1100-1111-1110-1101-0101-1111-1110-0000-1111-1111-1100-1101-0011-1111-1010-1100-0111-1111-0111-0100-1111-1111-0001-1011-1111-1110-1000-0111-1111-1101-1001-1011-1111-1100-0000-0011-1111-1001-0110-1111-1111-0101-0000-1111-1110-1101-1100-1111-1110-0001-1011-1111-1100-1110-0111-1111-1010-1101-

//  bs binary, m_iBsIdx=38, bins=1111-1110-1111-1101-1000-0111-1111-1011-1110-1111-1111-1001-0100-0111-1111-0100-1100-1111-1110-1101-0101-1111-1110-0000-1111-1111-1100-1101-0011-1111-1010-1100-0111-1111-0111-0100-1111-1111-0001-1011-1111-1110-1000-0111-1111-1101-1001-1011-1111-1100-0000-0011-1111-1001-0110-1111-1111-0101-0000-1111-1110-1101-1100-1111-1110-0001-1011-1111-1100-1110-0111-1111-1010-1101-1111-1111-
//    bs binary, m_iBsIdx=38, bins=1111-0100-1000-0000-0000-0001-1000-0000-1000-0001-1000-0000-1000-0001-1000-0001-0000-0001-1000-0000-0000-0001-1000-0000-1000-0001-1000-0000-1000-0001-0100-0001-0000-0001-0000-0000-1000-0000-1000-0001-1000-0001-0000-0001-1000-0000-0000-0001-1000-0001-1000-0001-1000-0000-0000-0001-1000-0000-1000-0001-1000-0000-0000-0001-1000-0000-0000-0001-1000-0001-0000-0001-0100-0001-1000-0001-
   
    return 0;
}
