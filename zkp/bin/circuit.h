#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>
#include <nil/crypto3/zk/components/comparison.hpp>

#include <nil/crypto3/zk/snark/schemes/ppzksnark/r1cs_gg_ppzksnark.hpp>

using namespace std;
using namespace nil::crypto3::zk::components;
using namespace nil::crypto3::zk::snark;

float roundoff(float value, uint prec)
{
    float pow_10 = pow(10, prec);
    return roundf(value * pow_10) / pow_10;
}

int convert_input(float value, uint prec) {
    int pow_10 = pow(10, prec);
    return int(roundoff(value, prec) * pow_10);
}

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
        std::size_t n = 150;

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
        // to keep all coordinates positive shift them to (1000, 1000)
        uint coord_shift = 1000;
        uint precision = 6;

        bp.val(areaX) = convert_input(ax + coord_shift, precision);
        bp.val(areaY) = convert_input(ay + coord_shift, precision);;
        bp.val(userX) = convert_input(x + coord_shift, precision);;
        bp.val(userY) = convert_input(y + coord_shift, precision);
        bp.val(radius) = convert_input(r, precision);;

        bp.val(out) = value_type::one();

        bp.val(deltaX) = bp.val(userX) - bp.val(areaX);
        bp.val(deltaY) = bp.val(userY) - bp.val(areaY);
        bp.val(deltaX_square) = bp.val(deltaX) * bp.val(deltaX);
        bp.val(deltaY_square) = bp.val(deltaY) * bp.val(deltaY);
        bp.val(deltaSum) = bp.val(deltaX_square) + bp.val(deltaY_square);
        bp.val(radius_square) = bp.val(radius) * bp.val(radius);

        cmp.get()->generate_r1cs_witness();

        cout << "Area: (" << bp.val(areaX).data << ", " << bp.val(areaY).data <<  ") radius=" << bp.val(radius).data << endl;
        cout << "User coords: (" << bp.val(userX).data << ", " << bp.val(userY).data <<  ")" << endl;
        cout << "deltaX = " << bp.val(deltaX).data << endl;
        cout << "deltaY = " << bp.val(deltaY).data << endl;
        cout << "deltaX_square = " << bp.val(deltaX_square).data << endl;
        cout << "deltaY_square = " << bp.val(deltaY_square).data << endl;
        cout << "deltaSum = " << bp.val(deltaSum).data << endl;
        cout << "radius_square = " << bp.val(radius_square).data << endl;
        cout << "less = " << bp.val(less).data << endl;
        cout << "lessOrEq = " << bp.val(lessOrEq).data << endl;
        cout << "out = " << bp.val(out).data << endl;
    }
};
