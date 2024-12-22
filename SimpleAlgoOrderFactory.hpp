// SimpleAlgoOrderFactory.hpp
//
// Implements a simple factory for creating algorithmic execution orders based on order book data.
//
// @class SimpleAlgoOrderFactory
// @description This class provides a straightforward implementation of the `IAlgoOrderFactory` interface,
//              generating execution orders with basic logic based on the best bid-offer spread.
//
// @methods
// - CreateExecutionOrder: Generates an execution order based on the provided order book and a counter.
//
// @logic
// - If the bid-offer spread is narrow (<= 1/128), alternates between placing BID and OFFER orders.
// - Defaults to placing a BID order if conditions are not met.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef SIMPLEALGOORDERFACTORY_HPP
#define SIMPLEALGOORDERFACTORY_HPP

#include "IAlgoOrderFactory.hpp"
#include "RandomUtils.hpp"
#include <memory>

template<typename T>
class SimpleAlgoOrderFactory : public IAlgoOrderFactory<T> {
public:
    std::unique_ptr<ExecutionOrder<T>> CreateExecutionOrder(const OrderBook<T>& orderBook, long count) override {
        T product = orderBook.GetProduct();
        std::string orderId = "Algo" + RandomUtils::GenerateRandomId(11);
        std::string parentOrderId = "AlgoParent" + RandomUtils::GenerateRandomId(5);

        BidOffer bidOffer = orderBook.BestBidOffer();
        Order bid = bidOffer.GetBidOrder();
        Order offer = bidOffer.GetOfferOrder();
        double bidPrice = bid.GetPrice();
        double offerPrice = offer.GetPrice();
        long bidQuantity = bid.GetQuantity();
        long offerQuantity = offer.GetQuantity();

        PricingSide side;
        double price;
        long quantity = 0; // default 0 if no condition met

        if (offerPrice - bidPrice <= 1.0 / 128.0) {
            if (count % 2 == 0) {
                side = BID;
                price = offerPrice;
                quantity = bidQuantity;
            } else {
                side = OFFER;
                price = bidPrice;
                quantity = offerQuantity;
            }
        } else {
            side = BID;
            price = bidPrice;
            quantity = bidQuantity;
        }

        long visibleQuantity = quantity;
        long hiddenQuantity = 0;
        bool isChildOrder = false;
        OrderType orderType = MARKET;

        return std::make_unique<ExecutionOrder<T>>(product, side, orderId, orderType, price, visibleQuantity, hiddenQuantity, parentOrderId, isChildOrder);
    }
};

#endif
