pragma ton-solidity >=0.30.0;
pragma AbiHeader pubkey;

contract Verify {
    bytes public m_vkey = hex"";

    function verify(bytes proof) public view returns (bool) {
        require(m_vkey.length > 0, 200, "Empty verification key");
        tvm.accept();
        string blob_str = proof;
        blob_str.append(m_vkey);
        return tvm.vergrth16(blob_str);
    }

    function setVerificationKey(bytes vkey) public {
        require(msg.pubkey() == tvm.pubkey(), 150);
        tvm.accept();
        m_vkey = vkey;
    }

}
