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

using namespace nil::crypto3;
using namespace nil::crypto3::zk;

typedef algebra::curves::bls12<381> curve_type;
typedef curve_type::scalar_field_type field_type;

typedef zk::snark::r1cs_gg_ppzksnark<curve_type> scheme_type;


void pack_verifier_data(
    scheme_type::keypair_type keypair,
    scheme_type::proof_type proof,
    snark::r1cs_primary_input<field_type> primary_input) {

    boost::filesystem::path pout = "proof";
    std::vector<std::uint8_t> byteblob;

    std::vector<std::uint8_t> verification_key_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
        keypair.second);
    std::vector<std::uint8_t> primary_input_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
        primary_input);
    std::vector<std::uint8_t> proof_byteblob = nil::marshalling::verifier_input_serializer_tvm<scheme_type>::process(
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

components::blueprint<field_type> get_fisherman_blueprint() {
    components::blueprint<field_type> bp;

    components::blueprint_variable<field_type> latitudeRange0, latitudeRange1;
    components::blueprint_variable<field_type> longitudeRange0, longitudeRange1;
    components::blueprint_variable<field_type> out;
    components::blueprint_variable<field_type> fishingLocation0, fishingLocation1;

    components::blueprint_variable<field_type> lessOne, lessOrEqOne;
    components::blueprint_variable<field_type> lessTwo, lessOrEqTwo;
    components::blueprint_variable<field_type> lessThree, lessOrEqThree;
    components::blueprint_variable<field_type> lessFour, lessOrEqFour;

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
    components::comparison<field_type> cmpOne(bp, n, latitudeRange0, fishingLocation0, lessOne, lessOrEqOne);
    cmpOne.generate_r1cs_constraints();
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(lessOne, 1, field_type::value_type::one()));

    components::comparison<field_type> cmpTwo(bp, n, fishingLocation0, latitudeRange1, lessTwo, lessOrEqTwo);
    cmpTwo.generate_r1cs_constraints();
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(lessTwo, 1, field_type::value_type::one()));

    components::comparison<field_type> cmpThree(bp, n, fishingLocation0, latitudeRange1, lessThree, lessOrEqThree);
    cmpThree.generate_r1cs_constraints();
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(lessThree, 1, field_type::value_type::one()));

    components::comparison<field_type> cmpFour(bp, n, fishingLocation0, latitudeRange1, lessFour, lessOrEqFour);
    cmpFour.generate_r1cs_constraints();
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(lessFour, 1, field_type::value_type::one()));

    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>((lessOne + lessTwo + lessThree + lessFour), 1, out));


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

components::blueprint<field_type> get_blueprint(std::size_t a, std::size_t b) {
    const size_t n = 5;

    components::blueprint<field_type> bp;

    components::blueprint_variable<field_type> A, B, less, less_or_eq;
    A.allocate(bp);
    B.allocate(bp);
    less.allocate(bp);
    less_or_eq.allocate(bp);

    components::comparison<field_type> cmp(bp, n, A, B, less, less_or_eq);
    cmp.generate_r1cs_constraints();

    // const std::size_t a = 3;
    // const std::size_t b = 1;

    bp.val(A) = field_type::value_type(a);
    bp.val(B) = field_type::value_type(b);

    cmp.generate_r1cs_witness();

    const bool _less = bp.val(less) == field_type::value_type::one() ? true : false;
    const bool _less_or_eq = bp.val(less_or_eq) == field_type::value_type::one() ? true : false;

    std::cout << "A: " << a << std::endl;
    std::cout << "B: " << b << std::endl;
    std::cout << "LESS: " << _less << std::endl;
    std::cout << "LESS OR EQ: " << _less_or_eq << std::endl;
    std::cout << "Blueprint is satisfied: " << bp.is_satisfied() << std::endl;

    return bp;
}

components::blueprint<field_type> get_example_blueprint() {

    // Create blueprint
    components::blueprint<field_type> bp;

    // Define variables
    components::blueprint_variable<field_type> x;
    components::blueprint_variable<field_type> sym_1;
    components::blueprint_variable<field_type> y;
    components::blueprint_variable<field_type> sym_2;
    components::blueprint_variable<field_type> out;


  // Allocate variables to blueprint
  // The strings (like "x") are only for debugging purposes
    out.allocate(bp);
    x.allocate(bp);
    sym_1.allocate(bp);
    y.allocate(bp);
    sym_2.allocate(bp);

    // This sets up the blueprint variables
    // so that the first one (out) represents the public
    // input and the rest is private input
    bp.set_input_sizes(1);

    // Add R1CS constraints to blueprint:

    // x*x = sym_1
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(x, x, sym_1));

    // sym_1 * x = y
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(sym_1, x, y));

    // y + x = sym_2
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(y + x, 1, sym_2));

    // sym_2 + 5 = ~out
    bp.add_r1cs_constraint(snark::r1cs_constraint<field_type>(sym_2 + 5, 1, out));


    // Add witness values

    bp.val(x) = 3;
    bp.val(out) = 35;
    bp.val(sym_1) = 9;
    bp.val(y) = 27;
    bp.val(sym_2) = 30;

    return bp;
}

int main(int argc, char *argv[]) {
    std::size_t a, b;

    boost::program_options::options_description options("CLI Proof Generator");
    options.add_options()
    ("a,a", boost::program_options::value<std::size_t>(&a)->default_value(10))
    ("b,b", boost::program_options::value<std::size_t>(&b)->default_value(10));

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
    boost::program_options::notify(vm);

    std::cout << "Getting blueprint..." << std::endl;

    // components::blueprint<field_type> bp = get_blueprint(a, b);
    components::blueprint<field_type> bp = get_fisherman_blueprint();

    std::cout << "Generating constraint system..." << std::endl;

    const snark::r1cs_constraint_system<field_type> constraint_system = bp.get_constraint_system();

    // std::cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << std::endl;
    // std::cout << "Primary (public) input: " << bp.primary_input() << std::endl;
    // std::cout << "Auxiliary (private) input: " << bp.auxiliary_input() << std::endl;

    std::cout << "Generating keypair..." << std::endl;

    scheme_type::keypair_type keypair = zk::snark::generate<scheme_type>(constraint_system);


    std::cout << "Generating proof..." << std::endl;

    const scheme_type::proof_type proof = snark::prove<scheme_type>(keypair.first, bp.primary_input(), bp.auxiliary_input());

    using basic_proof_system = snark::r1cs_gg_ppzksnark<curve_type>;
    const bool verified = snark::verify<basic_proof_system>(keypair.second, bp.primary_input(), proof);

    std::cout << "Verification status: " << verified << std::endl;


    std::cout << "Packing verifier data..." << std::endl;

    pack_verifier_data(keypair, proof, bp.primary_input());

    return 0;
}
