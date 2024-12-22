// ExecutionOrder.hpp
//
// Defines an execution order used in trading systems, extending the functionality of `BaseExecutionOrder`.
// Includes support for streaming the order's details via the `<<` operator.
//
// @class ExecutionOrder
// @description This class represents a specific implementation of an execution order, adding functionality
//              for formatted output. It inherits attributes and methods from the `BaseExecutionOrder` class.
//
// @methods 
// - Constructor: Initializes an `ExecutionOrder` with product details, pricing side, order type, quantities, and more.
// - Destructor: Default implementation.
// - operator<<: Outputs a formatted string representation of the execution order.
//
// @attributes
// Inherited from `BaseExecutionOrder`:
// - Product: The product associated with the order.
// - OrderId: Unique identifier for the order.
// - Side: Pricing side (e.g., BID or ASK).
// - OrderType: Type of the order (e.g., MARKET, LIMIT).
// - Price: Price of the order.
// - VisibleQuantity: Visible quantity of the order.
// - HiddenQuantity: Hidden quantity of the order.
// - ParentOrderId: Identifier of the parent order if applicable.
// - IsChildOrder: Indicates if the order is a child order.
//
// @date 2024-12-20
// @version 1.1
//
// @notes The `operator<<` provides a detailed, comma-separated output for logging or debugging purposes.
// @examples Example output format: "ProductId,OrderId,Bid,MARKET,123.45,1000,2000,ParentId,False"
// @dependencies PriceUtils (for formatting price values).
//
// @template T: Represents the type of the product associated with the execution order.
//
// @friend operator<<: Allows formatted output of execution order details directly via an output stream.
//
// @author Junhao Yu

#ifndef EXECUTIONORDER_HPP
#define EXECUTIONORDER_HPP

#include "BaseExecutionOrder.hpp"
#include "PriceUtils.hpp"
#include <iostream>
#include <string>

template<typename T>
class ExecutionOrder : public BaseExecutionOrder<T>
{
public:
    ExecutionOrder() = default;
    ExecutionOrder(const T& product, PricingSide side, std::string orderId, OrderType orderType, double price,
                   long visibleQuantity, long hiddenQuantity, std::string parentOrderId, bool isChildOrder)
        : BaseExecutionOrder<T>(product, side, std::move(orderId), orderType, price, visibleQuantity, hiddenQuantity,
                                std::move(parentOrderId), isChildOrder)
    {}

    virtual ~ExecutionOrder() = default;

    template<typename S>
    friend std::ostream& operator<<(std::ostream& output, const ExecutionOrder<S>& order);
};

template<typename T>
std::ostream& operator<<(std::ostream& output, const ExecutionOrder<T>& order)
{
    std::string productId = order.GetProduct().GetProductId();
    std::string sideStr = (order.GetSide() == BID) ? "Bid" : "Ask";
    std::string orderTypeStr;
    switch (order.GetOrderType())
    {
        case FOK: orderTypeStr = "FOK"; break;
        case MARKET: orderTypeStr = "MARKET"; break;
        case LIMIT: orderTypeStr = "LIMIT"; break;
        case STOP: orderTypeStr = "STOP"; break;
        case IOC: orderTypeStr = "IOC"; break;
    }

    std::string priceStr = PriceUtils::Price2Frac(order.GetPrice());
    std::string visibleQuantity = std::to_string(order.GetVisibleQuantity());
    std::string hiddenQuantity = std::to_string(order.GetHiddenQuantity());
    std::string parentOrderId = order.GetParentOrderId();
    std::string isChildOrder = order.IsChildOrder() ? "True" : "False";

    output << productId << "," << order.GetOrderId() << "," << sideStr << "," << orderTypeStr << ","
           << priceStr << "," << visibleQuantity << "," << hiddenQuantity << ","
           << parentOrderId << "," << isChildOrder;

    return output;
}

#endif
