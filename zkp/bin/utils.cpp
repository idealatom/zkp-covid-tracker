#include <iostream>
#include <fstream>

#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark/marshalling.hpp>

#include "./types.h"


std::string convert_byteblob_to_hex_string(std::vector<std::uint8_t> blob) {
    // convert byte_blob to hex string and print it to output
    std::string hex;
    hex.reserve(blob.size() * 2);
    boost::algorithm::hex(blob.begin(), blob.end(), back_inserter(hex));
    return hex;
}

void save_byteblob(std::vector<std::uint8_t> byteblob, boost::filesystem::path fname) {
    boost::filesystem::ofstream out(fname);
    for (const auto &v : byteblob) {
        out << v;
    }
    out.close();
}

std::vector<std::uint8_t> load_byteblob(boost::filesystem::path fname) {
    boost::filesystem::ifstream stream(fname, std::ios::in | std::ios::binary);
    std::vector<std::uint8_t> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    if (contents.size() == 0) {
        throw std::ios_base::failure("Empty file");
    }
    return contents;
}

// proving key

void save_proving_key(scheme_type::proving_key_type pk, boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(pk);
    save_byteblob(byteblob, fname);
}

scheme_type::proving_key_type load_proving_key(boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = load_byteblob(fname);
    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    return nil::marshalling::verifier_input_deserializer_tvm<scheme_type>::proving_key_process(
        byteblob.cbegin(),
        byteblob.cend(),
        processingStatus);
}

// verification key

void save_verification_key(scheme_type::verification_key_type vk, boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(vk);
    save_byteblob(byteblob, fname);
}

scheme_type::verification_key_type load_verification_key(boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = load_byteblob(fname);
    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    return nil::marshalling::verifier_input_deserializer_tvm<scheme_type>::verification_key_process(
        byteblob.cbegin(),
        byteblob.cend(),
        processingStatus);
}

// proof

void save_proof(scheme_type::proof_type proof, boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(proof);
    save_byteblob(byteblob, fname);
}

scheme_type::proof_type load_proof(boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = load_byteblob(fname);
    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    return nil::marshalling::verifier_input_deserializer_tvm<scheme_type>::proof_process(
        byteblob.cbegin(),
        byteblob.cend(),
        processingStatus);
}


//  primary input

void save_primary_input(zk::snark::r1cs_primary_input<field_type> primary_input, boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(primary_input);
    save_byteblob(byteblob, fname);
}

zk::snark::r1cs_primary_input<field_type> load_primary_input(boost::filesystem::path fname) {
    std::vector<std::uint8_t> byteblob = load_byteblob(fname);
    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    return nil::marshalling::verifier_input_deserializer_tvm<scheme_type>::primary_input_process(
        byteblob.cbegin(),
        byteblob.cend(),
        processingStatus);
}
