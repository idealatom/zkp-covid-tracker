#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>
#include <nil/crypto3/zk/components/comparison.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>

using namespace std;
using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;

template<typename field_type>
class LocationCircuit {

    public:
        blueprint_variable<field_type> areaX, areaY;
        blueprint_variable<field_type> radius;
        blueprint_variable<field_type> out;
        blueprint_variable<field_type> userX, userY;

        blueprint_variable<field_type> deltaX, deltaY, deltaSum;
        blueprint_variable<field_type> deltaX_square, deltaY_square, radius_square;
        blueprint_variable<field_type> less, lessOrEq;

        std::shared_ptr<comparison<field_type>> cmp;

    LocationCircuit(blueprint<field_type> &bp) {
        areaX.allocate(bp);
        areaY.allocate(bp);
        radius.allocate(bp);

        out.allocate(bp);

        userX.allocate(bp);
        userY.allocate(bp);

        deltaX.allocate(bp);
        deltaY.allocate(bp);
        deltaSum.allocate(bp);
        deltaX_square.allocate(bp);
        deltaY_square.allocate(bp);
        radius_square.allocate(bp);
        less.allocate(bp);
        lessOrEq.allocate(bp);

        bp.set_input_sizes(4);
    }

    void generate_r1cs_constraints(blueprint<field_type> &bp) {
        std::size_t n = 100;
        // (userX - areaX)^2 + (userY - areaY)^2 <= radius^2.

        // 1. userX - areaX = deltaX
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(userX - areaX, 1, deltaX));
        // 2. deltaX^2 = deltaX_square
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(deltaX, deltaX, deltaX_square));
        // 3. userY - areaY = deltaY
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(userY - areaY, 1, deltaY));
        // 4. deltaY^2 = deltaY_square
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(deltaY, deltaY, deltaY_square));
        // 5. deltaX_square + deltaY_square = deltaSum
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(deltaX_square + deltaY_square, 1, deltaSum));
        // 6. radius^2 = radius_square
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(radius, radius, radius_square));
        // 7. deltaSum < radius_square
        cmp.reset(new comparison<field_type>(bp, n, deltaSum, radius_square, less, lessOrEq));
        cmp.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(lessOrEq, 1, out));
    }

    void generate_r1cs_witness(blueprint<field_type> &bp, float ax, float ay, float r, float x, float y) {
        bp.val(areaX) = ax;
        bp.val(areaY) = ay;
        bp.val(radius) = r;

        bp.val(out) = 1;

        bp.val(userX) = x;
        bp.val(userY) = y;

        bp.val(deltaX) = x - ax;
        bp.val(deltaY) = y - ay;
        bp.val(deltaX_square) = std::pow(x - ax, 2);
        bp.val(deltaY_square) = std::pow(y - ay, 2);
        bp.val(deltaSum) = std::pow(x - ax, 2) + std::pow(y - ay, 2);
        bp.val(radius_square) = r * r;

        cmp.get()->generate_r1cs_witness();
    }
};
