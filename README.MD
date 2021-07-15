# Privacy-awarded Covid-19 tracker

This repo contains a proof of concept project made for the TON contest:  [#18 Groth16 zkSNARK Proof Verification Use Cases](https://devex.gov.freeton.org/proposal?proposalAddress=0%3Ae6b65075478e7d412fdb0870452f30dfa8bf51272e28a3167abc5c5df6fd051d).

We care about privacy and allow some instances (like governance) to ensure that a person wasn't be in an infected by COVID-19 area, by generation proof that proves User is outside of the specified area. So Governance can control spreading of a virus without tracking an exact location of all Users.

## Idea

**The idea is to prove that you weren't in some specific area on the map without revealing your actual location.**

To do so, a prototype was made: zkSNARK that accepts your coordinates (as personal input) and generates proof that they are outside of an area defined by min and max coordinates (as auxiliary input).

## Demonstration

![demonstration](demonstration.gif)


### Steps

1. **Gov** create a solidity contract with snark that can validate coordinates and get the answer, is the User placed in the specified area
2. ** User** generates proof on his device and send it to the contract

After this:

1. ✅ User can  confirm that he is in a safe area
2. ✅ User location data doesn't send outside of his device, so we preserve privacy

## Assumptions

1. Let assume that we have a particular device that can securely generate, encrypt and transfer your coordinates via the encrypted protocol.

Then you can run this code on this device to generate proof that the owner of this device is based in some specific area (safe place).

2. Personal ID and Time assumption: we can check on the blockchain part to make zksnark simple as possible. So let's assume that a personal tracker can log it into a smart contract on its side.


## Implementation:

A SNARK checks that at least one of this constrains is true, using a comparison components from a blueprint library:
```
userLatitude <= minLatitude
maxLatitude <= userLatitude
userLongtitude <= minLongtitude
maxLongtitude <= userLongtitude
```

## Architecture:

### ZKP (blueprint)

Generates proof by given user coordinates and min/max coordinates of a restricted area.

### TON Blockchain (smart contract)

Validates proof on-chain using VERGRTH16 TVM instruction.

### Demo web app

This web application interactivly generate and validate proofs for a given user location and restricted area coordinates.
* Proofs are generating by a node backend with compiled blueprint cli tool and pregenerated proving key;
* Proofs are validating on chain using `verification.sol` smart contract from a repo, deployed at `0:e13752c9dc987ca1e33a012511409b273ea06af68e799c24f3cee861fc9815aa` address at NIL test network;
* A verification key is stored in the smart contract, and can be changed using `setVerificationKey` method

## How to build and use:

### ZKP cli tool

1. Clone:
```
git clone  https://github.com/idealatom/ton-zkp-contest --recursive
```

2. Go to build directory:
```
cd ton-zkp-contest/zkp && mkdir build && cd build
```

3. Build
```
cmake .. && make cli
```

4. Generate proving and verification keys:
```
> bin/cli setup
Blueprint size: 284
Generating constraint system...
Number of R1CS constraints: 287
Generating keypair...
Saving proving key to a file "proving.key"
Saving verification key to a file "verification.key"
```

5. Generate the proof for given coordinates:
```
> bin/cli prove --posLat 13.686019 --minLat 13.673677 --maxLat 13.697777 --posLng 100.564981 --minLng 100.523192 --maxLng 100.551189
Loading proving key from a file "proving.key"
Generating constraint system...
Generating witness...
Blueprint is satisfied: 1
Generating proof...
Saving proof to file "proof"
Saving primary input to file "primary.input"
```

6. Verify proof file using saved primary input and verification key:
```
> bin/cli verify
Loading proof from a file "proof"
Loading primary input from a file "verification.key"
Loading verification key from a file "primary.input"
Verification status: 1
```

### Web app backend

```
> cd /backend/
> npm install
> npm run dev
```

### Web app frontend

```
> cd /frontend/
> yarn install
> yarn start
```
Next open [localhost:8000](localhost:8000) in your browser.


## Testing

Build tests:
```
> cd /zkp/test/
> cmake .. -DBUILD_TESTS=1
> make circuit_test
```
Run tests
```
> test/circuit_test
```
