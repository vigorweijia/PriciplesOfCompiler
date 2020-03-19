#ifndef __EEROR_H__
#define __EEROR_H__

typedef enum{
    ERROR_OTHER = 0,
    ERROR_MISSING_SEMI = 1,
    ERROR_MISSING_LC = 2,
    ERROR_MISSING_RC = 3,
    ERROR_MISSING_LP = 4,
    ERROR_MISSING_RP = 5,
    ERROR_MISSING_LB = 6,
    ERROR_MISSING_RB = 7
}ErrorType;

const char* errorMsg[] = {
    "Syntax Error",  //0
    "Missing \";\"", //1
    "Missing \"}\"",     //2
    "Missing \"]\"",     //3
    "Missing \")\""      //4
    
};

#endif
