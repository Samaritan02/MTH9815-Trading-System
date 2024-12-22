// IOrder.hpp
//
// Defines an interface for orders in trading systems, encapsulating core attributes and behaviors.
//
// @class IOrder
// @description This interface provides a standardized structure for orders, including methods to access
//              details like price, quantity, type, and relationships with parent/child orders.
//
// @methods 
// - GetOrderId: Returns the unique identifier of the order.
// - GetSide: Retrieves the pricing side (e.g., BID or OFFER).
// - GetOrderType: Retrieves the type of the order (e.g., MARKET, LIMIT).
// - GetPrice: Returns the price of the order.
// - GetVisibleQuantity: Returns the visible quantity of the order.
// - GetHiddenQuantity: Returns the hidden quantity of the order.
// - GetParentOrderId: Retrieves the parent order ID, if applicable.
// - IsChildOrder: Checks whether the order is a child order.
//
// @enums
// - OrderType: Defines the types of orders (e.g., FOK, IOC, MARKET, LIMIT, STOP).
// - Market: Specifies the trading venue (e.g., BROKERTEC, ESPEED, CME).
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu


#ifndef IORDER_HPP
#define IORDER_HPP

#include <string>
#include "soa.hpp" 
#include "marketdataservice.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };
enum Market { BROKERTEC, ESPEED, CME };

class IOrder {
public:
    virtual ~IOrder() = default;
    virtual const std::string& GetOrderId() const = 0;
    virtual PricingSide GetSide() const = 0;
    virtual OrderType GetOrderType() const = 0;
    virtual double GetPrice() const = 0;
    virtual long GetVisibleQuantity() const = 0;
    virtual long GetHiddenQuantity() const = 0;
    virtual const std::string& GetParentOrderId() const = 0;
    virtual bool IsChildOrder() const = 0;
};

#endif
