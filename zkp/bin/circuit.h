#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>
#include <nil/crypto3/zk/components/comparison.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>

using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;

class LocationCircuit {

    public:
        blueprint_variable<field_type> latitudeRange0, latitudeRange1;
        blueprint_variable<field_type> longitudeRange0, longitudeRange1;
        blueprint_variable<field_type> out;
        blueprint_variable<field_type> userLocation0, userLocation1;

        blueprint_variable<field_type> lessOne, lessOrEqOne;
        blueprint_variable<field_type> lessTwo, lessOrEqTwo;
        blueprint_variable<field_type> lessThree, lessOrEqThree;
        blueprint_variable<field_type> lessFour, lessOrEqFour;

        std::shared_ptr<comparison<field_type>> cmpOne;
        std::shared_ptr<comparison<field_type>> cmpTwo;
        std::shared_ptr<comparison<field_type>> cmpThree;
        std::shared_ptr<comparison<field_type>> cmpFour;

    LocationCircuit(blueprint<field_type> &bp) {
        latitudeRange0.allocate(bp);
        latitudeRange1.allocate(bp);

        longitudeRange0.allocate(bp);
        longitudeRange1.allocate(bp);

        out.allocate(bp);

        userLocation0.allocate(bp);
        userLocation1.allocate(bp);

        bp.set_input_sizes(5);

        std::size_t n = 10;
        cmpOne.reset(new comparison<field_type>(bp, n, latitudeRange0, userLocation0, lessOne, lessOrEqOne));
        cmpTwo.reset(new comparison<field_type>(bp, n, userLocation0, latitudeRange1, lessTwo, lessOrEqTwo));
        cmpThree.reset(new comparison<field_type>(bp, n, userLocation0, latitudeRange1, lessThree, lessOrEqThree));
        cmpFour.reset(new comparison<field_type>(bp, n, userLocation0, latitudeRange1, lessFour, lessOrEqFour));
    }

    void generate_r1cs_constraints(blueprint<field_type> &bp) {
        cmpOne.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOne, 1, field_type::value_type::one()));

        cmpTwo.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessTwo, 1, field_type::value_type::one()));

        cmpThree.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessThree, 1, field_type::value_type::one()));

        cmpFour.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessFour, 1, field_type::value_type::one()));

        bp.add_r1cs_constraint(r1cs_constraint<field_type>((lessOne + lessTwo + lessThree + lessFour), 1, out));
    }

    void generate_r1cs_witness(blueprint<field_type> &bp) {
        bp.val(latitudeRange0) = 30;
        bp.val(latitudeRange1) = 60;
        bp.val(longitudeRange0) = 40;
        bp.val(longitudeRange1) = 60;

        bp.val(out) = 4;

        bp.val(userLocation0) = 40;
        bp.val(userLocation1) = 50;

        cmpOne.get()->generate_r1cs_witness();
        cmpTwo.get()->generate_r1cs_witness();
        cmpThree.get()->generate_r1cs_witness();
        cmpFour.get()->generate_r1cs_witness();
    }
};
