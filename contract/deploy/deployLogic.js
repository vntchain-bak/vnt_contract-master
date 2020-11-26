var fs = require('fs'); 
var Vnt = require('vnt');
var vntkit = require('vnt-kit');
var TX = require('ethereumjs-tx');
var vnt = new Vnt();
const util = require('util');
var async = require('async');


// var privider = 'http://47.102.157.204:8880';
var privider = 'http://127.0.0.1:8880';
var chainid = 2;

vnt.setProvider(new vnt.providers.HttpProvider(privider));

const {setLogicContractAddress} = require('./deployData');

console.log('vnt lib: ');
console.log(util.inspect(vnt));

// var from1 = '0x122369f04f32269598789998de33e3d56e2c507a';
// var from1Keystore = 'replace';
// var pass1 = '';
// var from2 = '0x3dcf0b3787c31b2bdf62d5bc9128a79c2bb18829';
// var from2Keystore = 'replace';
// var pass2 = '';
// var toAddr = '0x3ea7a559e44e8cabc362ca28b6211611467c76f3';

// need to apply for Ether
// https://hubscan.vnt.link/faucet
var from1 = '0xf3830b235b2110a177d30e53e313b093f0f0a370';
// var from1 = '0x02f8d9c9bb81b3a81bf13d4ec8818be5918d3658';
// 0x02f8d9c9bb81b3a81bf13d4ec8818be5918d3658
var from1Keystore = '/home/ubuntu/testnet/node3/keystore/UTC--2020-09-23T11-32-39.249397942Z--88a1493a2eb3358d8d298e53d8afa46609e8a8ab';
var pass1 = 'Test@2020';

var from2 = '0x460018c250d6fc6e2f4a57e1a34c7e36438cb55a';
var from2Keystore = '/home/ubuntu/testnet/node3/keystore/UTC--2020-09-23T11-32-39.249397942Z--460018c250d6fc6e2f4a57e1a34c7e36438cb55a';
var pass2 = 'Test@2020';

var from3 = '0xa263a3e0d28979c6fb380f0a47ed5b19a89963ef';
var from4 = '0x88a1493a2eb3358d8d298e53d8afa46609e8a8ab';

vnt.personal.unlockAccount(from1, pass1);
vnt.personal.unlockAccount(from2, pass2);



var codeFile =
    '../logicOutput/$DiceLogic.compress';
var abiFile =
    '../logicOutput/$DiceLogic.abi';
var wasmabi = fs.readFileSync(abiFile);
var abi = JSON.parse(wasmabi.toString('utf-8'));


const dataDeployConf = require('./static/dataContract.json');
const dataContractAddress = dataDeployConf.address;

const logicDeployConf = require('./static/logicContract.json');
const logicContractAddress = logicDeployConf.address;

async function deployWasmContract() {
  // Use abi to initialize a contract object and load the code file
  var contract = vnt.core.contract(abi).codeFile(codeFile);
  // const estimatedDeployGas = contract.deploy().estimateGas();
  // console.log('estimated deployed gas: ', estimatedDeployGas);

  // Deploy contract
  // Here we don’t need an explicit signature, the vntchain node will sign for us, and the contract can be deployed using a package-friendly new interface
  // get default from vnt: 400000, if more function, more gas
  var contractReturned = contract.new(1000000000000000, "VNT Token", "VNT", {
      from: from1, //The account corresponding to the from parameter will be used as a transaction signature
      data: contract.code, //pass the contract code as data
      gas: 4000000
  }, function(err, myContract){
     if(!err) {
        if(!myContract.address) {
            console.log("transactionHash: ", myContract.transactionHash)
        } else {
            console.log("contract address: ", myContract.address);
            const deployedConfig = {
              server: privider,
              administrator: from1,
              address: myContract.address,
              // acg721_address: newInstance721.options.address
          };
      
          const confString = JSON.stringify(deployedConfig);
          fs.writeFileSync("./static/logicContract.json", confString);
          console.log("Write Logic Contract deployment result to file ...");
          async.parallel({
            setDataContractAddressOnLogicContract: function(callback) {
              console.log('set data contract address on logic contract');
              setDataContractAddress(dataContractAddress, callback);
              // callback(null, null);
            },
            setLogicContractAddressOnDataContract: function(callback) {
              console.log('set logic contract address on data contract');
              setLogicContractAddress(myContract.address, callback);
              // callback(null, null);
            },
          },
          function(err, results) {
            if (err) {
              console.log('err with async serries: ', err)
            } else if (results) {
              console.log('asign data and logic contract address from one to another succesfully');
            }
          });
          // set this new Logic address to data contract
          // await setLogicContractAddress(myContract.address);
          // // asign data contract to this logic ocntract
          // await setDataContractAddress(dataContractAddress);
        }
     } else {
       console.log('error deploy contract as: ');
       console.log(err)
     }
   });
}

