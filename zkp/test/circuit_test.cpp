#define BOOST_TEST_MODULE circuit_test
#include <boost/test/included/unit_test.hpp>

#include "../bin/types.h"
#include "../bin/circuit.h"

using namespace nil::crypto3;
using namespace nil::crypto3::zk;

template<typename FieldType>
bool test_bp_input(float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
    blueprint<FieldType> bp;
    LocationCircuit<FieldType> circuit(bp);
    circuit.generate_r1cs_constraints(bp);
    circuit.generate_r1cs_witness(bp, minLat, maxLat, minLng, maxLng, posLat, posLng);
    return bp.is_satisfied();
}

void test_circuit_range(bool isSatisfied, float minLat, float maxLat, float minLng, float maxLng, float posLat, float posLng) {
    std::cout << "Testing " << posLat <<" in range (" << minLat << ", " << maxLat << ")" << std::endl;
    std::cout << "Testing " << posLng <<" in range (" << minLng << ", " << maxLng << ")" << std::endl;

    std::cout << "Testing latitude range..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, 0, 0, posLng, 0) == isSatisfied);

    std::cout << "Testing longtitude range..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(0, 0, minLat, maxLat, 0, posLat) == isSatisfied);

    std::cout << "Testing latitude and longtitude range..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, minLat, maxLat, posLng, posLat) == isSatisfied);
}

void test_circuit_range_boundaries(bool isSatisfied, float minLat, float maxLat, float minLng, float maxLng) {
    std::cout << "Testing boundaries of range (" << minLat << ", " << maxLat << ")" << std::endl;
    std::cout << "Testing boundaries of range (" << minLng << ", " << maxLng << ")" << std::endl;

    std::cout << "Testing latitude range min boundary..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, 0, 0, minLng, 0) == isSatisfied);

    std::cout << "Testing latitude range max boundary..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, 0, 0, maxLng, 0) == isSatisfied);

    std::cout << "Testing longtitude range min boundary..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(0, 0, minLat, maxLat, 0, minLat) == isSatisfied);

    std::cout << "Testing longtitude range max boundary..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(0, 0, minLat, maxLat, 0, maxLat) == isSatisfied);

    std::cout << "Testing latitude and longtitude range min boundaries..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, minLat, maxLat, minLng, minLat) == isSatisfied);

    std::cout << "Testing latitude and longtitude range max boundaries..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(minLng, maxLng, minLat, maxLat, maxLng, maxLat) == isSatisfied);
}

BOOST_AUTO_TEST_SUITE(circuit_test_suite)

float minLat = 150.987654, maxLat = 160.1234567, posLatValid = 155.7593730, posLatInvalid = 161.7593730;
float minLng = 10.123456, maxLng = 10.234567, posLngValid = 10.181234, posLngInvalid = 10.351324;

BOOST_AUTO_TEST_CASE(circuit_test_valid_ranges) {
    std::cout << setprecision(6);

    std::cout << "Testing default values..." << std::endl;
    BOOST_CHECK(test_bp_input<field_type>(0, 0, 0, 0, 0, 0));

    test_circuit_range(true, minLat, maxLat, minLng, maxLng, posLatValid, posLngValid);
    test_circuit_range_boundaries(true, minLat, maxLat, minLng, maxLng);

    // test negative values
    test_circuit_range(true, -maxLat, -minLat, -maxLng, -minLng, -posLatValid, -posLngValid);
    test_circuit_range_boundaries(true, -maxLat, -minLat, -maxLng, -minLng);
}

BOOST_AUTO_TEST_CASE(circuit_test_invalid_ranges) {
    std::cout << setprecision(6);

    test_circuit_range(false, minLat, maxLat, minLng, maxLng, posLatInvalid, posLngInvalid);

    // test negative values
    test_circuit_range(false, -maxLat, -minLat, -maxLng, -minLng, -posLatInvalid, -posLngInvalid);
}

BOOST_AUTO_TEST_SUITE_END()
