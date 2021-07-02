#include <iostream>

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


void pack_verifier_data(
    boost::filesystem::path pout,
    scheme_type::keypair_type keypair,
    scheme_type::proof_type proof,
    r1cs_primary_input<field_type> primary_input) {

    vector<uint8_t> byteblob;

    vector<uint8_t> verification_key_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
        keypair.second);
    vector<uint8_t> primary_input_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
        primary_input);
    vector<uint8_t> proof_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
        proof);

    byteblob.insert (byteblob.end(), proof_byteblob.begin(), proof_byteblob.end());
    byteblob.insert (byteblob.end(), primary_input_byteblob.begin(), primary_input_byteblob.end());
    byteblob.insert (byteblob.end(), verification_key_byteblob.begin(), verification_key_byteblob.end());

    boost::filesystem::ofstream poutf(pout);
    for (const auto &v : byteblob) {
        poutf << v;
    }
    poutf.close();
}

blueprint<field_type> get_blueprint(std::size_t x, std::size_t y) {
    blueprint<field_type> bp;
    LocationCircuit circuit(bp);
    circuit.generate_r1cs_constraints(bp);
    circuit.generate_r1cs_witness(bp, x, y);

    cout << "X = " << x << endl;
    cout << "Y = " << y << endl;
    cout << "Blueprint is satisfied: " << bp.is_satisfied() << endl;

    return bp;
}

int main(int argc, char *argv[]) {
    size_t x, y;

    boost::program_options::options_description options("CLI Proof Generator");
    options.add_options()
    ("x,x", boost::program_options::value<size_t>(&x)->default_value(0))
    ("y,y", boost::program_options::value<size_t>(&y)->default_value(0));

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
    boost::program_options::notify(vm);

    cout << "Getting blueprint..." << endl;
    blueprint<field_type> bp = get_blueprint(x, y);

    cout << "Generating constraint system..." << endl;
    const r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();

    cout << "Generating keypair..." << endl;
    scheme_type::keypair_type keypair = generate<scheme_type>(constraint_system);

    cout << "Generating proof..." << endl;
    const scheme_type::proof_type proof = prove<scheme_type>(keypair.first, bp.primary_input(), bp.auxiliary_input());

    using basic_proof_system = r1cs_gg_ppzksnark<curve_type>;
    const bool verified = verify<basic_proof_system>(keypair.second, bp.primary_input(), proof);
    cout << "Verification status: " << verified << endl;

    if (verified) {
        cout << "Packing verifier data..." << endl;
        pack_verifier_data("proof", keypair, proof, bp.primary_input());
    }

    return 0;
}
