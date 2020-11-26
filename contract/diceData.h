#ifndef DICE_DATA_CONTRACT_INTERFACE    /* This is an "include guard" */
#define DICE_DATA_CONTRACT_INTERFACE    /* prevents the file from being included twice. */
                     /* Including a header file twice causes all kinds */
                     /* of interesting problems.*/
// https://www.embedded.com/modular-programming-in-c/

#include "vntlib.h"


// uint256 getFee(uint256 amount);

// void setFee(uint256 amount);

void checkOwner();

void checkPool(uint256 amount);

MUTABLE
void SetTotalGameCount(uint64 _totalGameCount);

UNMUTABLE
uint64 getTotalGameCount();

UNMUTABLE
uint64 getTotalGameCount();
void setFee(uint256 _fee);

// get and set balance
UNMUTABLE
uint256 getBalanceOf(address addr);


MUTABLE
void setBalanceOf(address from, uint256 amount);

MUTABLE
void addBalanceOf(address from, uint256 amount);

MUTABLE
void addBalanceOf(address from, uint256 amount);

UNMUTABLE
uint256 GetAmount();

// get and set winning
UNMUTABLE
uint256 getRewardOf(address addr);


MUTABLE
void setRewardOf(address from, uint256 amount);
// get and set for losing
UNMUTABLE
uint256 getLostOf(address addr);


MUTABLE
void setLostOf(address from, uint256 amount);

// get and set win count
UNMUTABLE
uint64 getWinCountOf(address addr);


MUTABLE
void setWinCountOf(address from);

// get and set win count
UNMUTABLE
uint64 getLoseCountOf(address addr);


MUTABLE
void setLoseCountOf(address from);

// get and set win count
UNMUTABLE
uint64 getChikenCountOf(address addr);


MUTABLE
void setChikenCountOf(address from, uint64 _count);

// get and set win count
UNMUTABLE
uint64 getChikenCountOf(address addr);


MUTABLE
void setChikenCountOf(address from, uint64 _count);


// getFee
uint256 getReward(uint256 amount);

void setReward(address _address, uint256 reward);
void setLost(address _address, uint256 amount);

//Check Amount Interface
void checkAmount(uint256 amount);

// get and set freeAddress
UNMUTABLE
uint64 getFreeAddressOf(address addr);

MUTABLE
void setFreeAddressOf(address from);

//Withdraw interface
MUTABLE
void Withdraw(address _sender, uint256 amount);
void WithdrawPool(uint256 amount);



//DepositPool
MUTABLE
void $DepositPool();

//deposit interface
MUTABLE
void $Deposit();

MUTABLE
void addDeposit(address from, uint256 amount);

MUTABLE
void SetNickName(string name);

UNMUTABLE
string GetNickNameFromAddress(address addr);

UNMUTABLE
string GetNickName();

UNMUTABLE
address GetOwner();

UNMUTABLE
uint256 GetAmountFromAddress(address addr);

UNMUTABLE
uint256 GetAmount();


UNMUTABLE
uint256 GetPool();

UNMUTABLE
uint64 GetTotalGameCount();

#endif /* DICE_DATA_CONTRACT_INTERFACE */
