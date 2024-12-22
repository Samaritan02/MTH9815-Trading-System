// ProductFactory.hpp
//
// Provides a factory for querying and creating financial products based on their unique identifiers (CUSIPs).
//
// @class ProductFactory
// @description A static utility class that maps CUSIPs to product constructors and provides methods to query
//              and create products dynamically.
//
// @methods
// - QueryProduct: Retrieves and constructs a product object based on its CUSIP.
//
// @types
// - ProductCtor: A function type representing a constructor for a product.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef PRODUCTFACTORY_HPP
#define PRODUCTFACTORY_HPP

#include <string>
#include <map>
#include <functional>

template <typename T>
class ProductFactory {
public:
    using ProductCtor = std::function<T()>;

    static T QueryProduct(const std::string& cusip) {
        auto it = GetProductConstructors().find(cusip);
        if (it == GetProductConstructors().end()) {
            throw std::invalid_argument("Unknown CUSIP: " + cusip);
        }
        return it->second();
    }

private:
    static const std::map<std::string, ProductCtor>& GetProductConstructors() {
        static std::map<std::string, ProductCtor> productConstructors = {
            {"91282CAV3", []() { return Bond("91282CAV3", CUSIP, "US2Y", 0.04500, from_string("2026/11/30")); }},
            {"91282CBL4", []() { return Bond("91282CBL4", CUSIP, "US3Y", 0.04750, from_string("2027/12/15")); }},
            {"91282CCB5", []() { return Bond("91282CCB5", CUSIP, "US5Y", 0.04875, from_string("2029/11/30")); }},
            {"91282CCS8", []() { return Bond("91282CCS8", CUSIP, "US7Y", 0.05000, from_string("2031/11/30")); }},
            {"91282CDH2", []() { return Bond("91282CDH2", CUSIP, "US10Y", 0.05125, from_string("2034/12/15")); }},
            {"912810TM0", []() { return Bond("912810TM0", CUSIP, "US20Y", 0.05250, from_string("2044/12/15")); }},
            {"912810TL2", []() { return Bond("912810TL2", CUSIP, "US30Y", 0.05375, from_string("2054/12/15")); }},
        };
        return productConstructors;
    }
};

#endif
