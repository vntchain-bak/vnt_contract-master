#include "vntlib.h"
#include "diceData.h"

typedef struct
{
  uint256 balance;     //deposit
  string nickName;     //nickname
  bool freeAddress;    //Have you already received the gift chips?
  uint64 winCount;     //Games won
  uint64 loseCount;    //Games lost
  uint64 chickenCount; //Guess the number of 50 games
  uint256 winReward;   //Winning profit
  uint256 loseAmount;  //Total lost
} Account;

//Account balance, mapping balance to address
KEY mapping(address, Account) accounts;

//Total games
KEY uint64 totalGameCount;

//Total deposit
KEY uint256 deposit;

// 10% fee
KEY uint256 fee = U256(10);

KEY uint256 freeAmount = U256(100000000000000000000); // 100*10**18;

// Owner address
KEY address owner;

KEY address logicContract;


// // Events
// EVENT EVENT_BET(indexed address from, string nickname, uint256 amount,
//                 int32 bigger, uint64 lottery, uint256 reward);
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

constructor $DiceData()
{
  owner = GetSender();
  totalGameCount = 0;
}

// bool ownerOnly() {
//   Require(GetSender() == GetOwner()), "Only Owner for the task");
// }

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
  checkLogicContract();
  totalGameCount = _totalGameCount;
}

UNMUTABLE
uint64 getTotalGameCount()
{
  return totalGameCount;
}

// set and get logic contract
MUTABLE
void setLogicContractAddress(address logicContractAddress)
{
  checkOwner();
  logicContract = logicContractAddress;
}

UNMUTABLE
address getLogicContractAddress()
{

  return logicContract;
}

// getFee
MUTABLE
void setFee(uint256 newFee)
{
  // checkLogicContract();
  fee = newFee;
}

UNMUTABLE
uint256 getFreeAmount()
{
  return freeAmount;
}

// getFee
MUTABLE
void setFreeAmount(uint256 newfreeAmount)
{
  // checkLogicContract();
  freeAmount = newfreeAmount;
}

UNMUTABLE
uint256 getFee()
{
  return fee;
}

UNMUTABLE
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

// you win
    // accounts.key = sender;
    // uint256 reward = getReward(amount);
    // accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
    // accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
    // deposit = U256SafeAdd(deposit, reward);
    // accounts.value.winCount += 1;
MUTABLE
void setReward(address _address, uint256 reward)
{
  // checkLogicContract();
  accounts.key = _address;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
  accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
  deposit = U256SafeAdd(deposit, reward);
  accounts.value.winCount += 1;
}

MUTABLE
void setLost(address _address, uint256 amount)
{
  // checkLogicContract();
  accounts.key = _address;
  accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
  accounts.value.loseAmount = U256SafeAdd(accounts.value.loseAmount, amount);
  deposit = U256SafeSub(deposit, amount);
  accounts.value.loseCount += 1;
}

// Is the prize pool sufficient?
UNMUTABLE
void checkPool(uint256 amount)
{
  uint256 contractBalance = GetBalanceFromAddress(GetContractAddress());
  PrintAddress("get contract address:", GetContractAddress());
  PrintUint256T("get contract balance:", contractBalance);
  PrintUint256T("get deposit balance:", deposit);
  uint256 reward = getReward(amount);
  // Require(U256_Cmp(U256SafeSub(contractBalance, U256SafeAdd(deposit, U256SafeMul(reward, U256(10)))), 0) != -1, "No enough money in prize pool");
  // Require(U256_Cmp(U256SafeSub(contractBalance, U256SafeAdd(deposit, U256SafeMul(reward, U256(10)))), 0) != -1, "No enough money in prize pool");
}


// Check if there is enough to bet
UNMUTABLE
void checkAmountOf(address from, uint256 amount)
{
  // Require(U256_Cmp(amount, U256(0) == 1), "amount must > 0");
  Require(U256_Cmp(amount, U256(0)), "amount must > 0");
  // address from = GetSender();
  accounts.key = from;
  uint256 balance = accounts.value.balance;
  PrintAddress("get sender:", from);
  PrintUint256T("get balance:", balance);
  Require(U256_Cmp(U256SafeSub(balance, amount), 0) != -1, "No enough money to bet");
}

MUTABLE
void Withdraw(address from, uint256 amount)
{
  // checkLogicContract();
  checkAmountOf(from, amount);
  // address from = GetSender();
  if (TransferFromContract(from, amount) == true)
  {
    accounts.key = from;
    accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
    deposit = U256SafeSub(deposit, amount);
    EVENT_WITHDRAW(from, accounts.value.nickName, amount);
  }
}

