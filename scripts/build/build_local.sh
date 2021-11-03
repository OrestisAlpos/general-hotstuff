#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DHOTSTUFF_ENABLE_BENCHMARK=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SAMPLES=OFF -DGENERALIZED_QUORUMS_WITH_MSP=OFF -DUSE_GENERALIZED_QUORUMS=OFF -DUSE_BLS=OFF 
make
cp hotstuff-app hotstuff-app_thr_dsa
cp hotstuff-client hotstuff-client_thr_dsa
cp hotstuff-keygen hotstuff-keygen_thr

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DHOTSTUFF_ENABLE_BENCHMARK=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SAMPLES=OFF -DGENERALIZED_QUORUMS_WITH_MSP=OFF -DUSE_GENERALIZED_QUORUMS=OFF -DUSE_BLS=ON
make
cp hotstuff-app hotstuff-app_thr_bls
cp hotstuff-client hotstuff-client_thr_bls
cp hotstuff-keygen hotstuff-keygen_thr

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DHOTSTUFF_ENABLE_BENCHMARK=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SAMPLES=OFF -DGENERALIZED_QUORUMS_WITH_MSP=OFF -DUSE_GENERALIZED_QUORUMS=ON -DUSE_BLS=OFF
make
cp hotstuff-app hotstuff-app_gen_dsa
cp hotstuff-client hotstuff-client_gen_dsa
cp hotstuff-keygen hotstuff-keygen_gen

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DHOTSTUFF_ENABLE_BENCHMARK=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_SAMPLES=OFF -DGENERALIZED_QUORUMS_WITH_MSP=OFF -DUSE_GENERALIZED_QUORUMS=ON -DUSE_BLS=ON
make
cp hotstuff-app hotstuff-app_gen_bls
cp hotstuff-client hotstuff-client_gen_bls
cp hotstuff-keygen hotstuff-keygen_gen
