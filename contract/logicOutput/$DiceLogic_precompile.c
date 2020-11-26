#include "vntlib.h"

                        


                               
                                                    
                                                              




KEY address dataContract;
                                                                                    

                                                                       

EVENT EVENT_BET(indexed address from, string nickname, uint256 amount,
                int32 bigger, uint64 lottery, uint256 reward);
                                                                               
                                                                              
                                                               
                                                          




void keynbukjx40(){
AddKeyInfo( &dataContract, 7, &dataContract, 9, false);
}
constructor $DiceLogic()
{
keynbukjx40();
InitializeVariables();

}

                  
uint64 random()
{
  uint64 time = GetTimestamp();
  PrintUint64T("get time", time);
  string time_sha3 = SHA3(SHA3(SHA3(FromU64(time))));
  PrintStr("get time sha3", time_sha3);
  uint64 index = time % 63 + 2;
  PrintStr("get index", index);
  uint64 gas = GetGas() % 64 + 2;
  PrintStr("get gas", gas);
  uint64 random_a = (uint64)time_sha3[index];
  PrintUint64T("get random_a", random_a);
  uint64 random_b = (uint64)time_sha3[index + 1];
  PrintUint64T("get random_b", random_b);
  uint64 random_c = random_a * random_b * gas % 101;
  PrintUint64T("get result", random_c);
  return random_c;
}

                                
                                                      
CALL void SetTotalGameCount(CallParams params, uint64 _totalGameCount);
CALL uint64 getTotalGameCount(CallParams params);
           
CALL void addDeposit(CallParams params, address from, uint256 amount);
                                  
CALL void setFreeAmount(CallParams params, uint256 newfreeAmount);
                                  
CALL void claimFreeChips(CallParams params, address from);

                                    
CALL void checkAmountOf(CallParams params, address from, uint256 amount);
CALL void checkPool(CallParams params, uint256 amount);
CALL uint256 getRewardOf(CallParams params, address from);
CALL void setRewardOf(CallParams params, address from, uint256 amount);
CALL void setLostOf(CallParams params, address from, uint256 amount);
CALL void setNickNameOf(CallParams params, address from, string name);
CALL string getNickNameOf(CallParams params, address addr);

MUTABLE
void setRewardForSender(uint256 amount) {
keynbukjx40();
                                     
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setRewardOf(params, from, amount);
}

MUTABLE
void setLoseForSender(uint256 amount) {
keynbukjx40();
                                     
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setLostOf(params, from, amount);
}

MUTABLE
void setNickNameForSender(string name) {
keynbukjx40();
                                     
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setNickNameOf(params, from, name);
}

UNMUTABLE
void checkAmount(uint256 amount) {
keynbukjx40();
                                     
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  checkAmountOf(params, from, amount);
}

UNMUTABLE
void checkPoolOnDataContract(uint256 amount) {
keynbukjx40();
  CallParams params = {dataContract, U256(0), 100000};
  checkPool(params, amount);
}

MUTABLE
void setDataContractAddress(string _address) {
keynbukjx40();
                                     
  address addr1 = AddressFrom(_address);
  dataContract = addr1;
}

UNMUTABLE
address getDataContractAddress(){
keynbukjx40();
                                                                                     
  return dataContract;
}



MUTABLE
void setTotalGameCountFromDataCtr(uint64 _totalGameCount){
keynbukjx40();
     CallParams params = {dataContract, U256(0), 400000};
     SetTotalGameCount(params, _totalGameCount);
}




UNMUTABLE
uint64 getTotalGameCountFromDataCtr(){
keynbukjx40();
                                                              
                                                              
     CallParams params = {dataContract, U256(0), 40000};
                                                  
     uint64 res = getTotalGameCount(params);
     return res;
}



MUTABLE
void depositFromSender(uint256 amount) {
keynbukjx40();
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 400000};
  addDeposit(params, from, amount);
}


MUTABLE
void setFreeAmountOnDataContract(uint256 newAmount) {
keynbukjx40();
  CallParams params = {dataContract, U256(0), 400000};
  setFreeAmount(params, newAmount);
}


MUTABLE
void claimFreeChipsFrom(){
keynbukjx40();
    address from = GetSender();
    CallParams params = {dataContract, U256(0), 100000};
    claimFreeChips(params, from);
}





MUTABLE
void bet(uint256 amount, int32 bigger)
{
keynbukjx40();
  PrintUint256T("get amount:", amount);
  address sender = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
                                           
                               
  uint64 res = random();
                            

  uint64 totalGameCount = getTotalGameCountFromDataCtr();
  totalGameCount += 1;
  setTotalGameCountFromDataCtr(totalGameCount);
  string _nickName = getNickNameOf(params, sender);
  uint256 reward = getRewardOf(params, sender);

  if (res > 50 && bigger == 1)
  {
              
                                                  
    setRewardOf(params, sender, reward);

    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }

  else if (res < 50 && bigger == -1)
  {
              

    setRewardOf(params, sender, reward);
                                        
    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }
  else if (res == 50 && bigger == 0)
  {
                              
                                          
    reward = U256SafeMul(reward, U256(100));
    setRewardOf(params, sender, reward);
                                        
    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }
  else
  {
               
                             
                                                                            
                                                                                  
                                              
                                     
    setLostOf(params, sender, amount);
                                        
    EVENT_BET(sender, _nickName, amount, bigger, res, U256(0));
  }
}
