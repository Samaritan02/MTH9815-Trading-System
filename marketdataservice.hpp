// marketdataservice.hpp
//
// @file marketdataservice.hpp
// @brief Defines the MarketDataService and supporting classes for managing market data.
//
// @class MarketDataService
// @description The MarketDataService is responsible for managing, aggregating, and distributing 
//              market data for financial products. It processes order books, handles external 
//              data feeds, and notifies listeners of changes. The service is designed to 
//              support real-time trading systems by maintaining a detailed order book for each 
//              product.
//
// @features
//   - **Order**: Represents individual market orders with price, quantity, and side (BID or OFFER).
//   - **BidOffer**: Encapsulates the best bid and offer for a product.
//   - **OrderBook**: Stores the bid and offer stacks for a financial product, allowing for operations
//                    like retrieving the best bid/offer and aggregating depth.
//   - **MarketDataService**: Manages a collection of OrderBooks, notifies registered listeners of updates, 
//                            and aggregates market data for efficient processing.
//   - **MarketDataConnector**: Integrates external market data feeds into the MarketDataService.
//
// @design
// This file provides an extensible framework for market data management, using templates to 
// support various financial product types. It emphasizes modularity, with clearly defined 
// responsibilities for each class, and supports real-time updates through a publish-subscribe 
// model.
//
// @date 2024-12-20
// @version 1.2
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu

#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "soa.hpp"
#include "products.hpp"
#include "PriceUtils.hpp"
#include "ProductFactory.hpp"

using namespace std;

// Enum for market sides
enum PricingSide { BID, OFFER };

// Order class encapsulates price, quantity, and side of a single order
class Order {
public:
    Order() = default;
    Order(double _price, long _quantity, PricingSide _side)
        : price(_price), quantity(_quantity), side(_side) {}

    double GetPrice() const { return price; }
    long GetQuantity() const { return quantity; }
    PricingSide GetSide() const { return side; }

private:
    double price;
    long quantity;
    PricingSide side;
};

// BidOffer class holds the best bid and offer orders
class BidOffer {
public:
    BidOffer(const Order &_bidOrder, const Order &_offerOrder)
        : bidOrder(_bidOrder), offerOrder(_offerOrder) {}

    const Order& GetBidOrder() const { return bidOrder; }
    const Order& GetOfferOrder() const { return offerOrder; }

private:
    Order bidOrder;
    Order offerOrder;
};

// OrderBook class manages bid and offer orders for a specific product
template<typename T>
class OrderBook {
public:
    OrderBook() = default;
    OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack)
        : product(_product), bidStack(_bidStack), offerStack(_offerStack) {}

    const T& GetProduct() const { return product; }
    vector<Order>& GetBidStack() { return bidStack; }
    vector<Order>& GetOfferStack() { return offerStack; }

    BidOffer BestBidOffer() const {
        auto bestBid = max_element(bidStack.begin(), bidStack.end(), ComparePriceAsc);
        auto bestOffer = min_element(offerStack.begin(), offerStack.end(), ComparePriceAsc);
        return BidOffer(*bestBid, *bestOffer);
    }

private:
    static bool ComparePriceAsc(const Order &a, const Order &b) {
        return a.GetPrice() < b.GetPrice();
    }

    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;
};

// forward declaration
template<typename T>
class MarketDataConnector;

// MarketDataService manages and disseminates market data
template<typename T>
class MarketDataService : public Service<string, OrderBook<T>> {
public:
    MarketDataService() : connector(new MarketDataConnector<T>(this)), bookDepth(5) {}

    OrderBook<T>& GetData(string key) override {
        if (orderBookMap.find(key) == orderBookMap.end()) {
            orderBookMap.emplace(key, OrderBook<T>(ProductFactory<T>::QueryProduct(key), {}, {}));
        }
        return orderBookMap[key];
    }

    void OnMessage(OrderBook<T>& data) override {
        const auto &key = data.GetProduct().GetProductId();
        orderBookMap[key] = data;
        for (auto& listener : listeners) {
            listener->ProcessAdd(data);
        }
    }

    void AddListener(ServiceListener<OrderBook<T>>* listener) override {
        listeners.push_back(listener);
    }

    const vector<ServiceListener<OrderBook<T>>*>& GetListeners() const override {
        return listeners;
    }

    MarketDataConnector<T>* GetConnector() { return connector; }
    int GetBookDepth() const { return bookDepth; }

    const BidOffer& BestBidOffer(const string &productId) {
        return orderBookMap[productId].BestBidOffer();
    }

    const OrderBook<T>& AggregateDepth(const string &productId) {
        auto &orderBook = orderBookMap[productId];
        orderBook = OrderBook<T>(orderBook.GetProduct(), Aggregate(orderBook.GetBidStack(), BID), Aggregate(orderBook.GetOfferStack(), OFFER));
        return orderBook;
    }

private:
    MarketDataConnector<T>* connector;
    unordered_map<string, OrderBook<T>> orderBookMap;
    vector<ServiceListener<OrderBook<T>>*> listeners;
    int bookDepth;

    static vector<Order> Aggregate(const vector<Order>& stack, PricingSide side) {
        unordered_map<double, long> priceMap;
        for (const auto &order : stack) {
            priceMap[order.GetPrice()] += order.GetQuantity();
        }

        vector<Order> aggregatedOrders;
        aggregatedOrders.reserve(priceMap.size());
        for (const auto &[price, quantity] : priceMap) {
            aggregatedOrders.emplace_back(price, quantity, side);
        }
        return aggregatedOrders;
    }
};

// MarketDataConnector feeds data into MarketDataService
template<typename T>
class MarketDataConnector : public Connector<OrderBook<T>> {
public:
    explicit MarketDataConnector(MarketDataService<T>* _service) : service(_service) {}

    void Publish(OrderBook<T>& data) override {}
    void Subscribe(ifstream& dataStream) {
        string line;
        getline(dataStream, line); // Skip header

        while (getline(dataStream, line)) {
            auto orderBook = ParseOrderBook(line);
            service->OnMessage(orderBook);
        }
    }

private:
    MarketDataService<T>* service;

    OrderBook<T> ParseOrderBook(const string &line) {
        stringstream ss(line);
        vector<string> fields;
        string field;

        while (getline(ss, field, ',')) {
            fields.push_back(field);
        }

        const auto &productId = fields[1];
        auto &orderBook = service->GetData(productId);

        for (int i = 0; i < service->GetBookDepth(); ++i) {
            orderBook.GetBidStack().emplace_back(PriceUtils::Frac2Price(fields[4 * i + 2]), stol(fields[4 * i + 3]), BID);
            orderBook.GetOfferStack().emplace_back(PriceUtils::Frac2Price(fields[4 * i + 4]), stol(fields[4 * i + 5]), OFFER);
        }

        return service->AggregateDepth(productId);
    }
};

#endif
