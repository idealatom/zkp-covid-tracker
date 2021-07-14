# Privacy-awarded Covid-19 tracker

This repo contains  a POC project made for the TON contest:  [#18 Groth16 zkSNARK Proof Verification Use Cases](https://devex.gov.freeton.org/proposal?proposalAddress=0%3Ae6b65075478e7d412fdb0870452f30dfa8bf51272e28a3167abc5c5df6fd051d)



We care about privacy and allow some instances (like governance) to ensure that person wasn't placed in infected by covid-19 area, by generation proof that helps to check, that User out of the specified area.

## Idea

**The idea is to prove that you weren't in some specific area on the map without revealing your actual location.**

To do so, we made a prototype: zkSNARK that accepts your coordinates (as personal input) and generates proof that you were in the range of `r`  meters from a specific area: coordinates.


### Steps

1. **Gov** create a solidity contract with snark that can validate coordinates and get the answer, is the User placed in the specified area
2. ** User** generates proof on his device and send it to the contract

After this:

1. ✅ User can  confirm that he is in a safe area
2. ✅ User location data doesn't send outside of his device, so we preserve privacy

## Assumptions



1. Let assume that we have a particular device that can securely generate, encrypt and transfer your coordinates via the encrypted protocol.

Then you can run this code on this device to generate proof that the owner of this device is based in some specific area (safe place).



2. Let's assume that the world is flat - otherwise, we have to use [Haversine formula](https://en.wikipedia.org/wiki/Haversine_formula) that possible, but not in timeframe of the contest :) 

3. Personal ID and Time assumption: we can check on the blockchain part to make zksnark simple as possible. So let's assume that a personal tracker can log it into a smart contract on its side.



## Implementation:



We did a zksnark to set a radius and coordinates and made a zkSNARK that calculate the distance by a simple formula:

`distance = √ a2 + b2 `



calculated as follwoing:

```c++
				bp.val(deltaX) = bp.val(userX) - bp.val(areaX);
        bp.val(deltaY) = bp.val(userY) - bp.val(areaY);
        bp.val(deltaX_square) = bp.val(deltaX) * bp.val(deltaX);
        bp.val(deltaY_square) = bp.val(deltaY) * bp.val(deltaY);
        bp.val(deltaSum) = bp.val(deltaX_square) + bp.val(deltaY_square);
        bp.val(radius_square) = bp.val(radius) * bp.val(radius);
```



## Architecture:

### ZKP (blueprint):

Generates proof by input coordinates

### TON Blockchain (smart contract)

Validates time, id, and tin verification for zkp part on-chain.



### Backend

Generates proof for the User (could be packed into the portable device in the future)



### Frontend

Show UI to set location and allows to send it to generate a proof.



## How to build:



### ZKP

1. Clone:

   `git clone  https://github.com/idealatom/ton-zkp-contest --recurse-submodules`

2. go to zkp directory:

   `cd ton-zkp-contest/zkp && mkdir build && cd build `

3. build

   `cmake .. && make cli`
   
4. Setup keys:
   `bin/cli setup`

```
>Saving proving key to a file "proving.key"
>Saving verification key to a file "verification.key"
```
You will get to files as output 2 files


5. Generate the proof: `bin/cli prove`

> it will save proof and primary input ti file

6. Verify: `bin/cli verify`

In the end you will get: 

> Verification status: 1

### Whole project (with backend and frontend)

The whole project packed in Docker, so you can start it by command:

`//TBD`