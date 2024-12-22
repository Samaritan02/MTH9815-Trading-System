// PriceStreamOrder.hpp
//
// Represents an individual order (bid or offer) within a price stream, encapsulating details such as price,
// quantities, and side.
//
// @class PriceStreamOrder
// @description This class implements the `IPriceStreamOrder` interface, providing concrete methods to access
//              order details and a formatted output for logging or display.
//
// @methods 
// - GetSide: Retrieves the side of the order (e.g., BID or OFFER).
// - GetPrice: Returns the price of the order.
// - GetVisibleQuantity: Returns the visible quantity of the order.
// - GetHiddenQuantity: Returns the hidden quantity of the order.
//
// @operators
// - operator<<: Outputs a formatted representation of the order, including price, quantities, and side.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef PRICESTREAMORDER_HPP
#define PRICESTREAMORDER_HPP

#include "IPriceStreamOrder.hpp"
#include "PriceUtils.hpp"
#include <string>
#include <iostream>

class PriceStreamOrder : public IPriceStreamOrder
{
public:
    PriceStreamOrder() = default;
    PriceStreamOrder(double price, long visibleQuantity, long hiddenQuantity, PricingSide side)
        : price(price), visibleQuantity(visibleQuantity), hiddenQuantity(hiddenQuantity), side(side) {}

    virtual ~PriceStreamOrder() = default;

    PricingSide GetSide() const override { return side; }
    double GetPrice() const override { return price; }
    long GetVisibleQuantity() const override { return visibleQuantity; }
    long GetHiddenQuantity() const override { return hiddenQuantity; }

    friend std::ostream& operator<<(std::ostream& output, const PriceStreamOrder& order) {
        std::string _price = PriceUtils::Price2Frac(order.GetPrice());
        std::string _visibleQuantity = std::to_string(order.GetVisibleQuantity());
        std::string _hiddenQuantity = std::to_string(order.GetHiddenQuantity());
        std::string _side = (order.GetSide() == BID) ? "BID" : "OFFER";
        output << _price << "," << _visibleQuantity << "," << _hiddenQuantity << "," << _side;
        return output;
    }

private:
    double price;
    long visibleQuantity;
    long hiddenQuantity;
    PricingSide side;
};

#endif

