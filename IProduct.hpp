// IProduct.hpp
//
// Defines an interface for financial products, providing a method to access the product's unique identifier.
//
// @class IProduct
// @description This interface standardizes the representation of financial products by requiring a method
//              to retrieve the product's unique identifier.
//
// @methods 
// - GetProductId: Returns the unique identifier of the product.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IPRODUCT_HPP
#define IPRODUCT_HPP

#include <string>

class IProduct {
public:
    virtual ~IProduct() = default;
    virtual const std::string& GetProductId() const = 0;
};

#endif
