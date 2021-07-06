# Privacy-awared Covid-19 tracker



This is POC project made for TON contest:  [#18 Groth16 zkSNARK Proof Verification Use Cases](https://devex.gov.freeton.org/proposal?proposalAddress=0%3Ae6b65075478e7d412fdb0870452f30dfa8bf51272e28a3167abc5c5df6fd051d)

The idea is to proof that you wasn't in some specific area on the map, without revealing your actual location.

To do so we made prototype: zkSNARK that accepts your coordinates (as private input) and generates proof that you was in range of `r`  meters form specific area: coordinates.



## Assumtions



1. Let assume that we have special device that can securely generate, encrypt and trasfer you coordinates via encrypted protocol.

Then you can run this code on this device to generate proof that owner of this device based in some specific area (safe place).



2. We diide to set a radius and coordinates and made a zkSNARK that calculate distance by simple formula:

distance = √ a2 + b2 



```c++
				bp.val(deltaX) = bp.val(userX) - bp.val(areaX);
        bp.val(deltaY) = bp.val(userY) - bp.val(areaY);
        bp.val(deltaX_square) = bp.val(deltaX) * bp.val(deltaX);
        bp.val(deltaY_square) = bp.val(deltaY) * bp.val(deltaY);
        bp.val(deltaSum) = bp.val(deltaX_square) + bp.val(deltaY_square);
        bp.val(radius_square) = bp.val(radius) * bp.val(radius);
```



3. Let's assume that world is flat - otherwise we have to use [Haversine formula](https://en.wikipedia.org/wiki/Haversine_formula) that possible, but not in timeframe of contest :) 
4. Personal ID and Time assumtion: we can check on blockchain part to make zksnark simple as poosible. So let's assume that pesonal tracker can log it into smart contract on it's side.
5. 
