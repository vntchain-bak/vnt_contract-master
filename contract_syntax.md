# Get started

## How to crate a smart contract
For Chinese references, please visit [`How to Create a Smart contract`](https://hubscan.vnt.link/developer/dapp/develop)

### VNT smart contract
Smart contracts in VNT can be written based on C language. The smart contract contains state variables that can persist data and methods that can modify these variables and access these variables.

#### Create smart contract
1. Use vnt libry
Before creating a smart contract, you need to import vntlib.h or copy the content of vntlib.h to the starting position of the smart contract

Download vntlib.h

`#include "vntlib"`

2. Define contract name.
```c
constructor ContractSample(){
  // some initiations of contract if any
}
```

#### State variables
1. Define State variables
State variables are used to persist data to the chain. State variables include simple types such as int32, int64, uint32, uint64, uint256, string, address, bool, and complex types such as mapping, array, and struct composed of these simple types. State variables are declared through the KEY keyword, and immutable constants can be modified with const. For simple types and struct types composed of simple types, initialization and assignment can be performed at the same time as the declaration. State variables must be declared in accordance with global variables in the c language. The assignment of the simple state variable is consistent with the reading and the variable in the C language.

2. Declaration and initialization of simple state variables
//Only declare
KEY int32 var1;
//Declare and initialize
KEY int32 var2 = 1;
KEY const int32 var3;
KEY string var4;
KEY string var5 = "string";
KEY uint256 var6 = U256(1000000000000);
KEY address var7;
KEY address var8 = Address("0xaaa");
//The declaration and assignment of multiple variables of the same type can be separated by commas
KEY int32 var9,var10;
KYE int32 var11 = 1, var12 = 2;
note:

The assignment of the uint256 type needs to use the keyword U256 to convert the number.
Assignment to the address type needs to use the keyword Address to convert the string.
Usage of U256 and U256From: U256 can be used for uint256 type initialization and the conversion of numeric constants in the method body, U256From can only be used for the conversion of string constants and variables in the method body

#### Usage of Address and AddressFrom: 
Address can be used for the initialization of address type and the conversion of string constants in the method body, AddressFrom can only be used for the conversion of string constants and variables in the method body

- Note: to asign address to available, this address has to be initiated first:
```c
  KEY address addr2;
  address addr1 = Address("0x1");
  

KEY uint256 var1 = U256(1000000000000);
KEY address var82 = Address("0xaaa");

void functionbody(){
  string a ="0x1";
  string b = "10000";
  address addr1 = Address("0x1");
  address addr2 = AddressFrom("0x1");
  address addr2 = AddressFrom(a);

  uint256 addr1 = U256(1000);
  uint256 addr2 = U256From("100000");
  uint256 addr2 = U256From(b);

}
```

#### Struct type
1. Definition, declaration and initialization of struct type state variables
The definition of struct type follows the definition rules of c language struct
```c
struct S1{
     int32 a;
};
//Only declare
KEY struct S1 var1;
//Declare and initialize
KEY struct S1 var2 = {1};
//Only declare
KEY struct {
    int32 a;
} var3;
//Declare and initialize
KEY struct {
    int32 a;
} var4 = {1};

//Use typedef to define struct
typedef struct S2{
     int32 a;
} s2;
//Only declare
KEY s2 var5;
//Declare and initialize
KEY s2 var5 = {1};


KEY struct {
  string str;
  address addr;
  uint256 u256;
  uint64 u64;
} s1 = {"teststringinstruct", Address("0xaaaaaa11"), U256(10000000000011),
        1000001};

KEY struct {
  string str;
  address addr;
  uint256 u256;
  uint64 u64;
  struct {
    string str;
    address addr;
    uint256 u256;
    uint64 u64;
  } s;
} s2 = {"teststringinstruct",
        Address("0xaaaaaa11"),
        U256(10000000000011),
        1000001,
        {"teststringinstructstruct", Address("0xaaaaaa1122"),
         U256(1000000000001122), 10000012));
```

#### Declaration of array type state variable
The array type variable is used to store any type of data with uint64 numeric index. The array has three elements of index, value, and length. The length is set for the array through length, and the value and index of any type are combined through the joint assignment of index and value. index to associate. The type of the parameter value when the array is declared
```c
//array declaration;
KEY array(string) var1;

//array assignment
var1.length = 1;
var1.index = 0;
var1.value = "value";

//array value
var1.index = 0;
string val = var1.value;
note:
```

The length of the array must be set before the first assignment of the array
Assignment and value operations need to be assigned to the index in advance, and then the value assignment and value operations can be associated with the previously defined index
For multiple assignments to index, value will only be associated with the closest index in the code
The value of the array can use any type, so it is possible to nest the array in the array
Array does not support initialization in the declaration.

#### Declaration of mapping type state variables
The mapping type variable is used to store any type of data with a simple type index. Mapping has two elements of key and value, and any type of value and index key are associated through the joint assignment of key and value. In the mapping statement, the first parameter is the type of key, and the second parameter is the type of value
```c
//Mapping declaration;
KEY mapping(int32,string) var1;

//Mapping assignment
var1.key = 0;
var1.value = "value";

//Mapping value
var1.key = 0;
string val = var1.value;
```
notes:
Assignment and value operations need to be assigned to the key in advance, and then the value assignment and value operations can be associated with the previously defined key
For multiple assignments to the key, the value will only be associated with the closest key in the code
The value of the mapping can use any type, so it is possible to nest the mapping in the mapping
Mapping does not support initialization in the declaration.

#### Compound declaration of mapping, array, struct
The value of mapping and array can use any type, and the variable in struct can use any type. Therefore, mapping, array, and struct can be nested with each other.

1. Array nested array declaration
```c
KEY array(array(string)) var1;

//Assignment
var1.length = 0;
var1.index = 0;
var1.value.length = 1;
var1.value.index = 0;
var1.value.value = "value";

//Value
var1.index = 0;
var1.value.index = 0;
string var = var1.value.value;
Declaration of array nested mapping
//
KEY array(mapping(string,string)) var1;

//Assignment
var1.length = 1;

var1.index = 0;
var1.value.key = "key";
var1.value.value = "value";

//Value
var1.index = 0;
var1.value.key = "key";
string var = var1.value.value;
```

2. Mapping declaration of nested mapping
```c
KEY mapping(string,mapping(string,string)) var1;

//Assignment
var1.key = "key1";
var1.value.key = "key2";
var1.value.value = "value";

//Value
var1.key = "key1";
var1.value.key = "key2";
string var = var1.value.value;
```

3. Declaration of mapping nested array
```c
KEY mapping(string,array(string)) var1;

//Assignment
var1.key = "key";
var1.value.length = 1;
var1.value.index = 0;
var1.value.value = "value";

//Value
var1.key = "key";
var1.value.index = 0;
string var = var1.value.value;
```

4. Declaration of mapping nested struct
```c
KEY mapping(string,struct{
   int32 a;
   string b;
}) var1;

//Assignment
var1.key = "key";
var1.value.a = 0;
var1.value.b = "b";

//Value
var1.key = "key";
int32 a = var1.value.a;
string b = var1.value.b;
struct nested array declaration
KEY struct{
     int32 a;
     array(string) b;
} var1;

//Assignment
var1.a = 1;

var1.b.length = 1;
var1.b.index = 0;
var1.b.value = "value";

//Value
int32 a1 = var1.a;

var1.b.index = 0;
string b1 = var1.b.value;
```

5. Declaration of struct nested mapping
```c
KEY struct{
     int32 a;
     mapping(string,string) b;
} var1;

//Assignment
var1.a = 1;

var1.b.key = "key";
var1.b.value = "value";

//Value
int32 a1 = var1.a;

var1.b.key = "key";
string b1 = var1.b.value;
struct nested struct declaration
KEY struct{
     int32 a;
     struct{
          int32 b;
          string c;
     } d;
} var1;

//Assignment
var1.a = 1;

var1.d.b = 1;
var1.d.c = "value";

//Value
int32 a = var1.a;
int32 b = var1.d.b;
string c = var1.d.c;
```

#### Multi-level nesting

For the multi-level nesting of mapping, array and struct, the value and assignment need to follow:

- Before assigning and fetching the value, you need to assign the index key or index in all levels before the value to be able to get or assign the value.
- For the nested array, before assigning a value to the array for the first time, the length of the array needs to be set, otherwise the length of the array is 0
Functions
- The smart contract uses the function function to access and modify the state variables. Like the C language, the function can have multiple parameters and only one return value. Function can be modified with MUTABLE or UNMUTABLE, and MUTABLE or UNMUTABLE needs to be written in the previous line of function. The modified function can be accessed by the outside, and the unmodified function is an internal function and cannot be accessed by the outside.

- Functions that can be accessed externally
The parameter types and return value types of functions that can be accessed externally are restricted, and currently only simple types are allowed:

  + Input parameter types: 
      int32, int64, uint32, uint64, uint256, string, address, bool
  + Return value types: 
      int32, int64, uint32, uint64, uint256, string, address, bool and void
  + Input parameters and return values ​​do not support:
      pointer, mapping, array, struct and other custom types

- There are 4 types of functions that can be accessed externally, MUTABLE and UNMUTBALE, Payable and Unpayable

1. MUTABLE
The method modified by MUTABLE will change the state, so access to the method requires a submission of a transaction
For example: contract.someFunction.sendTransaction([someVariable,]{..})

2. UNMUTBALE
UNMUTABLE does not modify state variables, so no transaction is required to access the method
For instance: contract.someMethods.call([someVariable,]{...})

3. Payable
The Payable function can receive native tokens from the transaction, which is represented by the symbol `$`, which needs to be written at the top of the method name

The declaration of the Payable function:
```c
MUTABLE
uint32 $testfunction1(int32 var1,string var2,address var3){

}
```
4. Unpayable
- If there is no $ sign at the beginning of the method name, it means that the method is Unpayable and it is forbidden to receive native tokens from the transaction

- Declaration of Unpayable function:
```c
MUTABLE
uint32 testfunction1(int32 var1,string var2,address var3){

}

UNMUTABLE
uint32 testfunction2(int32 var1,string var2,address var3){

}
```

#### Internal functions
- Internal functions follow the definition of C language functions without any restrictions.

#### Event EVENT
- EVENT provides an abstraction for the logging function of WAVM. Applications can subscribe to and monitor these events through the client's RPC interface. The event is declared by the keyword EVENT. The event only needs to declare the method name and parameters, and the event has no return value. Event parameter types: int32, int64, uint32, uint64, uint256, string, address, bool event parameters can be modified by indexed, modified parameters will be indexed, indexed needs to be written in the call of the event before the parameter type and C language The method call is the same
```c
//statement
EVENT testEvent(indexed int32 var1,string var2);

//transfer
testEvent(1,"string");
```
#### Call CALL across contracts
CALL provides a way for cross-contract access. The accessed object is a function in the contract that can be accessed externally. The CALL method is declared by the keyword CALL. The definition rule of the CALL method is:

  - The first parameter type must be the structure CallParams, which contains three parameters that need to be initialized: address, vnt, and gas
  - The second (if there are parameters) and subsequent parameters are the parameters of the externally accessible function in the called contract
  - The return value is the return value of the externally accessible function in the called contract
```c
    //contract a, used to be called
    MUTABLE
    uint32 test(int32 var1,string var2){
        ...
    }
```
- Example of contract b, call contract a
```c
  //Declare CALL
  CALL uint32 test(CallParams params,int32 var1,string var2);
  MUTABLE
  uint32 testcall(){
      CallParams prams = {Address("0xaaaa"), U256(10000), 100000};
      uint32 res = test(prams, 1, "string");
      ...
  }
```
#### Fallback function

- Fallback is an optional function. If the contract implements the Fallback function, when the Input Data of the executed contract is incorrect or empty, wavm will enter the Fallback function to execute. The Fallback function has no parameters and no return value.

- Declaration of Fallback function
```c
  _(){ //unpayable fallback
      ...
  }

  $_(){ //payable fallback
    ...
  }
```
note:
- When both payable fallback and unpayable fallback appear in a contract, only unpayable fallback will be executed

## Appendix: Types and Standard Library
1. Basic types
- Boolean type
bool: two constants of true and false. Supported operators:
```c
  ! (Logical no)
  && (logical AND)
  || (logical OR)
  == (equal)
  != (not waiting)
```
2. Integer type

int32 / uint32 / int64 / uint64

Supported operators:

Comparison: <=, <, ==, !=, >=,> (equivalent to BOOL)
Bit operations: &, |, ^ (bitwise AND, bitwise OR, bitwise exclusive OR)
Arithmetic operators: +, -, *, /, %, <<, >>
Large number type
uint256: uint256 type constant needs the keyword U256 to indicate

int256: to be implemented
```c
uint256 u = U256(1000000000000000000);
```
Supported methods:
```c
uint256 U256_Add(uint256 x, uint256 y) // addition
uint256 U256_Sub(uint256 x, uint256 y) // subtraction
uint256 U256_Mul(uint256 x, uint256 y) // multiplication
uint256 U256_Div(uint256 x, uint256 y) // division
uint256 U256_Pow(uint256 x, uint256 y) // power operation
int32 U256_Cmp(uint256 x, uint256 y) // Comparison operation
```
3. String type
string: string type must be represented by "" double quotation marks

string str = "string";
Address type
address: address type is a special string with a byte length of 20, which needs to be represented by the keyword Address

address addr = Address("0xaaa");

Two, type conversion
from\to int32 int64 uint32 uint64 string address uint256 bool
int32 (int64)x (uint32)x (uint64)x FromI64(x) U256FromI64(x) (bool)x
int64 (int32)x (uint32)x (uint64)x FromI64(x) U256FromI64(x) (bool)x
uint32 (int32)x (int64)x (uint64)x FromU64(x) U256FromU64(x) (bool)x
uint64 (int32)x (int64)x (uint32)x FromU64(x) U256FromU64(x) (bool)x
string ToI64(x) ToI64(x) ToU64(x) Tou64(x) AddressFrom(x) U256From(x)
address
uint256 x
bool (int32)x (int64)x (uint32)x (uint64)x FromI64(x) U256FromI64(x)
Three, the standard library method

4. Method name Method description Parameter type Return value type Gas consumption
GetSender Get the address of the transaction initiator address 2 + 40
GetOrigin Get the original initiator address of the transaction address 2 + 40
GetValue Get the transfer value that occurred at the same time when the contract was created and called uint256 2 + 64
GetBalanceFromAddress Get the native token balance of an address address addr uint256 2 + 64
GetContractAddress Get the address of the currently executing contract address 2 + 40
GetBlockHash obtains the block hash according to the block height, only the latest 256 blocks can be obtained, excluding the current block uint64 blocknumber string 20 + 64
GetBlockNumber Get block height uint64 2
GetTimestamp Get the timestamp of block generation uint64 2
GetBlockProduser Get the address of the block producer address 2 + 40
SHA3 ​​SHA3 encryption operation string data string 30 + 6 * size of string data

Ecrecover recovers the account address from the signed hash string hash, string v, string r, string s string 3000 + 40
GetGas Get the remaining GAS uint64 2
GetGasLimit Get the GasLimit of the current transaction uint64 2
Assert judges the condition and returns msg if it fails. The transaction fails and consumes all gas. This function is usually used for debugging. For production environment, please use require bool condition, string msg void true: 2; false: all gas
Revert rolls back the transaction and interrupts the contract operation string msg void 2 + 2 * size of string data
Require Judge whether the condition is established, if it fails, the transaction will fail bool condition, string msg void true: 0; false: 2 + 2 * size of string data
SendFromContract contract transfers money to addr, the transfer amount is amount, if the transfer fails, it will revert (address addr, uint256 amount) void 2300
TransferFromContract contract transfers money to addr, the transfer amount is amount, and the transfer fails to return false (address addr, uint256 amount) bool 2300
FromI64 converts the value of int64 into a string int64 value string 2
FromU64 converts the value of uint64 into a string uint64 value string 2
ToI64 converts a string to int64 string value int64 2
ToU64 converts a string to uint64 string value uint64 2
Concat connects two strings string str1, string str2 string 2 * (size of str1 and str2)
