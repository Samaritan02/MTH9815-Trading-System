// IExecutionOrder.hpp
//
// Defines an interface for execution orders, extending base order functionality.
//
// @class IExecutionOrder
// @description This interface extends the `IOrder` class to include product-specific information
//              for execution orders in trading systems.
//
// @methods 
// - GetProduct: Retrieves the product associated with the execution order.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IEXECUTIONORDER_HPP
#define IEXECUTIONORDER_HPP

#include "IOrder.hpp"
#include "IProduct.hpp"

template<typename T>
class IExecutionOrder : public IOrder {
public:
    virtual ~IExecutionOrder() = default;
    virtual const T& GetProduct() const = 0;
};

#endif
