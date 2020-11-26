#include "vntlib.h"

// #include "diceData.h"


// for calling another contract
// CALL uint64 getTotalGameCount(CallParams params);
// CALL uint32 test(CallParams params,int32 var1,string var2);




KEY address dataContract;
// KEY address dataContract = Address("0x431efa70fd152855c31eeca24b055d5b591d9ca2");

// KEY uint256 freeAmount = U256(100000000000000000000); // 100*10**18;

EVENT EVENT_BET(indexed address from, string nickname, uint256 amount,
                int32 bigger, uint64 lottery, uint256 reward);
// EVENT EVENT_WITHDRAW(indexed address from, string nickname, uint256 amount);
// EVENT EVENT_DEPOSIT(indexed address from, string nickname, uint256 amount);
// EVENT EVENT_NICKNAME(indexed address from, string nickName);
// EVENT EVENT_GETFREEVNT(indexed address from, bool got);



constructor $DiceLogic()
{

}

// random function
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

// INTERFACES FROM DATA CONTRACT
// for set and get Total Game Count from data Contract
CALL void SetTotalGameCount(CallParams params, uint64 _totalGameCount);
CALL uint64 getTotalGameCount(CallParams params);
// Deposite
CALL void addDeposit(CallParams params, address from, uint256 amount);
// change the freeAmount if needed
CALL void setFreeAmount(CallParams params, uint256 newfreeAmount);
// claim the freeAmount for player
CALL void claimFreeChips(CallParams params, address from);

// bet need the following interfaces
CALL void checkAmountOf(CallParams params, address from, uint256 amount);
CALL void checkPool(CallParams params, uint256 amount);
CALL uint256 getRewardOf(CallParams params, address from);
CALL void setRewardOf(CallParams params, address from, uint256 amount);
CALL void setLostOf(CallParams params, address from, uint256 amount);
CALL void setNickNameOf(CallParams params, address from, string name);
CALL string getNickNameOf(CallParams params, address addr);

MUTABLE
void setRewardForSender(uint256 amount) {
  // string _comingString = _address;
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setRewardOf(params, from, amount);
}

MUTABLE
void setLoseForSender(uint256 amount) {
  // string _comingString = _address;
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setLostOf(params, from, amount);
}

MUTABLE
void setNickNameForSender(string name) {
  // string _comingString = _address;
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  setNickNameOf(params, from, name);
}

UNMUTABLE
void checkAmount(uint256 amount) {
  // string _comingString = _address;
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  checkAmountOf(params, from, amount);
}

UNMUTABLE
void checkPoolOnDataContract(uint256 amount) {
  CallParams params = {dataContract, U256(0), 100000};
  checkPool(params, amount);
}

MUTABLE
void setDataContractAddress(string _address) {
  // string _comingString = _address;
  address addr1 = AddressFrom(_address);
  dataContract = addr1;
}

UNMUTABLE
address getDataContractAddress(){
  // address _contracAddress = Address("0xe122cba856617107b8620ea043e1e650d8695b9c");
  return dataContract;
}



MUTABLE
void setTotalGameCountFromDataCtr(uint64 _totalGameCount){
     CallParams params = {dataContract, U256(0), 400000};
     SetTotalGameCount(params, _totalGameCount);
}




UNMUTABLE
uint64 getTotalGameCountFromDataCtr(){
  // CallParams params = {dataContract, U256(10000), 4000000};
  // the second parameter: amount of token if transfer is used
     CallParams params = {dataContract, U256(0), 40000};
    //  0x431efa70fd152855c31eeca24b055d5b591d9ca2
     uint64 res = getTotalGameCount(params);
     return res;
}



MUTABLE
void depositFromSender(uint256 amount) {
  address from = GetSender();
  CallParams params = {dataContract, U256(0), 400000};
  addDeposit(params, from, amount);
}


MUTABLE
void setFreeAmountOnDataContract(uint256 newAmount) {
  CallParams params = {dataContract, U256(0), 400000};
  setFreeAmount(params, newAmount);
}


MUTABLE
void claimFreeChipsFrom(){
    address from = GetSender();
    CallParams params = {dataContract, U256(0), 100000};
    claimFreeChips(params, from);
}





MUTABLE
void bet(uint256 amount, int32 bigger)
{
  PrintUint256T("get amount:", amount);
  address sender = GetSender();
  CallParams params = {dataContract, U256(0), 100000};
  checkAmountOf(params, sender, amount);
  // checkPool(params, amount); //need to tranfer token to data contract first.
  uint64 res = random();
  // increase totalGameCount

  uint64 totalGameCount = getTotalGameCountFromDataCtr();
  totalGameCount += 1;
  setTotalGameCountFromDataCtr(totalGameCount);
  string _nickName = getNickNameOf(params, sender);
  uint256 reward = getRewardOf(params, sender);

  if (res > 50 && bigger == 1)
  {
    // you win
    setRewardOf(params, sender, reward);
    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }

  else if (res < 50 && bigger == -1)
  {
    // you win

    setRewardOf(params, sender, reward);
    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }
  else if (res == 50 && bigger == 0)
  {
    // you are the luckist man
    reward = U256SafeMul(reward, U256(100));
    setRewardOf(params, sender, reward);
    EVENT_BET(sender, _nickName, amount, bigger, res, reward);
  }
  else
  {
    // you lose
    setLostOf(params, sender, amount);
    EVENT_BET(sender, _nickName, amount, bigger, res, U256(0));
  }
}
