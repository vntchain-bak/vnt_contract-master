#include "vntlib.h"
#include "diceData.h"

typedef struct
{
  uint256 balance;              
  string nickName;               
  bool freeAddress;                                               
  uint64 winCount;                
  uint64 loseCount;                
  uint64 chickenCount;                               
  uint256 winReward;                   
  uint256 loseAmount;              
} Account;

                                             
KEY mapping(address, Account) accounts;

             
KEY uint64 totalGameCount;

               
KEY uint256 deposit;

          
KEY uint256 fee = U256(10);

                
KEY address owner;

KEY address logicContract;


            
                                                                         
                                                                 
EVENT EVENT_WITHDRAW(indexed address from, string nickname, uint256 amount);
EVENT EVENT_DEPOSIT(indexed address from, string nickname, uint256 amount);
EVENT EVENT_ADD_DEPOSIT(indexed address from, uint256 amount);
EVENT EVENT_NICKNAME(indexed address from, string nickName);
EVENT EVENT_GETFREEVNT(indexed address from, bool got);
EVENT EVENT_SET_BALANCE(indexed address from, uint256 amount);
EVENT EVENT_ADD_BALANCE(indexed address from, uint256 amount);
EVENT EVENT_SUB_BALANCE(indexed address from, uint256 amount);
EVENT EVENT_SET_REWARD_AMOUNT(indexed address from, uint256 amount);
EVENT EVENT_SET_WIN_COUNT(indexed address from);
EVENT EVENT_SET_LOSE_AMOUNT(indexed address from, uint256 amount);
EVENT EVENT_SET_LOSE_COUNT(indexed address from);
EVENT EVENT_SET_CHICKEN_COUNT(indexed address from, uint64 _count);
EVENT EVENT_SET_FREE_ADDRESS(indexed address from);


void keyj165lw5b(){
AddKeyInfo( &accounts.value.winCount, 4, &accounts, 9, false);
AddKeyInfo( &accounts.value.winCount, 4, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.winCount, 4, &accounts.value.winCount, 9, false);
AddKeyInfo( &accounts.value.loseAmount, 5, &accounts, 9, false);
AddKeyInfo( &accounts.value.loseAmount, 5, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.loseAmount, 5, &accounts.value.loseAmount, 9, false);
AddKeyInfo( &accounts.value.balance, 5, &accounts, 9, false);
AddKeyInfo( &accounts.value.balance, 5, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.balance, 5, &accounts.value.balance, 9, false);
AddKeyInfo( &totalGameCount, 4, &totalGameCount, 9, false);
AddKeyInfo( &fee, 5, &fee, 9, false);
AddKeyInfo( &logicContract, 7, &logicContract, 9, false);
AddKeyInfo( &deposit, 5, &deposit, 9, false);
AddKeyInfo( &accounts.value.freeAddress, 8, &accounts, 9, false);
AddKeyInfo( &accounts.value.freeAddress, 8, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.freeAddress, 8, &accounts.value.freeAddress, 9, false);
AddKeyInfo( &accounts.value.winReward, 5, &accounts, 9, false);
AddKeyInfo( &accounts.value.winReward, 5, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.winReward, 5, &accounts.value.winReward, 9, false);
AddKeyInfo( &accounts.value.loseCount, 4, &accounts, 9, false);
AddKeyInfo( &accounts.value.loseCount, 4, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.loseCount, 4, &accounts.value.loseCount, 9, false);
AddKeyInfo( &accounts.value.chickenCount, 4, &accounts, 9, false);
AddKeyInfo( &accounts.value.chickenCount, 4, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.chickenCount, 4, &accounts.value.chickenCount, 9, false);
AddKeyInfo( &owner, 7, &owner, 9, false);
AddKeyInfo( &accounts.value.nickName, 6, &accounts, 9, false);
AddKeyInfo( &accounts.value.nickName, 6, &accounts.key, 7, false);
AddKeyInfo( &accounts.value.nickName, 6, &accounts.value.nickName, 9, false);
}
constructor $Dice()
{
keyj165lw5b();
InitializeVariables();
  owner = GetSender();
  totalGameCount = 0;
}

                     
                                                                    
    

