This repo is a fork of https://github.com/hot-stuff/libhotstuff.

It was used for the benchmarks in the following publications:

-  Consensus Beyond Thresholds: Generalized Byzantine Quorums Made Live.
*Orestis Alpos, Christian Cachin.* SRDS 2020. https://arxiv.org/abs/2006.04616

- Do Not Trust in Numbers: Practical Distributed Cryptography with General Trust.
*Orestis Alpos, Christian Cachin.* SSS 2023. https://eprint.iacr.org/2022/1767

Initially the whole library was adopted to support general trust assumptions
and general distributed cryptography (as opposed to threshold cryptography in
the original paper and library). This required new dependencies, such as the
*herumi bls* and *mcl* libraries, and code to support the new algebraic and
cryptographic operations (see */include/hotstuff/* and */src/*).

Later, additional distributed cryptographic schemes were developed for the
second paper. This included a *verifiable secret sharing*, a *common coin*, and
a *distributed signature* scheme, both in their threshold and in their general
versions, as long as tests and benchmarks.