include(CMTest)
cm_find_package(Boost COMPONENTS unit_test_framework)
cm_test_link_libraries(
    crypto3::algebra
    crypto3::blueprint
    crypto3::fft
    crypto3::multiprecision
    crypto3::zk
${Boost_LIBRARIES})

cm_test(NAME circuit_test SOURCES circuit_test.cpp)
target_include_directories(circuit_test PRIVATE
"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
"$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"

${Boost_INCLUDE_DIRS})

set_target_properties(circuit_test PROPERTIES CXX_STANDARD 14)
target_compile_definitions(circuit_test PRIVATE BOOST_TEST_DYN_LINK)

# include(CMTest)

# cm_find_package(Boost REQUIRED COMPONENTS unit_test_framework)
# # if(NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
# # endif()

# cm_test_link_libraries(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}

#                        ${CMAKE_WORKSPACE_NAME}::algebra
#                        ${CMAKE_WORKSPACE_NAME}::fft
#                        ${CMAKE_WORKSPACE_NAME}::hash
#                        ${CMAKE_WORKSPACE_NAME}::multiprecision
#                        ${CMAKE_WORKSPACE_NAME}::zk

#                        ${Boost_LIBRARIES})

# macro(define_test test)
#     get_filename_component(name ${test} NAME)
#     get_filename_component(path ${test} DIRECTORY)

#     get_filename_component(dir ${test} NAME)

#     set (full_test_name ton_zkp_contest_${name}_test)

#     if (NOT ${path}  STREQUAL "")
#         get_filename_component(dir ${path} NAME)

#         set (full_test_name ton_zkp_contest_${dir}_${name}_test)
#     endif()

#     cm_test(NAME ${full_test_name} SOURCES ${test}.cpp)

#     target_include_directories(${full_test_name} PRIVATE
#                                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
#                                "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"

#                                ${Boost_INCLUDE_DIRS})

#     set_target_properties(${full_test_name} PROPERTIES CXX_STANDARD 14)

#     get_target_property(target_type Boost::unit_test_framework TYPE)
#     if(target_type STREQUAL "SHARED_LIB")
#         target_compile_definitions(${full_test_name} PRIVATE BOOST_TEST_DYN_LINK)
#     elseif(target_type STREQUAL "STATIC_LIB")

#     endif()
# endmacro()

# set(TESTS_NAMES
#     "basic_components"
#     # "basic_components_r1cs_gg_ppzksnark"
#     )

# foreach(TEST_FILE ${TESTS_NAMES})
#     define_test(${TEST_FILE})
# endforeach()