void checkOwner()
{
  address sender = GetSender();
  Require(Equal(sender, owner) == true, "Only the owner can operate");
}

void checkLogicContract()
{
  address sender = GetSender();
  Require(Equal(sender, logicContract) == true, "Only the Logic contract can operate");
}

MUTABLE
void SetTotalGameCount(uint64 _totalGameCount)
{
keyj165lw5b();
  checkLogicContract();
  totalGameCount = _totalGameCount;
}

UNMUTABLE
uint64 getTotalGameCount()
{
keyj165lw5b();
  return totalGameCount;
}

                             
MUTABLE
void setLogicContractAddress(address logicContractAddress)
{
keyj165lw5b();
  checkOwner();
  logicContract = logicContractAddress;
}

UNMUTABLE
address getLogicContractAddress()
{
keyj165lw5b();

  return logicContract;
}

         
MUTABLE
void setFee(uint256 _fee)
{
keyj165lw5b();
  checkLogicContract();
  fee = _fee;
}

UNMUTABLE
uint256 getFee()
{
keyj165lw5b();
  return fee;
}


uint256 getReward(uint256 amount)
{
  PrintUint256T("get amount in getreward:", amount);
  PrintUint256T("get fee1:", fee);
  uint256 res = U256SafeDiv(amount, fee);
  PrintUint256T("get fee2:", res);
  uint256 reward = U256SafeSub(amount, res);
  PrintUint256T("get reward:", reward);
  return reward;
}

          
                             
                                          
                                                                            
                                                                                
                                              
                                    

void setReward(address _address, uint256 reward)
{
  checkLogicContract();
  accounts.key = _address;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
  accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
  deposit = U256SafeAdd(deposit, reward);
  accounts.value.winCount += 1;
}

void setLost(address _address, uint256 amount)
{
  checkLogicContract();
  accounts.key = _address;
  accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
  accounts.value.loseAmount = U256SafeAdd(accounts.value.loseAmount, amount);
  deposit = U256SafeSub(deposit, amount);
  accounts.value.loseCount += 1;
}

                                
void checkPool(uint256 amount)
{
  uint256 contractBalance = GetBalanceFromAddress(GetContractAddress());
  PrintAddress("get contract address:", GetContractAddress());
  PrintUint256T("get contract balance:", contractBalance);
  PrintUint256T("get deposit balance:", deposit);
  uint256 reward = getReward(amount);
  Require(
      U256_Cmp(U256SafeSub(contractBalance,
                           U256SafeAdd(deposit, U256SafeMul(reward, U256(10)))),
               0) != -1,
      "No enough money in prize pool");
}


                                  
void checkAmount(uint256 amount)
{
  Require(U256_Cmp(amount, U256(0) == 1), "amount must > 0");
  address from = GetSender();
  accounts.key = from;
  uint256 balance = accounts.value.balance;
  PrintAddress("get sender:", from);
  PrintUint256T("get balance:", balance);
  Require(U256_Cmp(U256SafeSub(balance, amount), 0) != -1,
          "No enough money to bet");
}

MUTABLE
void Withdraw(address from, uint256 amount)
{
keyj165lw5b();
  checkLogicContract();
  checkAmount(amount);
                                
  if (TransferFromContract(from, amount) == true)
  {
    accounts.key = from;
    accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
    deposit = U256SafeSub(deposit, amount);
    EVENT_WITHDRAW(from, accounts.value.nickName, amount);
  }
}

                                      
MUTABLE
void WithdrawPool(uint256 amount)
{
keyj165lw5b();
  checkLogicContract();
  checkOwner();
  checkPool(amount);
  TransferFromContract(GetSender(), amount);
}

                       
MUTABLE
void $DepositPool() {
keyj165lw5b();}

         
MUTABLE
void $Deposit()
{
keyj165lw5b();
  checkLogicContract();
  uint256 amount = GetValue();
  address from = GetSender();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  deposit = U256SafeAdd(deposit, amount);
  EVENT_DEPOSIT(from, accounts.value.nickName, amount);
}

