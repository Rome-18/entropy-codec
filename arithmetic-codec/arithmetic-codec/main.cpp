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


int main(int argc, const char * argv[]) {
  
//    arithmeticTest();
    
    cCabacEngine sCabacTest;
    sCabacTest.encodeBinsTest();
    
    return 0;
}
