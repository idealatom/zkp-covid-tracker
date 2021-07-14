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
#include "./utils.cpp"
#include "./circuit.h"

using namespace std;

using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;


void setup_keys(boost::filesystem::path pk_path, boost::filesystem::path vk_path) {
    blueprint<field_type> bp;
    LocationCircuit circuit(bp);
    circuit.generate_r1cs_constraints(bp);

    cout << "Blueprint size: " << bp.num_variables() << endl;
    cout << "Generating constraint system..." << endl;
    const r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();
    cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;

    cout << "Generating keypair..." << endl;
    scheme_type::keypair_type keypair = generate<scheme_type>(constraint_system);

    cout << "Saving proving key to a file " << pk_path<< endl;
    save_proving_key(keypair.first, pk_path);

    cout << "Saving verification key to a file " << vk_path << endl;
    save_verification_key(keypair.second, vk_path);
}

void create_proof(boost::filesystem::path pk_path, boost::filesystem::path proof_path, boost::filesystem::path pi_path,float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
    cout << "Loading proving key from a file " << pk_path << endl;
    typename scheme_type::proving_key_type pk = load_proving_key(pk_path);

    blueprint<field_type> bp;
    LocationCircuit circuit(bp);
    cout << "Generating constraint system..." << endl;
    circuit.generate_r1cs_constraints(bp);
    cout << "Generating witness..." << endl;
    circuit.generate_r1cs_witness(bp, minLat, maxLat, minLng, maxLng, posLat, posLng);

    cout << "Blueprint is satisfied: " << bp.is_satisfied() << endl;
    if (!bp.is_satisfied()) {
        return;
    }

    cout << "Generating proof..." << endl;
    const scheme_type::proof_type proof = prove<scheme_type>(pk, bp.primary_input(), bp.auxiliary_input());

    cout << "Saving proof to file " << proof_path << endl;
    save_proof(proof, proof_path);

    cout << "Saving primary input to file " << pi_path << endl;
    save_primary_input(bp.primary_input(), pi_path);
}

bool verify_proof(boost::filesystem::path proof_path, boost::filesystem::path vk_path, boost::filesystem::path pi_path) {
    cout << "Loading proof from a file " << proof_path << endl;
    typename scheme_type::proof_type proof = load_proof(proof_path);

    cout << "Loading primary input from a file " << pi_path << endl;
    r1cs_primary_input<field_type> input = load_primary_input(pi_path);

    cout << "Loading verification key from a file " << vk_path << endl;
    typename scheme_type::verification_key_type vk = load_verification_key(vk_path);

    // verify
    using basic_proof_system = r1cs_gg_ppzksnark<curve_type>;
    const bool verified = verify<basic_proof_system>(vk, input, proof);
    cout << "Verification status: " << verified << endl;

    return verified;
}

int main(int argc, char *argv[]) {
    float minLat, maxLat, minLng, maxLng, posLat, posLng;
    boost::filesystem::path pk_path, vk_path, proof_path, pi_path;
    // bool hexFlag;

    boost::program_options::options_description options("CLI Proof Generator");
    options.add_options()
    // ("hex,h", boost::program_options::bool_switch(&hexFlag), "print only hex proof to output")
    ("minLat,minLat", boost::program_options::value<float>(&minLat)->default_value(0))
    ("maxLat,maxLat", boost::program_options::value<float>(&maxLat)->default_value(0))
    ("minLng,minLng", boost::program_options::value<float>(&minLng)->default_value(0))
    ("maxLng,maxLng", boost::program_options::value<float>(&maxLng)->default_value(0))
    ("posLat,posLat", boost::program_options::value<float>(&posLat)->default_value(0))
    ("proving-key-path,pk", boost::program_options::value<boost::filesystem::path>(&pk_path)->default_value("proving.key"))
    ("verification-key-path,vk", boost::program_options::value<boost::filesystem::path>(&vk_path)->default_value("verification.key"))
    ("proof-path,p", boost::program_options::value<boost::filesystem::path>(&proof_path)->default_value("proof"))
    ("primary-input-path,pi", boost::program_options::value<boost::filesystem::path>(&pi_path)->default_value("primary_input"));

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
    boost::program_options::notify(vm);

    cout << setprecision(16);

    if (!argv[1]) {
        cout << "Please select a command: [setup/prove/verify]" << endl;
        return 0;
    }
    else if (string(argv[1]) == "setup") {
        setup_keys(pk_path, vk_path);
    } else if (string(argv[1]) == "prove") {
        create_proof(pk_path, proof_path, pi_path, minLat, maxLat, minLng, maxLng, posLat, posLng);
    } else if (string(argv[1]) == "verify") {
        verify_proof(proof_path, pi_path, vk_path);
    }
    return 0;
}
