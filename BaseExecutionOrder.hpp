// BaseExecutionOrder.hpp
//
// Provides a base implementation for execution orders in trading systems.
// Encapsulates the essential attributes and behaviors of an execution order.
//
// @class BaseExecutionOrder
// @description This class serves as a base implementation of the `IExecutionOrder` interface,
//              providing concrete implementations for methods to access execution order attributes.
//              It is designed to be extended or used as a foundation for specific execution order types.
//
// @methods 
// - GetOrderId: Retrieves the unique identifier of the order.
// - GetSide: Retrieves the pricing side of the order (e.g., BID or OFFER).
// - GetOrderType: Retrieves the type of the order (e.g., LIMIT or MARKET).
// - GetPrice: Retrieves the price of the order.
// - GetVisibleQuantity: Retrieves the visible quantity of the order.
// - GetHiddenQuantity: Retrieves the hidden quantity of the order.
// - GetParentOrderId: Retrieves the parent order ID if the order is a child order.
// - IsChildOrder: Indicates whether the order is a child order.
// - GetProduct: Retrieves the product associated with the order.
//
// @date 2024-12-20
// @version 1.1
//
// @notes The constructor is protected, as this class is intended to be inherited.
// @author Junhao Yu

#ifndef BASEEXECUTIONORDER_HPP
#define BASEEXECUTIONORDER_HPP

#include "IExecutionOrder.hpp"
#include <string>

template<typename T>
class BaseExecutionOrder : public IExecutionOrder<T>
{
public:
    BaseExecutionOrder() = default;
    virtual ~BaseExecutionOrder() = default;

    const std::string& GetOrderId() const override { return orderId; }
    PricingSide GetSide() const override { return side; }
    OrderType GetOrderType() const override { return orderType; }
    double GetPrice() const override { return price; }
    long GetVisibleQuantity() const override { return visibleQuantity; }
    long GetHiddenQuantity() const override { return hiddenQuantity; }
    const std::string& GetParentOrderId() const override { return parentOrderId; }
    bool IsChildOrder() const override { return isChildOrder; }
    const T& GetProduct() const override { return product; }

protected:
    BaseExecutionOrder(const T& _product, PricingSide _side, std::string _orderId, OrderType _orderType, double _price,
                       long _visibleQuantity, long _hiddenQuantity, std::string _parentOrderId, bool _isChildOrder)
        : product(_product),
          orderId(std::move(_orderId)),
          side(_side),
          orderType(_orderType),
          price(_price),
          visibleQuantity(_visibleQuantity),
          hiddenQuantity(_hiddenQuantity),
          parentOrderId(std::move(_parentOrderId)),
          isChildOrder(_isChildOrder)
    {}

private:
    T product;
    std::string orderId;
    PricingSide side;
    OrderType orderType;
    double price;
    long visibleQuantity;
    long hiddenQuantity;
    std::string parentOrderId;
    bool isChildOrder;
};

#endif
