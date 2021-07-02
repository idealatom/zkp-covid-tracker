#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>
#include <nil/crypto3/zk/components/comparison.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>

using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;

class LocationCircuit {

    public:
        blueprint_variable<field_type> areaX0, areaX1;
        blueprint_variable<field_type> areaY0, areaY1;
        blueprint_variable<field_type> out;
        blueprint_variable<field_type> userX, userY;

        blueprint_variable<field_type> less1, lessOrEq1;
        blueprint_variable<field_type> less2, lessOrEq2;
        blueprint_variable<field_type> less3, lessOrEq3;
        blueprint_variable<field_type> less4, lessOrEq4;

        std::shared_ptr<comparison<field_type>> cmp1;
        std::shared_ptr<comparison<field_type>> cmp2;
        std::shared_ptr<comparison<field_type>> cmp3;
        std::shared_ptr<comparison<field_type>> cmp4;

    LocationCircuit(blueprint<field_type> &bp) {
        areaX0.allocate(bp);
        areaX1.allocate(bp);

        areaY0.allocate(bp);
        areaY1.allocate(bp);

        out.allocate(bp);

        userX.allocate(bp);
        userY.allocate(bp);

        bp.set_input_sizes(5);
    }

    void generate_r1cs_constraints(blueprint<field_type> &bp) {
        std::size_t n = 10;

        // The user latitude has to be greater than or equal to
        // the min latitude of the tracking area
        cmp1.reset(new comparison<field_type>(bp, n, areaX0, userX, less1, lessOrEq1));
        cmp1.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOrEq1, 1, field_type::value_type::one()));

         // The user latitude has to be less than or equal to
        // the max latitude of the tracking area
        cmp2.reset(new comparison<field_type>(bp, n, userX, areaX1, less2, lessOrEq2));
        cmp2.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOrEq2, 1, field_type::value_type::one()));

        // The user longitude has to be greater than or equal to
        // the min longitude of the tracking area
        cmp3.reset(new comparison<field_type>(bp, n, areaY0, userY, less3, lessOrEq3));
        cmp3.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOrEq3, 1, field_type::value_type::one()));

        // The user longitude has to be less than or equal to
        // the max longitude of the tracking area
        cmp4.reset(new comparison<field_type>(bp, n, userY, areaY1, less4, lessOrEq4));
        cmp4.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOrEq4, 1, field_type::value_type::one()));

        bp.add_r1cs_constraint(r1cs_constraint<field_type>((lessOrEq1 + lessOrEq2 + lessOrEq3  + lessOrEq4), 1, out));
    }

    void generate_r1cs_witness(blueprint<field_type> &bp, std::size_t x, std::size_t y) {
        bp.val(areaX0) = 30;
        bp.val(areaX1) = 60;
        bp.val(areaY0) = 40;
        bp.val(areaY1) = 60;

        bp.val(out) = 4;

        bp.val(userX) = x;
        bp.val(userY) = y;

        cmp1.get()->generate_r1cs_witness();
        cmp2.get()->generate_r1cs_witness();
        cmp3.get()->generate_r1cs_witness();
        cmp4.get()->generate_r1cs_witness();
    }
};
