%%{
    # number machine
    machine number;

    # Actions
    # handle a number
    action setNegative {
        #ifdef DEBUG
        std::cout << "setNegative" << std::endl;
        #endif
        intIsNeg = true;
    }
    action recordInt {
        gotAtLeastOneDigit = true;
        intPart *= 10;
        intPart += *p - '0';
        #ifdef DEBUG
        std::cout << "recordInt " << *p << " - " << intPart << std::endl; 
        #endif
    }
    action recordDecimal {
        intPart *= 10;
        intPart += *p - '0';
        --expPart1; // the 'actual' end exponent will be way at the end
        #ifdef DEBUG
        std::cout << "recordDecimal " << *p << " - " << intPart << " - " << expPart1 << std::endl; 
        #endif
    }
    action setExpNeg {
        if (*p == '-')
            expIsNeg = true;
        #ifdef DEBUG
        std::cout << "setExpNeg " << expIsNeg << std::endl;
        #endif
    }
    action recordExponent {
        expPart2 *= 10;
        expPart2 += *p - '0';
        #ifdef DEBUG
        std::cout << "recordExponent " << *p << " - " << expPart2 << std::endl;
        #endif
    }
    action gotNumber {
        #ifdef DEBUG
        std::cout << "gotNumber " << expIsNeg << " - " << expPart1 << " - " << expPart2 << " - " << intPart << " - " << intIsNeg << " - ";
        #endif
        return makeJSONNumber();
    }
    # JSON Number expression
    basic_int = ('-'?@setNegative).([0-9]+@recordInt)@recordInt;
    basic_float = (basic_int|'-'?.'0').'.'.([0-9]+)@recordDecimal;
    exponent = [eE].([+\-]?@setExpNeg).([0-9]+)@recordExponent;
    number = (basic_int . (exponent?) | basic_float . (exponent?)) % gotNumber;

}%%
