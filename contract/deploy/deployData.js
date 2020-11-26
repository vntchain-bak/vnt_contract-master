var fs = require('fs'); 
var Vnt = require('vnt');
var vntkit = require('vnt-kit');
var TX = require('ethereumjs-tx');
var vnt = new Vnt();
const util = require('util');


// var privider = 'http://47.102.157.204:8880';
var privider = 'http://127.0.0.1:8880';
var chainid = 2;

vnt.setProvider(new vnt.providers.HttpProvider(privider));


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

vnt.personal.unlockAccount(from1, pass1);
vnt.personal.unlockAccount(from2, pass2);



var codeFile =
    '../dataOutput/$DiceData.compress';
var abiFile =
    '../dataOutput/$DiceData.abi';
var wasmabi = fs.readFileSync(abiFile);
var abi = JSON.parse(wasmabi.toString('utf-8'));

const dataDeployConf = require('./static/dataContract.json');
const dataContractAddress = dataDeployConf.address;

const logicDeployConf = require('./static/logicContract.json');
const logicContractAddress = logicDeployConf.address;

function deployWasmContract() {
  // Use abi to initialize a contract object and load the code file
  var contract = vnt.core.contract(abi).codeFile(codeFile)

  // Deploy contract
  // Here we don’t need an explicit signature, the vntchain node will sign for us, and the contract can be deployed using a package-friendly new interface
  var contractReturned = contract.new(1000000000, "bitcoin", "BTC", {
      from: from1, //The account corresponding to the from parameter will be used as a transaction signature
      data: contract.code, //pass the contract code as data
      gas: 4000000
  }, function(err, myContract){
     if(!err) {
        if(!myContract.address) {
            console.log("transactionHash: ", myContract.transactionHash)
        } else {
            console.log("contract address: ", myContract.address)
            
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
              fs.writeFileSync("./static/dataContract.json", confString);
              console.log("Write data Contract deployment result to file ...");
          }
        }
     } else {
       console.log('error deploy contract as: ');
       console.log(err)
     }
   });
}

function setLogicContractAddress(_logicContradtAddress) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = contract.setLogicContractAddress.estimateGas(_logicContradtAddress, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = contract.setLogicContractAddress.sendTransaction(
    _logicContradtAddress,{from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
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

async function getLogicContractAddress() {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  const estimateGas = await contract.getLogicContractAddress.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getLogicContractAddress.call({
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function setTotalGameCount(_gameCount) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  // var data = contract.packFunctionData("setDataContractAddress", [_dataContractAddress]);
  // var account = vntkit.account.decrypt(from1Keystore, pass1, false);
  // var account = vnt.personal.unlockAccount(from2, pass2);
  const estimateGas = contract.SetTotalGameCount.estimateGas(_gameCount, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = contract.SetTotalGameCount.sendTransaction(
    _gameCount,{from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend with : SetTotalGameCount", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
  // sendRawTransaction(account, contractAddress, data, vnt.toHex(0))

}

async function GetTotalGameCount() {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  const estimateGas = await contract.getTotalGameCount.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getTotalGameCount.call({
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}


// set and get total Game count
async function addDeposit(_from, _depositAmount) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.addDeposit.estimateGas(_from, _depositAmount, {
    from: from1,
  });
  contract.addDeposit.sendTransaction(
    _from, _depositAmount,{from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
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


async function getBalanceOf(_from) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  const estimateGas = await contract.getBalanceOf.estimateGas(_from, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getBalanceOf.call(_from, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function checkAmountOf(_from, _amount) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.checkAmountOf.estimateGas(_from, _amount, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.checkAmountOf.call(_from, _amount, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function calculateReward(_from, _amount) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.getReward.estimateGas(_amount, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getReward.call(_amount, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function checkPool(_from, _amount) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.checkPool.estimateGas(_amount, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.checkPool.call(_amount, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function getBalanceOfContract(_from) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.getBalanceOfContract.estimateGas({
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getBalanceOfContract.call({
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function depositToContract(_from){
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.$Deposit.estimateGas({
    from: from1,
  });
  contract.$Deposit.sendTransaction(
    {from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
}

async function depositToPool(_from){
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log("instance of contract is: ", util.inspect(contract));
  const estimateGas = await contract.$DepositPool.estimateGas({
    from: from1,
  });
  contract.$DepositPool.sendTransaction(
    {from: from1, gas: Math.floor(estimateGas * 1.5)}, function(err, txid) {
        if(err) {
            console.log("error happend: ", err)
        } else {
            getTransactionReceipt(txid, function(receipt) {
                console.log("status: ", receipt.status)
                console.log('receipt transaction: ', util.inspect(receipt));
            })
        }
    })
}

async function getNickNameOf(_from) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.getNickNameOf.estimateGas(_from, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getNickNameOf.call(_from, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
}

async function getRewardOf(_from) {
  var contract = vnt.core.contract(abi).at(dataContractAddress);
  console.log('data contract: ', util.inspect(contract));
  const estimateGas = await contract.getRewardOf.estimateGas(_from, {
      from: from1,
  });
  console.log('gas estimated: ', estimateGas);
  const res = await contract.getRewardOf.call(_from, {
    from: from1,
    gas: Math.floor(estimateGas * 1.5)
  })
  console.log(res);
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

// ../../bottle/build/bin/bottle compile -code diceData.c -output dataOutput
// scp -i ~/.ssh/id_rsa -r ./dataOutput ubuntu@136.186.108.56:~/vnt-contract-demo/contract

// deployWasmContractWithPrivateKey();

// var contractAddress = '0xda8110f8fac5e486dc38adc41be0e65a91659fb5';
// GetPool();
// GetAmount();
// Deposit();
// Bet();
// TestRandom();



// ========== new from test net
// deployWasmContract()


// setLogicContractAddress(logicContractAddress)
// getLogicContractAddress()
// setTotalGameCount(1)
// GetTotalGameCount()

// addDeposit(from2,1000);
getBalanceOf(from2);

// checkAmountOf(from2, 1000);

// calculateReward(from2, 1000);
// depositToContract(from1);
// depositToPool(from1);
// getBalanceOfContract(from2);
// checkPool(from2, 0)

// getNickNameOf(from2);
// getRewardOf(from2);

module.exports = {
  setLogicContractAddress,
}