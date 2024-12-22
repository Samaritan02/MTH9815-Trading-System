// PriceUtils.hpp
//
// Utility class for converting between fractional price formats and decimal price representations.
//
// @class PriceUtils
// @description Provides static methods to convert fractional bond prices (e.g., "99-162+") to decimal prices
//              and vice versa.
//
// @methods 
// - Frac2Price: Converts a fractional price string to its decimal equivalent.
// - Price2Frac: Converts a decimal price to its fractional string representation.
//
// @constants
// - BASE32: Used for the numerator in the fractional price conversion.
// - BASE256: Represents finer granularity for the fractional conversion.
// - FRACTIONAL_THRESHOLD: Determines the range for fractional parts.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu


#ifndef PRICEUTILS_HPP
#define PRICEUTILS_HPP

#include <string>
#include <stdexcept>
#include <cmath>

class PriceUtils {
public:
    static double Frac2Price(const std::string& PriceFrac) {
        constexpr int FRACTIONAL_LENGTH = 3;
        constexpr double BASE32 = 32.0;
        constexpr double BASE256 = 256.0;

        int posDash = PriceFrac.find('-');
        if (posDash == std::string::npos) {
            throw std::invalid_argument("Invalid format: Dash '-' not found. Expected format: 'X-XX+' or 'X-XXY'.");
        }

        double price1;
        try {
            price1 = std::stod(PriceFrac.substr(0, posDash));
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid format: Integer part is not a valid number.");
        }

        std::string fractionalPart = PriceFrac.substr(posDash + 1);
        if (fractionalPart.size() != FRACTIONAL_LENGTH) {
            throw std::invalid_argument("Invalid format: Fractional part should be exactly 3 characters.");
        }

        if (fractionalPart[2] == '+') {
            fractionalPart[2] = '4';
        }

        double price32, price256;
        try {
            price32 = std::stod(fractionalPart.substr(0, 2)) / BASE32;
            price256 = std::stod(fractionalPart.substr(2, 1)) / BASE256;
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid format: Fractional part contains non-numeric characters.");
        }

        return price1 + price32 + price256;
    }

    static std::string Price2Frac(double price) {
        constexpr double BASE32 = 32.0;
        constexpr double BASE256 = 256.0;
        constexpr int FRACTIONAL_THRESHOLD = 8;

        int intpart = static_cast<int>(std::floor(price));
        double fracpart = price - intpart;

        int xy = static_cast<int>(fracpart * BASE32);
        int z = static_cast<int>(fracpart * BASE256) % FRACTIONAL_THRESHOLD;

        std::string fractionalString = std::to_string(intpart) + "-";
        fractionalString += (xy < 10 ? "0" : "") + std::to_string(xy);
        fractionalString += (z == 4 ? "+" : std::to_string(z));

        return fractionalString;
    }
};

#endif
