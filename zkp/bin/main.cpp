#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/fields/bls12/base_field.hpp>
#include <nil/crypto3/algebra/fields/bls12/scalar_field.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>
#include <nil/crypto3/algebra/curves/params/multiexp/bls12.hpp>
#include <nil/crypto3/algebra/curves/params/wnaf/bls12.hpp>

#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>
#include <nil/crypto3/zk/components/comparison.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark/marshalling.hpp>

#include <nil/crypto3/zk/snark/algorithms/generate.hpp>
#include <nil/crypto3/zk/snark/algorithms/verify.hpp>
#include <nil/crypto3/zk/snark/algorithms/prove.hpp>

using namespace std;

using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;

typedef algebra::curves::bls12<381> curve_type;
typedef curve_type::scalar_field_type field_type;

typedef zk::snark::r1cs_gg_ppzksnark<curve_type> scheme_type;


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

blueprint<field_type> get_blueprint() {
    blueprint<field_type> bp;

    blueprint_variable<field_type> latitudeRange0, latitudeRange1;
    blueprint_variable<field_type> longitudeRange0, longitudeRange1;
    blueprint_variable<field_type> out;
    blueprint_variable<field_type> fishingLocation0, fishingLocation1;

    blueprint_variable<field_type> lessOne, lessOrEqOne;
    blueprint_variable<field_type> lessTwo, lessOrEqTwo;
    blueprint_variable<field_type> lessThree, lessOrEqThree;
    blueprint_variable<field_type> lessFour, lessOrEqFour;

    latitudeRange0.allocate(bp);
    latitudeRange1.allocate(bp);

    longitudeRange0.allocate(bp);
    longitudeRange1.allocate(bp);

    out.allocate(bp);

    fishingLocation0.allocate(bp);
    fishingLocation1.allocate(bp);

    bp.set_input_sizes(5);

    const size_t n = 10;

    // generate_r1cs_constraints
    comparison<field_type> cmpOne(bp, n, latitudeRange0, fishingLocation0, lessOne, lessOrEqOne);
    cmpOne.generate_r1cs_constraints();
    bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOne, 1, field_type::value_type::one()));

    comparison<field_type> cmpTwo(bp, n, fishingLocation0, latitudeRange1, lessTwo, lessOrEqTwo);
    cmpTwo.generate_r1cs_constraints();
    bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessTwo, 1, field_type::value_type::one()));

    comparison<field_type> cmpThree(bp, n, fishingLocation0, latitudeRange1, lessThree, lessOrEqThree);
    cmpThree.generate_r1cs_constraints();
    bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessThree, 1, field_type::value_type::one()));

    comparison<field_type> cmpFour(bp, n, fishingLocation0, latitudeRange1, lessFour, lessOrEqFour);
    cmpFour.generate_r1cs_constraints();
    bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessFour, 1, field_type::value_type::one()));

    bp.add_r1cs_constraint(r1cs_constraint<field_type>((lessOne + lessTwo + lessThree + lessFour), 1, out));


    // generate_r1cs_witness
    bp.val(latitudeRange0) = 30;
    bp.val(latitudeRange1) = 60;
    bp.val(longitudeRange0) = 40;
    bp.val(longitudeRange1) = 60;

    bp.val(out) = 4;

    bp.val(fishingLocation0) = 40;
    bp.val(fishingLocation1) = 50;

    cmpOne.generate_r1cs_witness();
    cmpTwo.generate_r1cs_witness();
    cmpThree.generate_r1cs_witness();
    cmpFour.generate_r1cs_witness();

    return bp;
}

int main(int argc, char *argv[]) {
    size_t a, b;

    boost::program_options::options_description options("CLI Proof Generator");
    options.add_options()
    ("a,a", boost::program_options::value<size_t>(&a)->default_value(10))
    ("b,b", boost::program_options::value<size_t>(&b)->default_value(10));

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
    boost::program_options::notify(vm);

    cout << "Getting blueprint..." << endl;
    blueprint<field_type> bp = get_blueprint();

    cout << "Generating constraint system..." << endl;
    const r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();

    cout << "Generating keypair..." << endl;
    scheme_type::keypair_type keypair = generate<scheme_type>(constraint_system);

    cout << "Generating proof..." << endl;
    const scheme_type::proof_type proof = prove<scheme_type>(keypair.first, bp.primary_input(), bp.auxiliary_input());

    using basic_proof_system = r1cs_gg_ppzksnark<curve_type>;
    const bool verified = verify<basic_proof_system>(keypair.second, bp.primary_input(), proof);
    cout << "Verification status: " << verified << endl;

    cout << "Packing verifier data..." << endl;
    pack_verifier_data("proof", keypair, proof, bp.primary_input());

    return 0;
}
