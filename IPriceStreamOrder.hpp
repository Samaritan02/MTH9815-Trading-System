// IPriceStreamOrder.hpp
//
// Defines an interface for individual orders within a price stream, encapsulating details such as price, 
// quantities, and side (bid/offer).
//
// @class IPriceStreamOrder
// @description This interface provides methods to access order-specific attributes for bid or offer orders
//              in a price stream.
//
// @methods 
// - GetSide: Returns the side of the order (e.g., BID or OFFER).
// - GetPrice: Retrieves the price of the order.
// - GetVisibleQuantity: Retrieves the visible quantity of the order.
// - GetHiddenQuantity: Retrieves the hidden quantity of the order.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IPRICESTREAMORDER_HPP
#define IPRICESTREAMORDER_HPP

#include "soa.hpp"
#include "marketdataservice.hpp"

class IPriceStreamOrder {
public:
    virtual ~IPriceStreamOrder() = default;
    virtual PricingSide GetSide() const = 0;
    virtual double GetPrice() const = 0;
    virtual long GetVisibleQuantity() const = 0;
    virtual long GetHiddenQuantity() const = 0;
};

#endif
