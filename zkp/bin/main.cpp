#include <iostream>
#include <fstream>

#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark/marshalling.hpp>

#include <nil/crypto3/zk/snark/algorithms/generate.hpp>
#include <nil/crypto3/zk/snark/algorithms/verify.hpp>
#include <nil/crypto3/zk/snark/algorithms/prove.hpp>

#include "./types.h"
#include "./circuit.h"

using namespace std;

using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;
using namespace nil::marshalling;

// int MIN_RADIUS = 100;
// int MAX_RADIUS = 1000;
// int MAX_LATITUDE = 90;
// int MAX_LONGITUDE = 180;

void print_byte_blob_hex(vector<uint8_t> blob) {
    // convert byte_blob to hex string and print it to output
    string hex;
    hex.reserve(blob.size() * 2);
    boost::algorithm::hex(blob.begin(), blob.end(), back_inserter(hex));
    cout << hex << endl;
}

void save_input(vector<uint8_t> byteblob, boost::filesystem::path fname) {
    boost::filesystem::ofstream out(fname);
    for (const auto &v : byteblob) {
        out << v;
    }
    out.close();
}

vector<uint8_t> load_input(boost::filesystem::path fname) {
    boost::filesystem::ifstream stream(fname, ios::in | ios::binary);
    vector<uint8_t> contents((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
    // for(auto i: contents) {
    //     int value = i;
    //     cout << value << endl;
    // }
    // cout << "file size: " << contents.size() << endl;
    return contents;
}

void pack_verifier_data(
    boost::filesystem::path pout,
    scheme_type::keypair_type keypair,
    scheme_type::proof_type proof,
    r1cs_primary_input<field_type> primary_input) {

    vector<uint8_t> byteblob;

    vector<uint8_t> verification_key_byteblob = verifier_input_serializer_tvm<scheme_type>::process(
        keypair.second);
    vector<uint8_t> primary_input_byteblob = verifier_input_serializer_tvm<scheme_type>::process(
        primary_input);
    vector<uint8_t> proof_byteblob = verifier_input_serializer_tvm<scheme_type>::process(
        proof);

    byteblob.insert (byteblob.end(), proof_byteblob.begin(), proof_byteblob.end());
    byteblob.insert (byteblob.end(), primary_input_byteblob.begin(), primary_input_byteblob.end());

    // convert proof + public_input to hex string and print it to output
    print_byte_blob_hex(byteblob);

    byteblob.insert (byteblob.end(), verification_key_byteblob.begin(), verification_key_byteblob.end());

    boost::filesystem::ofstream poutf(pout);
    for (const auto &v : byteblob) {
        poutf << v;
    }
    poutf.close();
}

// blueprint<field_type> get_blueprint(float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
//     blueprint<field_type> bp;
//     LocationCircuit circuit(bp);
//     circuit.generate_r1cs_constraints(bp);
//     // circuit.generate_r1cs_witness(bp, minLat, maxLat, minLng, maxLng, posLat, posLng);
//     return bp;
// }

// bool validate_inputs(float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
//     // The latitude must be a number between -90 and 90
//     // The longitude must be a number between -180 and 180
//     if (abs(ax) > MAX_LATITUDE) {
//         cout << "Invalid area center latitude. It should be in range [" << -MAX_LATITUDE << ", " << MAX_LATITUDE << "]" << endl;
//         return false;
//     }
//     if (abs(x) > MAX_LATITUDE) {
//         cout << "Invalid user latitude. It should be in range [" << -MAX_LATITUDE << ", " << MAX_LATITUDE << "]" << endl;
//         return false;
//     }
//     if (abs(ay) > MAX_LONGITUDE) {
//         cout << "Invalid area center longitude. It should be in range [" << -MAX_LONGITUDE << ", " << MAX_LONGITUDE << "]" << endl;
//         return false;
//     }
//     if (abs(y) > MAX_LONGITUDE) {
//         cout << "Invalid user longitude. It should be in range [" << -MAX_LONGITUDE << ", " << MAX_LONGITUDE << "]" << endl;
//         return false;
//     }
//     if (r > MAX_RADIUS || r < MIN_RADIUS) {
//         cout << "Radius should be in a range [" << MIN_RADIUS << "..." << MAX_RADIUS << endl;
//         return false;
//     }

//     return true;
// }

// LocationCircuit get_circuit() {
//     blueprint<field_type> bp = get_blueprint();
//     LocationCircuit circuit(bp);
//     circuit.generate_r1cs_constraints(bp);
//     return circuit;
// }

void setup_keys() {
    blueprint<field_type> bp;
    LocationCircuit circuit(bp);
    circuit.generate_r1cs_constraints(bp);

    cout << "Blueprint size: " << bp.num_variables() << endl;
    cout << "Generating constraint system..." << endl;
    const r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();
    cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;

    cout << "Generating keypair..." << endl;
    scheme_type::keypair_type keypair = generate<scheme_type>(constraint_system);

    cout << "Saving proving key to proving.key" << endl;
    vector<uint8_t> proving_key_byteblob =
        verifier_input_serializer_tvm<scheme_type>::process(keypair.first);
    save_input(proving_key_byteblob, "proving.key");

    cout << "Saving verification key to verification.key" << endl;
    vector<uint8_t> verification_key_byteblob =
        verifier_input_serializer_tvm<scheme_type>::process(keypair.second);
    save_input(verification_key_byteblob, "verification.key");
}

void create_proof(float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
    // load proving key
    vector<uint8_t> proving_key_byteblob = load_input("proving.key");
    if (!proving_key_byteblob.size()) {
        cout << "Empty proving.key file!" << endl;
        return;
    }

    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    typename scheme_type::proving_key_type pk =
        verifier_input_deserializer_tvm<scheme_type>::proving_key_process(
            proving_key_byteblob.cbegin(),
            proving_key_byteblob.cend(),
            processingStatus);

    blueprint<field_type> bp;
    LocationCircuit circuit(bp);
    circuit.generate_r1cs_constraints(bp);
    circuit.generate_r1cs_witness(bp, minLat, maxLat, minLng, maxLng, posLat, posLng);

    cout << "Blueprint is satisfied: " << bp.is_satisfied() << endl;
    if (!bp.is_satisfied()) {
        return;
    }

    cout << "Generating proof..." << endl;
    const scheme_type::proof_type proof = prove<scheme_type>(pk, bp.primary_input(), bp.auxiliary_input());

    // save the proof
    vector<uint8_t> proof_byteblob = verifier_input_serializer_tvm<scheme_type>::process(proof);
    save_input(proof_byteblob, "proof");
}

bool verify_proof(float minLat, float maxLat, float minLng, float maxLng) {
    // load proof
    vector<uint8_t> proof_byteblob = load_input("proof");
    if (!proof_byteblob.size()) {
        cout << "Empty proof file!" << endl;
        return false;
    }
    nil::marshalling::status_type processingStatus = nil::marshalling::status_type::success;
    typename scheme_type::proof_type proof =
        verifier_input_deserializer_tvm<scheme_type>::proof_process(
            proof_byteblob.cbegin(),
            proof_byteblob.cend(),
            processingStatus);

    // load verification key
    vector<uint8_t> verification_key_byteblob = load_input("verification.key");
    if (!verification_key_byteblob.size()) {
        cout << "Empty verification.key file!" << endl;
        return false;
    }
    typename scheme_type::verification_key_type vk =
        verifier_input_deserializer_tvm<scheme_type>::verification_key_process(
            verification_key_byteblob.cbegin(),
            verification_key_byteblob.cend(),
            processingStatus);

    // public input
    r1cs_primary_input<field_type> input;
    input.push_back(convert_input(minLat));
    input.push_back(convert_input(maxLat));
    input.push_back(convert_input(minLng));
    input.push_back(convert_input(maxLng));
    input.push_back(4);

    // verify
    using basic_proof_system = r1cs_gg_ppzksnark<curve_type>;
    const bool verified = verify<basic_proof_system>(vk, input, proof);
    cout << "Verification status: " << verified << endl;

    return verified;
}

int main(int argc, char *argv[]) {
    float minLat, maxLat, minLng, maxLng, posLat, posLng;
    bool silent;

    boost::program_options::options_description options("CLI Proof Generator");
    options.add_options()
    ("silent,s", boost::program_options::bool_switch(&silent), "print only hex proof to output")
    ("minLat,minLat", boost::program_options::value<float>(&minLat)->default_value(0))
    ("maxLat,maxLat", boost::program_options::value<float>(&maxLat)->default_value(0))
    ("minLng,minLng", boost::program_options::value<float>(&minLng)->default_value(0))
    ("maxLng,maxLng", boost::program_options::value<float>(&maxLng)->default_value(0))
    ("posLat,posLat", boost::program_options::value<float>(&posLat)->default_value(0));
    ("posLng,posLng", boost::program_options::value<float>(&posLng)->default_value(0));

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
    boost::program_options::notify(vm);

    cout << setprecision(16);

    if (string(argv[1]) == "setup") {
        setup_keys();
    } else if (string(argv[1]) == "prove") {
        create_proof(minLat, maxLat, minLng, maxLng, posLat, posLng);
    } else if (string(argv[1]) == "verify") {
        verify_proof(minLat, maxLat, minLng, maxLng);
    }
    return 0;



    // if (!silent) {
    //     cout << "Lattiture range: [" << minLat << ", " << maxLat <<  "]" << endl;
    //     cout << "Longitude range: [" << minLng << ", " << maxLng <<  "]" << endl;
    //     cout << "Position coords: (" << posLat << ", " << posLng <<  ")" << endl;
    // }

    // // cout << "Validating input..." << endl;
    // // if (!validate_inputs(ax, ay, r, x, y)) {
    // //     return -1;
    // // }

    // if (!silent) {cout << "Getting blueprint..." << endl;}
    // blueprint<field_type> bp = get_blueprint();

    // if (!silent) {cout << "Blueprint size: " << bp.num_variables() << endl;}

    // if (!silent) {cout << "Blueprint is satisfied: " << bp.is_satisfied() << endl;}
    // if (!bp.is_satisfied()) {
    //     return -1;
    // }

    // if (!silent) {cout << "Generating constraint system..." << endl;}
    // const r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();
    // if (!silent) {cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;}

    // if (!silent) {cout << "Generating keypair..." << endl;}
    // scheme_type::keypair_type keypair = generate<scheme_type>(constraint_system);

    // if (!silent) {cout << "Generating proof..." << endl;}
    // const scheme_type::proof_type proof = prove<scheme_type>(keypair.first, bp.primary_input(), bp.auxiliary_input());

    // using basic_proof_system = r1cs_gg_ppzksnark<curve_type>;
    // const bool verified = verify<basic_proof_system>(keypair.second, bp.primary_input(), proof);
    // if (!silent) {cout << "Verification status: " << verified << endl;}

    // if (!verified) {
    //     return -1;
    // }

    // if (!silent) {cout << "Packing verifier data..." << endl;}
    // pack_verifier_data("proof", keypair, proof, bp.primary_input());

    // return 0;
}