MUTABLE
void addDeposit(address from, uint256 amount)
{
keyj165lw5b();
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  deposit = U256SafeAdd(deposit, amount);
  EVENT_ADD_DEPOSIT(from, amount);
}


MUTABLE
void SetNickName(string name)
{
keyj165lw5b();
  checkLogicContract();
  address from = GetSender();
  accounts.key = from;
  accounts.value.nickName = name;
  EVENT_NICKNAME(from, name);
}

UNMUTABLE
string GetNickNameFromAddress(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.nickName;
}

UNMUTABLE
string GetNickName() {
keyj165lw5b(); return GetNickNameFromAddress(GetSender()); }






UNMUTABLE
address GetOwner() {
keyj165lw5b(); return owner; }

                      
UNMUTABLE
uint256 getBalanceOf(address addr)
{
keyj165lw5b();
  checkLogicContract();
  accounts.key = addr;
  return accounts.value.balance;
}


MUTABLE
void setBalanceOf(address from, uint256 amount)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = amount;
  EVENT_SET_BALANCE(from, amount);
}

MUTABLE
void addBalanceOf(address from, uint256 amount)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  EVENT_ADD_BALANCE(from, amount);
}

MUTABLE
void subBalanceOf(address from, uint256 amount)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
  EVENT_SUB_BALANCE(from, amount);
}

UNMUTABLE
uint256 GetAmount() {
keyj165lw5b();
  return getBalanceOf(GetSender());
}

                      
UNMUTABLE
uint256 getRewardOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.winReward;
}


MUTABLE
void setRewardOf(address from, uint256 amount)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.winReward = U256SafeAdd(accounts.value.winReward, amount);
  EVENT_SET_REWARD_AMOUNT(from, amount);
}

                         
UNMUTABLE
uint256 getLostOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.winReward;
}


MUTABLE
void setLostOf(address from, uint256 amount)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.loseAmount = U256SafeSub(accounts.value.loseAmount, amount);
  EVENT_SET_LOSE_AMOUNT(from, amount);
}

                        
UNMUTABLE
uint64 getWinCountOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.winCount;
}


MUTABLE
void setWinCountOf(address from)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.winCount += 1;
  EVENT_SET_WIN_COUNT(from);
}

                        
UNMUTABLE
uint64 getLoseCountOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.loseCount;
}


MUTABLE
void setLoseCountOf(address from)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.loseCount += 1;
  EVENT_SET_LOSE_COUNT(from);
}

                        
UNMUTABLE
uint64 getChikenCountOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.chickenCount;
}


MUTABLE
void setChikenCountOf(address from, uint64 count)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  accounts.value.loseCount = count;
  EVENT_SET_CHICKEN_COUNT(from, count);
}

                          
UNMUTABLE
uint64 getFreeAddressOf(address addr)
{
keyj165lw5b();
  accounts.key = addr;
  return accounts.value.freeAddress;
}


MUTABLE
void setFreeAddressOf(address from)
{
keyj165lw5b();
                                
  checkLogicContract();
  accounts.key = from;
  if (accounts.value.freeAddress == false) {
    accounts.value.freeAddress = true;
  }
  EVENT_SET_FREE_ADDRESS(from);
}


UNMUTABLE
uint256 GetPool()
{
keyj165lw5b();
  uint256 amount = GetBalanceFromAddress(GetContractAddress());
  return U256SafeSub(amount, deposit);
}

UNMUTABLE
uint64 GetTotalGameCount() {
keyj165lw5b(); return totalGameCount; }

$_() {
keyj165lw5b();
  $Deposit();
}
