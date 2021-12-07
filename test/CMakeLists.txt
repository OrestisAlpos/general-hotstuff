include_directories(../src/
                    ../salticidae/include/
                    ../)

add_executable(test_secp256k1 test_secp256k1.cpp)
target_link_libraries(test_secp256k1 hotstuff_static)

add_executable(test_quorums test_quorums.cpp)
target_link_libraries(test_quorums hotstuff_static)

add_executable(microbench microbench.cpp)
target_link_libraries(microbench hotstuff_static)

add_executable(microbench_bls microbench_bls.cpp)
target_link_libraries(microbench_bls hotstuff_static)

add_executable(microbench_pairings microbench_pairings.cpp)
target_link_libraries(microbench_pairings hotstuff_static)

add_executable(microbench_vss microbench_vss.cpp)
target_link_libraries(microbench_vss hotstuff_static)

add_executable(microbench_coin microbench_coin.cpp)
target_link_libraries(microbench_coin hotstuff_static)
