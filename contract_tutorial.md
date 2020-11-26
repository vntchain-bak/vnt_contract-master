# VNT Dapp development process

## Vnt network
* [`VNT Test net`](https://hubscan.vnt.link/developer/test)
* [`VNT Public net`](https://hubscan.vnt.link/developer/join) 
* [`Private Network Deployment`](./network_deployment.md)

## development tools

* [``Bottle``](https://github.com/vntchain/bottle)，The vnt contract tool is used for code prompting, code compilation, abi generation, compression and decompression of contract writing.。
* [``VNT Contract for vscode``](https://github.com/vntchain/vnt-contract-vscode)，The vscode plugin prompted by the contract code
* [``vnt.js``](https://github.com/vntchain/vnt.js)，JavaScript Interface, used to provide common account query, transfer, contract and other related operations。
* [``vnt-kit.js``](https://github.com/vntchain/vnt-kit.js)，JavaScript interface, used to provide basic wallet-related operations。

## Contract syntax

* [`Chinese Language`](https://github.com/vntchain/vnt-documentation/blob/master/smart-contract/deploy-contract-tutorial.md)
* [`English`](./contract_syntax.md)

## Contract Development
This section contain several sections, mainly focus on the development of contract:
1. Writing a simple smart contract on VNT platform
Please refer to [`Contract Writing](#Contract-writing) section

2. Compile a contract using bottle
Please refer to [`Contract compile`](#Contract-compilation) section

3. Deploy a contract on VNT network
Plese refer to [`Contract Deployment`](#Contract-Deployment) section

4. Query and revoke contract
Please refer to [`Query and revoke contract`](#Contract-Revoke-and-Query) section

5. Upgradable contract pattern development
Please refer to [`Upgradable Smart Contract Pattern`](#Upgradable-Smart-Contract-Pattern) section

### Contract writing
For Chinese reference, please visit [`How to Create a Smart contract`](https://hubscan.vnt.link/developer/dapp/develop)

For detail of contract syntax and references, please refer to [`Contract Syntax`](./contract_syntac.md)

The VNT contract is written in the C language, and at the same time absorbs some of the grammar of the Ethereum contract language solidity, so that developers with a foundation in the C language and solidity can easily develop a smart contract

The following is an example of ***Dice Contract*** without any upgradability

```c
#include "vntlib.h"

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

// Owner address
KEY address owner;

// Free amount
KEY uint256 freeAmount = U256(100000000000000000000); // 100*10**18;

// Events
EVENT EVENT_BET(indexed address from, string nickname, uint256 amount,
                int32 bigger, uint64 lottery, uint256 reward);
EVENT EVENT_WITHDRAW(indexed address from, string nickname, uint256 amount);
EVENT EVENT_DEPOSIT(indexed address from, string nickname, uint256 amount);
EVENT EVENT_NICKNAME(indexed address from, string nickName);
EVENT EVENT_GETFREEVNT(indexed address from, bool got);

constructor $Dice()
{
  owner = GetSender();
  totalGameCount = 0;
}

// getFee
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

// Check if there is enough to bet
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

//Is the prize pool sufficient?
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

void checkOwner()
{
  address sender = GetSender();
  Require(Equal(sender, owner) == true, "Only the owner can operate");
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

UNMUTABLE
uint64 testRandom() { return random(); }

//-1:<50,0:=50,1:>50
MUTABLE
void Bet(uint256 amount, int32 bigger)
{
  PrintUint256T("get amount:", amount);
  checkAmount(amount);
  checkPool(amount);
  address sender = GetSender();
  uint64 res = random();
  totalGameCount += 1;
  if (res > 50 && bigger == 1)
  {
    // you win
    accounts.key = sender;
    uint256 reward = getReward(amount);
    accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
    accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
    deposit = U256SafeAdd(deposit, reward);
    accounts.value.winCount += 1;
    EVENT_BET(sender, accounts.value.nickName, amount, bigger, res, reward);
  }
  else if (res < 50 && bigger == -1)
  {
    // you win
    accounts.key = sender;
    uint256 reward = getReward(amount);
    accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
    accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
    deposit = U256SafeAdd(deposit, reward);
    accounts.value.winCount += 1;
    EVENT_BET(sender, accounts.value.nickName, amount, bigger, res, reward);
  }
  else if (res == 50 && bigger == 0)
  {
    // you are the luckist man
    accounts.key = sender;
    uint256 reward = getReward(amount);
    reward = U256SafeMul(reward, U256(100));
    accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
    accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
    deposit = U256SafeAdd(deposit, reward);
    accounts.value.chickenCount += 1;
    EVENT_BET(sender, accounts.value.nickName, amount, bigger, res, reward);
  }
  else
  {
    // you lose
    accounts.key = sender;
    accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
    accounts.value.loseAmount = U256SafeAdd(accounts.value.loseAmount, amount);
    deposit = U256SafeSub(deposit, amount);
    accounts.value.loseCount += 1;
    EVENT_BET(sender, accounts.value.nickName, amount, bigger, res, U256(0));
  }
}

//Withdrawal
MUTABLE
void Withdraw(uint256 amount)
{
  checkAmount(amount);
  address from = GetSender();
  if (TransferFromContract(from, amount) == true)
  {

    accounts.key = from;
    accounts.value.balance = U256SafeSub(accounts.value.balance, amount);
    deposit = U256SafeSub(deposit, amount);
    EVENT_WITHDRAW(from, accounts.value.nickName, amount);
  }
}

//Withdrawall All
MUTABLE
void WithdrawAll()
{
  accounts.key = GetSender();
  uint256 amount = accounts.value.balance;
  Withdraw(amount);
}

//Withdraw from pool, only owner
MUTABLE
void WithdrawPool(uint256 amount)
{
  checkOwner();
  checkPool(amount);
  TransferFromContract(GetSender(), amount);
}

//Withdraw the prize pool, only owner
MUTABLE
void WithdrawPoolAll()
{
  uint256 amount = GetBalanceFromAddress(GetContractAddress());
  WithdrawPool(amount);
}

//Expand the prize pool
MUTABLE
void $DepositPool() {}

//deposit
MUTABLE
void $Deposit()
{
  uint256 amount = GetValue();
  address from = GetSender();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  deposit = U256SafeAdd(deposit, amount);
  EVENT_DEPOSIT(from, accounts.value.balance, amount);
}

//Get 100VNT chips for free, each account can get one time
MUTABLE
void GetFreeChips()
{
  address from = GetSender();
  accounts.key = from;
  bool flag = accounts.value.freeAddress;
  Require(flag == false, "you have got before");
  accounts.value.balance = U256SafeAdd(accounts.value.balance, freeAmount);
  deposit = U256SafeAdd(deposit, freeAmount);
  accounts.value.freeAddress = true;
  EVENT_GETFREEVNT(from, true);
}

// Nickname of better
MUTABLE
void SetNickName(string name)
{
  address from = GetSender();
  accounts.key = from;
  accounts.value.nickName = name;
  EVENT_NICKNAME(from, name);
}

UNMUTABLE
string GetNickNameFromAddress(address addr)
{
  accounts.key = addr;
  return accounts.value.nickName;
}

UNMUTABLE
string GetNickName() { return GetNickNameFromAddress(GetSender()); }

UNMUTABLE
address GetOwner() { return owner; }

UNMUTABLE
uint256 GetAmountFromAddress(address addr)
{
  accounts.key = addr;
  return accounts.value.balance;
}

UNMUTABLE
uint256 GetAmount() { return GetAmountFromAddress(GetSender()); }

UNMUTABLE
string GetWinAndLose()
{
  accounts.key = GetSender();
  uint64 win = accounts.value.winCount;
  uint64 lose = accounts.value.loseCount;
  uint64 chicken = accounts.value.chickenCount;
  return Concat(
      Concat(Concat(Concat(FromU64(win), "-"), FromU64(chicken)), "-"),
      FromU64(lose));
}

UNMUTABLE
uint256 GetPool()
{
  uint256 amount = GetBalanceFromAddress(GetContractAddress());
  return U256SafeSub(amount, deposit);
}

UNMUTABLE
uint64 GetTotalGameCount() { return totalGameCount; }

$_() { $Deposit(); }

```

The above code is an example of a smart contract for guessing numbers, which includes the generation of random numbers, the storage and reading of data on the chain, event events, contract transfers and other key operations, more detailed syntax reference[Documentation](https://github.com/vntchain/vnt-documentation/blob/master/smart-contract/write-contract.md)

## Contract compilation

Need to use for contract compilation``bottle``,After compilation, abi files and xxx.compress files will be generated. The abi files are used to interact with the contract. The compress file is the compressed bytecode of the contract and is used for contract deployment.

```
$ bottle compile -code ./contract/dice.c


>>>Compile finished. 633.684578ms
>>>Input file
   Contract path :./contract/dice.c
>>>Output file
   Abi path: contract/output/$Dice.abi
   Precompile code path: contract/output/$Dice_precompile.c
   Wasm path: contract/output/$Dice.wasm
   Compress Data path: contract/output/$Dice.compress
   Compress Hex Data path: contract/output/$Dice.hex
   Deploy JS path: contract/output/$Dice.js
>>>Please use $Dice.compress when you want to create a contract
```

Other options:
$ bottle compile -code ./contract/diceData.c --output ./contract/dataOutput

--code value Specific a contract code path
  --include Specific the head file directory need by contract
  --output Specific a output directory path
  --file value Specific a compress file path to decompress
  --abi value Specific a abi path needed by contract
  --wasm value Specific a wasm path

## Contract deployment

Before deploying the contract, you need to unlock the account. The keystore file and unlock password are required to unlock. After unlocking, the account private key is obtained. The transaction can be signed with the private key, and the signed transaction can be sent to the chain.


```js
import vntkit from 'vnt-kit';
var unlockAccount = (keystore, passwd) => {
    return vntkit.account.decrypt(keystore, passwd, false)
};
```


```js
import Account from 'ethereumjs-account';
import TX from 'ethereumjs-tx';
import Vnt from 'vnt';
import vntkit from 'vnt-kit';

var vnt = new Vnt();
vnt.setProvider(new vnt.providers.HttpProvider(Config.providerUrl));

// The compress file and abi file are needed to deploy the contract
var codeFile =
    '../output/$Dice.compress';
var abiFile =
    '../output/abi.json';
var wasmabi = fs.readFileSync(abiFile);
var abi = JSON.parse(wasmabi.toString('utf-8'));


function deployWasmContract() {
  var contract = vnt.core.contract(abi).codeFile(codeFile);
  var deployContract = contract.packContructorData(
      {
        data: contract.code,  
        gas: 4000000,
        value: vnt.toWei(100000000, 'vnt'),
      });
  var nonce = vnt.core.getTransactionCount(from);
  var options = {
    nonce: nonce,
    gasPrice: vnt.toHex(30000000000000),
    gasLimit: vnt.toHex(4000000),
    data: deployContract,
    value: vnt.toHex(vnt.toWei(value)),
    chainId: CHAINID
  };
  var tx = new TX(options);
  tx.sign(new Buffer(
        prikey.substring(
            2,
            ),
        'hex'));
  var serializedTx = tx.serialize();
  vnt.core.sendRawTransaction(
        '0x' + serializedTx.toString('hex'), function(err, txHash) {
          if (err) {
            console.log('err happened: ', err)
            console.log('transaction hash: ', txHash);
          } else {
            console.log('transaction hash: ', txHash);
            var receipt = vnt.core.getTransactionReceipt("txHash")
            var contractAddress = receipt.contractAddress
          }
  });
}
```

After the contract is successfully deployed, the contract will be obtained``address``，The address represents the only entry of the contract. Make a note of this address, and it will be used in all operations related to the contract.

## Contract Revoke and Query

The execution of the contract requires the use of the abi file and the address of the contract. Take the bet method and GetNickName in the above example as an example
Before executing the contract, you need to unlock the account first. For unlocking refer to the code in the deployment contract

Bet
```js
var requestBet = (amount, bigger, from, prikey, cb) => {
    var funcName = 'Bet';
    var data = contract.packFunctionData(funcName, [vnt.toWei(amount), bigger]);
    sendTransaction(data, 0, from, prikey, cb);
  };
var sendTransaction = (data, value, from, prikey, cb) => {
    var nonce = vnt.core.getTransactionCount(from);
    var options = {
      nonce: nonce,
      to: contractAddress,
      gasPrice: vnt.toHex(30000000000000),
      gasLimit: vnt.toHex(4000000),
      data: data,
      value: vnt.toHex(vnt.toWei(value)),
      chainId: CHAINID
    };
    var tx = new TX(options);
    tx.sign(new Buffer(
        prikey.substring(
            2,
            ),
        'hex'));
    var serializedTx = tx.serialize();
    vnt.core.sendRawTransaction(
        '0x' + serializedTx.toString('hex'), function(err, txHash) {
          cb(err, txHash);
          if (err) {
            console.log('err happened: ', err)
            console.log('transaction hash: ', txHash);
          } else {
            console.log('transaction hash: ', txHash);
          }
        });
  };
```

GetNickName
```js
var requestNickName = (from, prikey, cb) => {
    var funcName = 'GetNickName';
    var data = contract.packFunctionData(funcName);
    var options =
        {from: from, to: contractAddress, data: data, chainId: CHAINID};
    vnt.core.call(options, function(err, res) {
      cb(err, contract.unPackOutput(funcName, res));
    });
  };
```
## Upgradable Smart Contract Pattern
In order to achieve upgradable smart contract, a target contract need to be separated into two distinctive contracts, data and logic contract.
This seperation will make sure that data on data contract will remain on the VNT blockchain after the upgrading of logic contract.


### Data contract
The data contract simply contains three parts:
- Data: all mutable and immutable data that will very unlikely change or being upgraded in the future, 
  For example, in ***Dice Contarct***, the balances of player, or broadly the accounts of players can be defined as data on data contract.
  ```c
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
  ```

- Interfaces: also interfaces to manipulate this data
  Once data is defined on data contract, some interfaces are also needed to be defined in order for logic contract to access and manipulate this data. For instance, the following interafces used to access and update state of reward of an given account.

  ```c
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
      checkLogicContract();
      accounts.key = _address;
      accounts.value.balance = U256SafeAdd(accounts.value.balance, reward);
      accounts.value.winReward = U256SafeAdd(accounts.value.winReward, reward);
      deposit = U256SafeAdd(deposit, reward);
      accounts.value.winCount += 1;
    }
  ```
- Authentication: besides, an authentication interface needed to authorised a target logic contract to interact with. For instance, the checkLogicContract() function as below used to authorise a given logic contract to access reward state
  ```c
    void checkLogicContract()
    {
      address sender = GetSender();
      Require(Equal(sender, logicContract) == true, "Only the Logic contract can operate");
    }
  ```

The following is an example of ***DiceData Contract***

```c
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
  Require(U256_Cmp(U256SafeSub(contractBalance, U256SafeAdd(deposit, U256SafeMul(reward, U256(10)))), 0) != -1, "No enough money in prize pool");
  Require(U256_Cmp(U256SafeSub(contractBalance, U256SafeAdd(deposit, U256SafeMul(reward, U256(10)))), 0) != -1, "No enough money in prize pool");
}


// Check if there is enough to bet
UNMUTABLE
void checkAmountOf(address from, uint256 amount)
{
  // Require(U256_Cmp(amount, U256(0) == 1), "amount must > 0");
  Require(U256_Cmp(amount, U256(0)), "amount must > 0");
  accounts.key = from;
  uint256 balance = accounts.value.balance;
  PrintAddress("get sender:", from);
  PrintUint256T("get balance:", balance);
  Require(U256_Cmp(U256SafeSub(balance, amount), 0) != -1, "No enough money to bet");
}

MUTABLE
void Withdraw(address from, uint256 amount)
{
  checkLogicContract();
  checkAmountOf(from, amount);
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
  checkLogicContract();
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
  checkLogicContract();
  accounts.key = from;
  accounts.value.balance = U256SafeAdd(accounts.value.balance, amount);
  deposit = U256SafeAdd(deposit, amount);
  EVENT_ADD_DEPOSIT(from, amount);
}


MUTABLE
void SetNickName(string name)
{
  checkLogicContract();
  address from = GetSender();
  accounts.key = from;
  accounts.value.nickName = name;
  EVENT_NICKNAME(from, name);
}

MUTABLE
void setNickNameOf(address from, string name)
{
  checkLogicContract();
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
  checkLogicContract();
  accounts.key = addr;
  return accounts.value.balance;
}


MUTABLE
void setBalanceOf(address from, uint256 amount)
{

  checkLogicContract();
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

  checkLogicContract();
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
  checkLogicContract();
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
  checkLogicContract();
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
  checkLogicContract();
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
  checkLogicContract();
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
  checkLogicContract();
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
  checkLogicContract();
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


```

### Logic Contract
- The logic contract designed to be upgradable without any loss of meaning or significant data, therefore, it contains most of upgradable part of a contract, as well as cross-call functions to access to data contract.
1. Logic part
- Any business logic part of a contract need to be placed in this contract, including some data with posibility of upgrading in the future.

2. Cross-contract calls
- In order to access the data contract, these CALL functions need to be defined in logic contract as reference form vntlib.h
- For example, the following ared used to access TotalGameCount state on ***DataDice Contract***

  ```c
    CALL uint64 getTotalGameCount(CallParams params);
    MUTABLE
    void setTotalGameCountFromDataCtr(uint64 _totalGameCount){
        CallParams params = {dataContract, U256(0), 400000};
        SetTotalGameCount(params, _totalGameCount);
    }
  ```


The following is an example of ***DiceLogic Contract**

```c
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


// Define name of logic contract
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


```

### Proxy-upgradable contract pattern
- This require an access from vntlib.h with a functionality to call contract without changing of original address. Currenlty, vntlib.hs has not implimented it yet, or has not exposed this interface yet.


## Summary

The above are the necessary steps to complete a vnt smart contract. For Dapp development, in addition to writing a creative, practical and interesting smart contract, it will also involve the display of the dapp front end, data interaction, and for some more complex Dapp also needs to record some useful information on the server side.
