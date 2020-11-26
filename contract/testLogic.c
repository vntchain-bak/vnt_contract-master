#include "vntlib.h"
#include "diceData.h"



CALL uint32 getTotalGameCount(CallParams params);


MUTABLE
uint32 testcall(){
     CallParams prams = {Address("0xaaaa")};
     uint32 res = getTotalGameCount(prams);
     printf("amount is: ", res);
}

