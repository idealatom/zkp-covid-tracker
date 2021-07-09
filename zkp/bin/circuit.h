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

int convert_input(float value) {
    // to keep all coordinates positive shift them
    uint INPUT_SHIFT = 1000;
    uint PRECISION = 6;
    int pow_10 = pow(10, PRECISION);
    return int(roundoff(value + INPUT_SHIFT, PRECISION) * pow_10);
}

template<typename field_type>
class LocationCircuit {

    public:
        blueprint_variable<field_type> minLat, maxLat;
        blueprint_variable<field_type> minLng, maxLng;
        blueprint_variable<field_type> posLat, posLng;
        blueprint_variable<field_type> out;

        blueprint_variable<field_type> minLatLess, minLatLessOrEq, maxLatLess, maxLatLessOrEq;
        blueprint_variable<field_type> minLngLess, minLngLessOrEq, maxLngLess, maxLngLessOrEq;

        std::shared_ptr<comparison<field_type>> minLatCmp, maxLatCmp, minLngCmp, maxLngCmp;

    LocationCircuit(blueprint<field_type> &bp) {
        minLat.allocate(bp);
        maxLat.allocate(bp);
        minLng.allocate(bp);
        maxLng.allocate(bp);

        out.allocate(bp);

        posLat.allocate(bp);
        posLng.allocate(bp);

        minLatLess.allocate(bp);
        maxLatLess.allocate(bp);
        minLngLess.allocate(bp);
        maxLngLess.allocate(bp);
        minLatLessOrEq.allocate(bp);
        maxLatLessOrEq.allocate(bp);
        minLngLessOrEq.allocate(bp);
        maxLngLessOrEq.allocate(bp);

        bp.set_input_sizes(1);
    }

    void generate_r1cs_constraints(blueprint<field_type> &bp) {
        std::size_t comparison_n = 150;

        // minLat <= posLat = 1
        minLatCmp.reset(new comparison<field_type>(bp, comparison_n, minLat, posLat, minLatLess, minLatLessOrEq));
        minLatCmp.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(minLatLessOrEq, 1, value_type::one()));

        // maxLat >= posLat = 1
        maxLatCmp.reset(new comparison<field_type>(bp, comparison_n, posLat, maxLat, maxLatLess, maxLatLessOrEq));
        maxLatCmp.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(maxLatLessOrEq, 1, value_type::one()));

        // minLng <= posLng = 1
        minLngCmp.reset(new comparison<field_type>(bp, comparison_n, minLng, posLng, minLngLess, minLngLessOrEq));
        minLngCmp.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(minLngLessOrEq, 1, value_type::one()));

        // maxLng >= posLng = 1
        maxLngCmp.reset(new comparison<field_type>(bp, comparison_n, posLng, maxLng, maxLngLess, maxLngLessOrEq));
        maxLngCmp.get()->generate_r1cs_constraints();
        bp.add_r1cs_constraint(r1cs_constraint<field_type>(maxLngLessOrEq, 1, value_type::one()));

        bp.add_r1cs_constraint(r1cs_constraint<field_type>(minLatLessOrEq + maxLatLessOrEq + minLngLessOrEq + maxLngLessOrEq, 1, out));
    }

    void generate_r1cs_witness(blueprint<field_type> &bp, float minLat_, float maxLat_, float minLng_, float maxLng_, float posLat_, float posLng_) {
        bp.val(minLat) = convert_input(minLat_);
        bp.val(maxLat) = convert_input(maxLat_);
        bp.val(minLng) = convert_input(minLng_);
        bp.val(maxLng) = convert_input(maxLng_);
        bp.val(posLat) = convert_input(posLat_);
        bp.val(posLng) = convert_input(posLng_);

        bp.val(out) = 4;

        minLatCmp.get()->generate_r1cs_witness();
        maxLatCmp.get()->generate_r1cs_witness();
        minLngCmp.get()->generate_r1cs_witness();
        maxLngCmp.get()->generate_r1cs_witness();

        // cout << "Lattiture range: [" << bp.val(minLat).data << ", " << bp.val(maxLat).data <<  "]" << endl;
        // cout << "Longitude range: [" << bp.val(minLng).data << ", " << bp.val(maxLng).data <<  "]" << endl;
        // cout << "Position coords: (" << bp.val(posLat).data << ", " << bp.val(posLng).data <<  ")" << endl;

        // cout << "minLatLess = " << bp.val(minLatLess).data << endl;
        // cout << "maxLatLess = " << bp.val(maxLatLess).data << endl;
        // cout << "minLngLess = " << bp.val(minLngLess).data << endl;
        // cout << "maxLngLess = " << bp.val(maxLngLess).data << endl;
        // cout << "minLatLessOrEq = " << bp.val(minLatLessOrEq).data << endl;
        // cout << "maxLatLessOrEq = " << bp.val(maxLatLessOrEq).data << endl;
        // cout << "minLngLessOrEq = " << bp.val(minLngLessOrEq).data << endl;
        // cout << "maxLngLessOrEq = " << bp.val(maxLngLessOrEq).data << endl;
    }
};
