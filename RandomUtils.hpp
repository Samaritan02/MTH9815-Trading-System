// RandomUtils.hpp
//
// Provides utility functions for generating random values such as spreads and IDs.
//
// @class RandomUtils
// @description A utility class offering static methods to generate random spreads for financial products
//              and unique alphanumeric identifiers.
//
// @methods
// - GenRandomSpread: Generates a random bid-offer spread within a predefined range.
// - GenerateRandomId: Creates a random alphanumeric string of specified length.
//
// @constants
// - charset: The character set used for generating random IDs.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu


#ifndef RANDOMUTILS_HPP
#define RANDOMUTILS_HPP

#include <string>
#include <random>
#include <cstdlib>

class RandomUtils {
public:
    static double GenRandomSpread(std::mt19937& gen) {
        std::uniform_real_distribution<double> dist(1.0 / 128.0, 1.0 / 64.0);
        return dist(gen);
    }

    static std::string GenerateRandomId(long length) {
        static const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static thread_local std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

        std::string id;
        id.reserve(length);
        for (long j = 0; j < length; ++j) {
            id += charset[dist(gen)];
        }
        return id;
    }
};

#endif