async function setDataContractAddress(_dataContractAddress) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setDataContractAddress.estimateGas(_dataContractAddress, {
    from: from1,
});
  contract.setDataContractAddress.sendTransaction(
    _dataContractAddress,{from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

function getDataContractAddress() {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = contract.getDataContractAddress.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = contract.getDataContractAddress.call({
    from: from1,
  })
  console.log(res);
  return res;
}

// set and get total Game count
async function setTotalGameCountFromDataCtr(_totalGameCount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setTotalGameCountFromDataCtr.estimateGas(_totalGameCount, {
    from: from1,
  });
  contract.setTotalGameCountFromDataCtr.sendTransaction(
    _totalGameCount,{from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

async function getTotalGameCountFromDataCtr() {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = await contract.getTotalGameCountFromDataCtr.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getTotalGameCountFromDataCtr.call({
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
  return res;
}

// set and get total Game count
async function claimFreeChip(_from) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.claimFreeChipsFrom.estimateGas({
    from: _from,
  });
  contract.claimFreeChipsFrom.sendTransaction(
    {from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}


// set and get total Game count
async function addDeposit(_from, _depositAmount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.depositFromSender.estimateGas(_depositAmount, {
    from: _from,
  });
  contract.depositFromSender.sendTransaction(
    _depositAmount,{from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

// checkAmountOnDataContract
async function checkAmountOnDataContract(_from, _amount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = await contract.checkAmount.estimateGas(_amount, {
      from: _from,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.checkAmount.call(_amount, {
    from: _from,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

// checkAmountOnDataContract
async function checkPoolOnDataContract(_from, _amount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = await contract.checkPoolOnDataContract.estimateGas(_amount, {
      from: _from,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.checkPoolOnDataContract.call(_amount, {
    from: _from,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

// set nickname for sender/player
async function setNickName(_from, _name) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setNickNameForSender.estimateGas(_name, {
    from: _from,
  });
  contract.setNickNameForSender.sendTransaction(
    _name,{from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

// set and get total Game count
async function getNickName(_from) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setNickNameForSender.estimateGas(_name, {
    from: _from,
  });
  contract.setNickNameForSender.call(
    {from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

// testing set reward on logic contract
async function setRewardForSender(_from, _amount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setRewardForSender.estimateGas(_amount, {
    from: _from,
  });
  contract.setRewardForSender.sendTransaction(
    _amount,{from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

// testing set reward on logic contract
async function setLoseForSender(_from, _amount) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.setLoseForSender.estimateGas(_amount, {
    from: _from,
  });
  contract.setLoseForSender.sendTransaction(
    _amount,{from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

// player place a bet
async function bet(_from, _amount, _guess) {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.bet.estimateGas(_amount, _guess, {
    from: _from,
  });
  contract.bet.sendTransaction(
    _amount, _guess,{from: _from, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}


function getTestNumber() {
  var contract = vnt.core.contract(abi).at(logicContractAddress);
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = contract.getDataContractAddress.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = contract.getDataContractAddress.call({
    from: from1,
  })
  console.log(res);
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}


function GetAmountNew(address) {
  console.log("get ammount of address: ", address)
  // Generate contract instance
  var contract = vnt.core.contract(abi).at(contractAddr)
  // Call the GetAmount method of the contract, pay attention to the use of call
  r = contract.GetAmount.call(address, {from: from1})
  console.log("result", r.toString())
}



function deployWasmContractWithPrivateKey() {
  var contract = vnt.core.contract(abi).codeFile(codeFile);

  var deployContract = contract.packContructorData();
  var value = vnt.toHex(vnt.toWei(100, 'vnt'))
  var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  
  sendRawTransaction(account, "0x0", deployContract, value)
}

function getTransactionReceipt(tx, cb) {
  var receipt = vnt.core.getTransactionReceipt(tx);
  if (!receipt) {
    setTimeout(function() {
      getTransactionReceipt(tx, cb)
    }, 1000);
  } else {
    cb(receipt)
  }
}

function GetPool() {
  var contract = vnt.core.contract(abi).at(contractAddress);
  r = contract.GetPool.call();
  console.log('result', r.toString());
}

function GetAmount() {
  var contract = vnt.core.contract(abi).at(contractAddress);
  var amount = contract.GetAmount.call({from: from2})
  console.log('amount', amount.toString());
}

function Deposit(){
  var contract = vnt.core.contract(abi);
  var data = contract.packFunctionData("$Deposit");
  var account = vntkit.account.decrypt(from2Keystore, pass2, false);

  sendRawTransaction(account, contractAddress, data, vnt.toHex(10))
}


function Bet() {
  var contract = vnt.core.contract(abi);
  var data = contract.packFunctionData("Bet", [vnt.toWei(10), 1]);
  var account = vntkit.account.decrypt(from2Keystore, pass2, false);

  sendRawTransaction(account, contractAddress, data, vnt.toHex(0))
}

function TestRandom() {
  var contract = vnt.core.contract(abi).at(contractAddress);
  var r = contract.testRandom.call({from: from1});
  console.log('random', r.toString());
}

function sendRawTransaction(account,to,data,value){
  var nonce = vnt.core.getTransactionCount(account.address);
  var options = {
    nonce: nonce,
    to: to,
    gasPrice: vnt.toHex(vnt.toWei(18, 'Gwei')),
    gasLimit: vnt.toHex(4000000),
    data: data,
    value: value,
    chainId: chainid
  };
  var tx = new TX(options);
  tx.sign(new Buffer(
    account.privateKey.substring(
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
      }
    });
}


// deployWasmContractWithPrivateKey();

// var contractAddress = '0xda8110f8fac5e486dc38adc41be0e65a91659fb5';
// GetPool();
// GetAmount();
// Deposit();
// Bet();
// TestRandom();


// ../../bottle/build/bin/bottle compile -code diceLogic.c -output logicOutput
// scp -i ~/.ssh/id_rsa -r ./logicOutput ubuntu@136.186.108.56:~/vnt-contract-demo/contract

// ========== new from test net
// deployWasmContract()


// setDataContractAddress(dataContractAddress);
// setLogicContractAddress(logicContractAddress);
// getDataContractAddress() 

// setTotalGameCountFromDataCtr(10);
// getTotalGameCountFromDataCtr()

// claimFreeChip(from2);
// addDeposit(from2, 100000000000000000000);

// checkAmountOnDataContract(from2, 1000);
// checkPoolOnDataContract(from2, 1000);  //not done yet as token has not been transfer successfully
// setNickName(from2, 'From Address 02');

// setRewardForSender(from2, '1000');
// setLoseForSender(from2, 500);
bet(from2, 500, 1)