// Withdraw the prize pool, only owner
MUTABLE
void WithdrawPool(uint256 amount)
{
  // checkLogicContract();
  checkOwner();
  checkPool(amount);
  TransferFromContract(GetSender(), amount);
}

//Expand the prize pool
MUTABLE
void $DepositPool() {}

//deposit
MUTABLE
void $Deposit()
{
  // checkLogicContract();
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
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  deposit = U256SafeAdd(deposit, amount);
  EVENT_ADD_DEPOSIT(from, amount);
}


MUTABLE
void SetNickName(string name)
{
  // checkLogicContract();
  address from = GetSender();
  accounts.key = from;
  accounts.value.nickName = name;
  EVENT_NICKNAME(from, name);
}

MUTABLE
void setNickNameOf(address from, string name)
{
  // checkLogicContract();
  // address from = GetSender();
  accounts.key = from;
  accounts.value.nickName = name;
  EVENT_NICKNAME(from, name);
}

UNMUTABLE
string getNickNameOf(address addr)
{
  accounts.key = addr;
  return accounts.value.nickName;
}

UNMUTABLE
string GetNickName() { return getNickNameOf(GetSender()); }






UNMUTABLE
address GetOwner() { return owner; }

// get and set balance
UNMUTABLE
uint256 getBalanceOf(address addr)
{
  // checkLogicContract();
  accounts.key = addr;
  return accounts.value.balance;
}


MUTABLE
void setBalanceOf(address from, uint256 amount)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.balance = amount;
  EVENT_SET_BALANCE(from, amount);
}

MUTABLE
void addBalanceOf(address from, uint256 amount)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  EVENT_ADD_BALANCE(from, amount);
}

MUTABLE
void subBalanceOf(address from, uint256 amount)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
  EVENT_SUB_BALANCE(from, amount);
}

UNMUTABLE
uint256 GetAmount() {
  return getBalanceOf(GetSender());
}

// get and set winning
UNMUTABLE
uint256 getRewardOf(address addr)
{
  accounts.key = addr;
  return accounts.value.winReward;
}


MUTABLE
void setRewardOf(address from, uint256 amount)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.winReward = U256SafeAdd(accounts.value.winReward, amount);
  EVENT_SET_REWARD_AMOUNT(from, amount);
}

// get and set for losing
UNMUTABLE
uint256 getLostOf(address addr)
{
  accounts.key = addr;
  return accounts.value.loseAmount;
}


MUTABLE
void setLostOf(address from, uint256 amount)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.loseAmount = U256SafeAdd(accounts.value.loseAmount, amount);
  EVENT_SET_LOSE_AMOUNT(from, amount);
}

// get and set win count
UNMUTABLE
uint64 getWinCountOf(address addr)
{
  accounts.key = addr;
  return accounts.value.winCount;
}


MUTABLE
void setWinCountOf(address from)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.winCount += 1;
  EVENT_SET_WIN_COUNT(from);
}

// get and set win count
UNMUTABLE
uint64 getLoseCountOf(address addr)
{
  accounts.key = addr;
  return accounts.value.loseCount;
}


MUTABLE
void setLoseCountOf(address from)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.loseCount += 1;
  EVENT_SET_LOSE_COUNT(from);
}

// get and set win count
UNMUTABLE
uint64 getChikenCountOf(address addr)
{
  accounts.key = addr;
  return accounts.value.chickenCount;
}


MUTABLE
void setChikenCountOf(address from, uint64 count)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  accounts.value.loseCount = count;
  EVENT_SET_CHICKEN_COUNT(from, count);
}

// get and set freeAddress
UNMUTABLE
uint64 getFreeAddressOf(address addr)
{
  accounts.key = addr;
  return accounts.value.freeAddress;
}


MUTABLE
void setFreeAddressOf(address from)
{
  // address from = GetSender();
  // checkLogicContract();
  accounts.key = from;
  if (accounts.value.freeAddress == false) {
    accounts.value.freeAddress = true;
  }
  EVENT_SET_FREE_ADDRESS(from);
}

MUTABLE
void claimFreeChips(address from)
{
  accounts.key = from;
  bool flag = accounts.value.freeAddress;
  Require(flag == false, "you have got before");
  accounts.value.balance = U256SafeAdd(accounts.value.balance, freeAmount);
  deposit = U256SafeAdd(deposit, freeAmount);
  accounts.value.freeAddress = true;
  EVENT_GETFREEVNT(from, true);
}


UNMUTABLE
uint256 GetPool()
{
  uint256 amount = GetBalanceFromAddress(GetContractAddress());
  return U256SafeSub(amount, deposit);
}

UNMUTABLE
uint256 getBalanceOfContract()
{
  uint256 amount = GetBalanceFromAddress(GetContractAddress());
  return amount;
}


$_() {
  $Deposit();
}
