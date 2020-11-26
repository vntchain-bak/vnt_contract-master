# How to build a Hubble development and test network locally

## Get started
* Before you start, make sure golang was install on your pc/server. For ubuntu os, the following environments can be set:
  ```sh
  export GOROOT=/usr/local/go
  export GOPATH=$HOME/go
  export PATH=$PATH:/usr/local/go/bin
  export PATH=$PATH:$GOPATH/bin
  source ~/.profile
  ```
* Also, make sure you install the [`vnt tools`](https://github.com/vntchain/go-vnt)

This article teaches you how to build a Hubble development test network locally, and takes the development and test network of 4 Hubble witness nodes on a single machine as an example.

The VNT blockchain network needs to send transactions to elect witness nodes. If there is no initial witness, no block will be generated, the election transaction cannot be packaged into the block, and the witness node cannot be elected, so an initial witness node is required. They are responsible for executing transactions and packaging blocks in place of witnesses before the mainnet is launched. However, after a given number of witnesses are registered, the rights to generate blocks and consensus will be transferred to the witness nodes.

The initial witness cannot be registered through the transaction. It is configured through the genesis block file. The genesis block configuration file contains the account address and p2p address of the initial witness.

Three pieces of data are required to build a test network:

Account of each node
P2p address of each node
Unified genesis block configuration file dpos.json for each node
Next, we first generate these 3 parts of data, and then use these 3 parts to build a test network.

VNT network topology of 4 witness nodes

  ![`4-witnesses-topology`](./assets/images/4nodesNetwork.png)
  [`Chinese Language`](https://hubscan.vnt.link/developer/network) 

  - Red node 0 is node 0, as the bootnode node, bootnode node will be introduced below, 
  - 1, 2, 3 are other witness nodes.

## Generate 3 parts of data
The process of generating these three parts is actually the process of creating founding witnesses and writing their information into the genesis block.

### Create 4 initial witness accounts
Create a test network file directory testnet,

cd ~
mkdir testnet
Use the gvnt account new command to create the account of node 0, and specify the data directory node0 for node 0. During the account creation process, you will be asked to enter the account password. You can directly press Enter, and the password is empty, that is, no password:

➜ gvnt account new --datadir node0


INFO [12-16|14:47:13] Maximum peer count VNT=25 LES=0 total=25
Your new account is locked with a password. Please give a password. Do not forget this password.
Passphrase:
Repeat passphrase:
Address: {f31a08c03e03e36214f737755c235e6eadc5502e}
"f31a08c03e03e36214f737755c235e6eadc5502e" is the address of the account. The account is stored in the node0/keystore directory. The account file under the keystore is the encrypted account private key and other information, which cannot be disclosed.

  - Save Acount addresses
    4c858b39c7730cef513b2eb0434295795cf342b1
    9bb59926a6cbac7b3ea479983fd8fa54cd89e98c
    6e7a1d7a5c265e098c5bfe672b378aa638dc44b8
    88a1493a2eb3358d8d298e53d8afa46609e8a8ab


  - passwords need to be saved if needed:

➜ ls node0/keystore
UTC--2018-12-16T04-42-33.665349000Z--f31a08c03e03e36214f737755c235e6eadc5502e
According to the above example, node1, node2, node3 can be generated as the accounts of nodes 1, 2, and 3, and then as long as --datadir node_dir is to operate on this node. Currently, the file structure in the testnet directory should be as follows:

➜ tree.
```sh
.
├── node0
│   └── keystore
│       └── UTC--2020-09-23T11-31-26.545196599Z--4c858b39c7730cef513b2eb0434295795cf342b1
├── node1
│   └── keystore
│       └── UTC--2020-09-23T11-31-46.713143403Z--9bb59926a6cbac7b3ea479983fd8fa54cd89e98c
├── node2
│   └── keystore
│       └── UTC--2020-09-23T11-32-16.287480538Z--6e7a1d7a5c265e098c5bfe672b378aa638dc44b8
├── node3
│   └── keystore
│       └── UTC--2020-09-23T11-32-39.249397942Z--88a1493a2eb3358d8d298e53d8afa46609e8a8ab

```

9 directories, 5 files

### Generate the p2p address of each node
The p2p address is the address for each node to establish a connection. We must first obtain the p2p address of each initial witness. When the node is first started, it will automatically generate the p2p address.

The p2p address contains the IP and port number. When we start the node, we need to specify the port number for communication between nodes. Node 0 to node 4 are assigned port numbers: 12340, 12341, 12342, 12343.

Use the following command to start node 0 and get the p2p address.
$ gvnt --datadir node0 --port 12340 console

-  Omit a lot of output
/ip4/127.0.0.1/tcp/12340/ipfs/1kHYbyYai6Ns9Kve7BGPqppg4iGEoC43qr1ecVnp8eaGaa1

- Using the same method, the p2p addresses of nodes 1, 2, and 3 can be obtained.
```sh
  gvnt --datadir node1 --port 12341 console
  gvnt --datadir node2 --port 12342 console
  gvnt --datadir node3 --port 12343 console

  /ip4/127.0.0.1/tcp/12341/ipfs/1kHBpENGoi69h1kSB3QyxshafAAQ96NzWd2HD2rFhre2mGk
  /ip4/127.0.0.1/tcp/12342/ipfs/1kHXty4MAZbdTTNPv4Lmpfwm2HaShdVTDQWJAzFbPWGj3Kp
  /ip4/127.0.0.1/tcp/12343/ipfs/1kHTMcsoVM3w4vbUL2fJYYP5a6vD1DFmCBin1jFwxuDmYyu
```

- View network connection amongst nodes. For example on node 3, issue admin.nodeInfo.vnode
> admin.nodeInfo.vnode
```sh
{
  id: "1kHVZToJpjDg9tWNzt7XECmf7ChLmZCL627negsi5TBZNc4",
  ip: "127.0.0.1",
  listenAddr: ":12340",
  name: "Gvnt/v0.6.4/linux-amd64/go1.12",
  ports: {
    discovery: 0,
    listener: 0
  },
  protocols: {},
  vnode: "/ip4/127.0.0.1/tcp/12340/ipfs/1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo"
}
{
  id: "1kHRKwLgG6zu7RwxWeFsSJpK2qYtcKPpNLjRd2wrCyCjKd8",
  ip: "127.0.0.1",
  listenAddr: ":12341",
  name: "Gvnt/v0.6.4/linux-amd64/go1.12",
  ports: {
    discovery: 0,
    listener: 0
  },
  protocols: {},
  vnode: "/ip4/127.0.0.1/tcp/12341/ipfs/1kHRPcSinoAPyzjbxzcqx9QdV4ioP24eDzigDQYg19jtsz4"
}
{
  id: "1kHfjnzJvrwLz4hPL6uDu3pjEifvK9GzU4npxu6caex8WWL",
  ip: "127.0.0.1",
  listenAddr: ":12342",
  name: "Gvnt/v0.6.4/linux-amd64/go1.12",
  ports: {
    discovery: 0,
    listener: 0
  },
  protocols: {},
  vnode: "/ip4/127.0.0.1/tcp/12342/ipfs/1kHQtrwqTEGszpMEgB5eNR9d5YadgtV6FPe75m2YtiVCKt2"
}
{
  id: "1kHU8Jquj1wqp2Nqnok3jiesg6vPqtuKrcxXjbYrsh1KQ4R",
  ip: "127.0.0.1",
  listenAddr: ":12343",
  name: "Gvnt/v0.6.4/linux-amd64/go1.12",
  ports: {
    discovery: 0,
    listener: 0
  },
  protocols: {},
  vnode: "/ip4/127.0.0.1/tcp/12343/ipfs/1kHG6WgboqNQ9qoAhGUUTDhMnga64jNVVLRdYzRWbN2g5pm"
}
```

Use control-D to close the client.

You can see that there are many more directories and files under node0. The p2p address is saved in the vntdb directory. The witness node should back up the vntdb directory, otherwise the node restarts and a new p2p address will be generated, and the witness needs to be re-registered. As the initial witness node, you should back up this directory, because once the address of the initial witness is written into the genesis block configuration file, it cannot be modified.
$ ls node0
  - output:
    gvnt history keystore vntdb


- Remove the temporary data gvnt directory of node 0:

$ rm -rf node0/gvnt

- You can use the above command to clean up the temporary data of nodes 1, 2, and 3, otherwise it will fail when using the configuration file to initialize the node.

### Create the genesis block configuration file dpos.json
We need to use the account and p2p address obtained in the previous 2 steps to create the genesis block configuration file dpos.json for the test network. The genesis block file can be modified based on the file go-vnt/genesis_dpos.json.

- dpos.json needs to be modified

  + Block period
  + Number of witness nodes
  + The account and p2p address of the initial witness node (as generated above)
    cfa0cc407430b95d01dc0553a29e6ec85fed330b
    c56998f3f3d04a51e7720000883cc1a48fe7f386
    cc669462babe1296906d07c551e094f5cc625f38
    c342a6b62dad572f8dc27a441cc9be992891565f

- This is the content of go-vnt/genesis_dpos.json:
```sh
  {
    "config": {
      "chainId": 1012,
      "dpos": {
        "period": 2,
        "witnessesNum": 4,
        "witnessesUrl": [
          "/ip4/127.0.0.1/tcp/12340/ipfs/1kHYbyYai6Ns9Kve7BGPqppg4iGEoC43qr1ecVnp8eaGaa1",
          "/ip4/127.0.0.1/tcp/12341/ipfs/1kHBpENGoi69h1kSB3QyxshafAAQ96NzWd2HD2rFhre2mGk",
          "/ip4/127.0.0.1/tcp/12342/ipfs/1kHXty4MAZbdTTNPv4Lmpfwm2HaShdVTDQWJAzFbPWGj3Kp",
          "/ip4/127.0.0.1/tcp/12343/ipfs/1kHTMcsoVM3w4vbUL2fJYYP5a6vD1DFmCBin1jFwxuDmYyu"
        ]
      }
    },
    "timestamp": "0x5F6B2F0E",
    "extraData": "0x",
    "gasLimit": "0x47b760",
    "difficulty": "0x1",
    "coinbase": "0x0000000000000000000000000000000000000000",
    "alloc": {
      "0xf3830b235b2110a177d30e53e313b093f0f0a370": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0x460018c250d6fc6e2f4a57e1a34c7e36438cb55a": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0xa263a3e0d28979c6fb380f0a47ed5b19a89963ef": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      }
    },
    "witnesses": [
      "0x4c858b39c7730cef513b2eb0434295795cf342b1",
      "0x9bb59926a6cbac7b3ea479983fd8fa54cd89e98c",
      "0x6e7a1d7a5c265e098c5bfe672b378aa638dc44b8",
      "0x88a1493a2eb3358d8d298e53d8afa46609e8a8ab"
    ],
    "number": "0x0",
    "gasUsed": "0x0",
    "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000"
  }
```

- "dpos": Contains the relevant configuration of dpos: the number of witnesses, the production period, and the initial witness address.

- "witnesses": is the account of the initial witness.

- "chainId": is the ID of the network we created, which is needed when starting the node.

Modify the above data on this basis, you can replace it with the configuration of our private network, the cycle and the number of witnesses remain unchanged, just modify the initial witness p2p address and account number, and then save the modified file to the testnet directory and name it For dpos.json:
```sh
  {
    "config": {
      "chainId": 1012,
      "dpos": {
        "period": 2,
        "witnessesNum": 4,
        "witnessesUrl": [
          "/ip4/127.0.0.1/tcp/12340/ipfs/1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo",
          "/ip4/127.0.0.1/tcp/12341/ipfs/1kHJFKr2bzUnMr1NbeyYbYJa3RXT18cEu7cNDrHWjg8XYKB",
          "/ip4/127.0.0.1/tcp/12342/ipfs/1kHfop9dnUHHmtBXVkLB5UauAmACtrsEX5H5t6oCRpdL198",
          "/ip4/127.0.0.1/tcp/12343/ipfs/1kHHWuQNUVV2wgE8SqzQjWhiFQcfpkP5tRVTdJXAPWVj4nR"
        ]
      }
    },
    "nonce": "0x0",
    "timestamp": "0x5b45b949",
    "extraData": "0x",
    "gasLimit": "0x47b760",
    "difficulty": "0x1",
    "mixHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
    "coinbase": "0x0000000000000000000000000000000000000000",
    "alloc": {
      "0x122369f04f32269598789998de33e3d56e2c507a": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0x3dcf0b3787c31b2bdf62d5bc9128a79c2bb18829": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0x42a875ac43f2b4e6d17f54d288071f5952bf8911": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0xbf66d398226f200467cd27b14e85b25a8c232384": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0x491f4e8d914e30b1a5e8c804789094fe30971807": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      },
      "0xe23f3ed4b6969f29284f667c16761212678c917d": {
        "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
      }
    },
    "witnesses": [
      "0xf31a08c03e03e36214f737755c235e6eadc5502e",
      "0x9689e062952b71b825cd9dfc1d1d01a6319c6ebc",
      "0x54604da2bad12b66e9aef6b8c04629b68771778e",
      "0xe3d4f3e7d1b82dcc210efe1e0666b45e5a619a2d"
    ],
    "number": "0x0",
    "gasUsed": "0x0",
    "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000"
  }
```
After the above operations, the testnet directory should be as follows:
```sh
  testnet tree. -L 2
  .
  ├── dpos.json
  ├── node0
  │ ├── history
  │ ├── keystore
  │ └── vntdb
  ├── node1
  │ ├── history
  │ ├── keystore
  │ └── vntdb
  ├── node2
  │ ├── history
  │ ├── keystore
  │ └── vntdb
  └── node3
      ├── history
      ├── keystore
      └── vntdb
```
16 directories, 5 files
If it is not the same, please read the above content again to see which step generated the missing file and execute the command to generate it.

### build the Test network
Use data and files to build a testnet
Use dpos.json to initialize each node
You need to use the dpos.json generated above to initialize the nodes of the test network, otherwise any node will not be able to join the network due to inconsistent genesis block information.

Use the following command to initialize all initial witness nodes, taking node 0 as an example.

$ gvnt init dpos.json --datadir node0

INFO [12-16|15:29:36] Maximum peer count VNT=25 LES=0 total=25
INFO [12-16|15:29:36] Allocated cache and file handles database=/Users/shitaibin/Workspace/testnet/node0/gvnt/chaindata cache=16 handles=16
INFO [12-16|15:29:36] Writing custom genesis block
INFO [12-16|15:29:36] Persisted trie from memory database nodes=9 size=1.44kB time=105.62µs gcnodes=0 gcsize=0.00B gctime=0s livenodes=1 livesize=0.00B
INFO [12-16|15:29:36] Successfully wrote genesis state database=chaindata hash=6104e6…838356
INFO [12-16|15:29:36] Allocated cache and file handles database=/Users/shitaibin/Workspace/testnet/node0/gvnt/lightchaindata cache=16 handles=16
INFO [12-16|15:29:36] Writing custom genesis block
INFO [12-16|15:29:36] Persisted trie from memory database nodes=9 size=1.44kB time=64.704µs gcnodes=0 gcsize=0.00B gctime=0s livenodes=1 livesize=0.00B
INFO [12-16|15:29:36] Successfully wrote genesis state database=lightchaindata hash=6104e6…838356

### Start the bootnode node
The VNT network needs to be automatically established using bootnode. One of the initial witness nodes can be used as the bootnode. When other nodes establish connections with this node, they will establish connections with the remaining nodes.

gvnt --networkid 1012 --datadir node0 --port 12340 console
- To be able to generate blocks, sync and --produce option must be provided
$ gvnt --networkid 1012 --datadir node0 --port 12340 --verbosity 4 --syncmode full --produce console
- The account also needs to be unlock
personal.unlockAccount(core.coinbase, "PasswordIfAny", 3153600000)

- If you want to attach rpc, rpcadd, rpcport, rpcapi need to be provided
gvnt --networkid 1012 --datadir node0 --port 12340 --verbosity 4 --syncmode full --produce --rpc --rpcaddr 0.0.0.0 --rpcport 8880 --rpcapi="db,core,net,vnt,personal" console
gvnt --networkid 2 --datadir . --port 3009 --vntbootnode "/ip4/101.37.164.86/tcp/3002/ipfs/1kHkLvCyGX5R33XNr7vmboyYe3etLDs1s9t8odPBZyBwNyB" --syncmode full --rpc --rpcaddr 0.0.0.0 --rpcport 8880 --rpcapi="db,core,net,vnt,personal" console

### Start the remaining initial witness nodes

The startup command of node 1 is as follows. Compared with the command to generate p2p address, you need to specify the p2p address of --vntbootnode, that is, the p2p address of node 0. Use admin.peers to view the nodes that are connected to this node, where <peer.ID 1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo> is the p2p address of node 0, indicating that they have established a connection.

gvnt --networkid 1012 --datadir node1 --port 12341 --vntbootnode "/ip4/127.0.0.1/tcp/12340/ipfs/1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo" --verbosity 4 --syncmode full --produce console
personal.unlockAccount(core.coinbase, "Test@2020", 3153600000)
<!-- adding rpc -->


<!-- 0xcf632f1f41cda44fe9017e175148b8727a22e590 -->

// Omit to start printing
> admin.peers
```sh
[{
    caps: null,
    id: "<peer.ID 1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo>",
    name: "",
    network: {
      inbound: false,
      localAddress: "/ip4/192.168.0.104/tcp/12341",
      remoteAddress: "/ip4/192.168.0.104/tcp/12340",
      static: false,
      trusted: false
    },
    protocols: null
}]
```
Start node 2:

gvnt --networkid 1012 --datadir node2 --port 12342 --vntbootnode "/ip4/127.0.0.1/tcp/12340/ipfs/1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo" --verbosity 4 --syncmode full --produce  console

Start node 3:
<!-- This is the node used for applications, need to attached the rpc -->
gvnt --networkid 1012 --datadir node3 --port 12343 --vntbootnode "/ip4/127.0.0.1/tcp/12340/ipfs/1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo" --verbosity 4 --syncmode full --produce --rpc --rpcaddr 0.0.0.0 --rpcport 8880 --rpcapi="db,core,net,vnt,personal" console
personal.unlockAccount(core.coinbase, "Test@2020", 3153600000)
The peer information on node 3 is as follows. There will be 3 nodes on each node, indicating that our test network has been set up:

> admin.peers
> admin.peers
```sh
[{
    caps: null,
    id: "<peer.ID 1kHJFKr2bzUnMr1NbeyYbYJa3RXT18cEu7cNDrHWjg8XYKB>",
    name: "",
    network: {
      inbound: false,
      localAddress: "/ip4/127.0.0.1/tcp/12343",
      remoteAddress: "/ip4/127.0.0.1/tcp/12341",
      static: false,
      trusted: false
    },
    protocols: null
}, {
    caps: null,
    id: "<peer.ID 1kHcch6yuBCgC5nPPSK3Yp7Es4c4eenxAeK167pYwUvNjRo>",
    name: "",
    network: {
      inbound: false,
      localAddress: "/ip4/127.0.0.1/tcp/12343",
      remoteAddress: "/ip4/127.0.0.1/tcp/12340",
      static: false,
      trusted: false
    },
    protocols: null
}, {
    caps: null,
    id: "<peer.ID 1kHfop9dnUHHmtBXVkLB5UauAmACtrsEX5H5t6oCRpdL198>",
    name: "",
    network: {
      inbound: false,
      localAddress: "/ip4/127.0.0.1/tcp/12343",
      remoteAddress: "/ip4/127.0.0.1/tcp/12342",
      static: false,
      trusted: false
    },
    protocols: null
}]
```
Package block
Unlock the account and open the consensus on each initial witness node, and you can see that the block is continuously generated.

personal.unlockAccount(core.coinbase, "Test@2020", 3153600000) // 3153600000 refers to the number of seconds to unlock, after this period of time, the account will be automatically locked
bp.start()
Above, the development and test network is set up.

personal.unlockAccount("0xf3830b235b2110a177d30e53e313b093f0f0a370", "Test@2020", 3153600000)

personal.unlockAccount("0x460018c250d6fc6e2f4a57e1a34c7e36438cb55a", "Test@2020", 3153600000)

personal.unlockAccount("0xa263a3e0d28979c6fb380f0a47ed5b19a89963ef", "Test@2020", 3153600000)

"alloc": {
    "0xf3830b235b2110a177d30e53e313b093f0f0a370": {
      "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
    },
    "0x460018c250d6fc6e2f4a57e1a34c7e36438cb55a": {
      "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
    },
    "0xa263a3e0d28979c6fb380f0a47ed5b19a89963ef": {
      "balance": "0x200000000000000000000000000000000000000000000000000000000000000"
    }
  },



  ## Troubleshooting the network
  1. Network
  - check node are connected:
    $ admin.peers

  2. Check if blocks are created
  - core.blockNumber


  3. running with log to troubleshooting
  --verbosity 4   //start node with this option

  4. Node our of turn error (warnings)
  - check if start node from address map with the address in genesis.json


  