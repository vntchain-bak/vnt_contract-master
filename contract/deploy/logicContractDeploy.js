var fs = require('fs');
var Vnt = require('vnt');
var vntkit = require('vnt-kit');
var TX = require('ethereumjs-tx');
var vnt = new Vnt();


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

const dataContractAddress = "0x431efa70fd152855c31eeca24b055d5b591d9ca2";

var codeFile =
    '../logicOutput/$DiceLogic.compress';
var abiFile =
    '../logicOutput/$DiceLogic.abi';
var wasmabi = fs.readFileSync(abiFile);
var abi = JSON.parse(wasmabi.toString('utf-8'));

function deployWasmContract() {
  // Use abi to initialize a contract object and load the code file
  var contract = vnt.core.contract(abi).codeFile(codeFile)

  // Deploy contract
  // Here we don’t need an explicit signature, the vntchain node will sign for us, and the contract can be deployed using a package-friendly new interface
  var contractReturned = contract.new(100000000000, "VNT Token", "VNT", {
      from: from1, //The account corresponding to the from parameter will be used as a transaction signature
      data: contract.code, //pass the contract code as data
      gas: 4000000
  }, function(err, myContract){
     if(!err) {
        if(!myContract.address) {
            console.log("transactionHash: ", myContract.transactionHash)
        } else {
            console.log("contract address: ", myContract.address)
        }
     } else {
       console.log('error deploy contract as: ');
       console.log(err)
     }
   });
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



// ========== new from test net
deployWasmContract()
// GetAmountNew('0x02f8d9c9bb81b3a81bf13d4ec8818be5918d3658')
