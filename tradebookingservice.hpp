// TradeBookingService.hpp
//
// Manages the booking of trades for financial products, integrating with execution services and connectors.
//
// @class Trade
// @description Represents a trade with attributes such as product, trade ID, price, book, quantity, and side.
//
// @class TradeBookingService
// @description Manages a collection of trades, allowing booking and notifying listeners of updates.
//
// @class TradeBookingConnector
// @description Handles inbound connections for subscribing to trade data.
//
// @class TradeBookingServiceListener
// @description Subscribes to execution service updates and transfers `ExecutionOrder` data to `Trade` data for booking.
//
// @methods (Trade)
// - GetProduct: Retrieves the associated product.
// - GetTradeId: Returns the trade ID.
// - GetPrice: Retrieves the trade price.
// - GetBook: Returns the book to which the trade belongs.
// - GetQuantity: Returns the trade quantity.
// - GetSide: Returns the trade side (BUY or SELL).
//
// @methods (TradeBookingService)
// - GetData: Retrieves a trade by its trade ID.
// - OnMessage: Handles new or updated trade data.
// - AddListener: Registers a listener for trade updates.
// - GetListeners: Retrieves all registered listeners.
// - GetConnector: Provides access to the associated connector.
// - GetTradeBookingServiceListener: Provides access to the trade booking listener.
// - BookTrade: Books a trade and notifies listeners.
//
// @methods (TradeBookingConnector)
// - Publish: No-op for this inbound-only connector.
// - Subscribe: Reads trade data from an input stream and adds it to the service.
//
// @methods (TradeBookingServiceListener)
// - ProcessAdd: Converts `ExecutionOrder` data to `Trade` data and books the trade.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu


#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include "soa.hpp"
#include "executionservice.hpp"

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade {
public:
    Trade() = default;
    Trade(const T& _product, const std::string& _tradeId, double _price, const std::string& _book, long _quantity, Side _side);
    ~Trade() = default;

    const T& GetProduct() const;
    const std::string& GetTradeId() const;
    double GetPrice() const;
    const std::string& GetBook() const;
    long GetQuantity() const;
    Side GetSide() const;

private:
    T product;
    std::string tradeId;
    double price;
    std::string book;
    long quantity;
    Side side;
};

template<typename T>
Trade<T>::Trade(const T& _product, const std::string& _tradeId, double _price, const std::string& _book, long _quantity, Side _side)
    : product(_product), tradeId(_tradeId), price(_price), book(_book), quantity(_quantity), side(_side) {}

template<typename T>
const T& Trade<T>::GetProduct() const { return product; }

template<typename T>
const std::string& Trade<T>::GetTradeId() const { return tradeId; }

template<typename T>
double Trade<T>::GetPrice() const { return price; }

template<typename T>
const std::string& Trade<T>::GetBook() const { return book; }

template<typename T>
long Trade<T>::GetQuantity() const { return quantity; }

template<typename T>
Side Trade<T>::GetSide() const { return side; }

// Forward declaration
template<typename T>
class TradeBookingConnector;

template<typename T>
class TradeBookingServiceListener;

/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<std::string, Trade<T>> {
public:
    TradeBookingService();
    ~TradeBookingService() = default;

    Trade<T>& GetData(std::string key) override;
    void OnMessage(Trade<T>& data) override;
    void AddListener(ServiceListener<Trade<T>>* listener) override;
    const std::vector<ServiceListener<Trade<T>>*>& GetListeners() const override;

    TradeBookingConnector<T>* GetConnector();
    TradeBookingServiceListener<T>* GetTradeBookingServiceListener();

    void BookTrade(Trade<T>& trade);

private:
    std::map<std::string, Trade<T>> tradeData;
    std::vector<ServiceListener<Trade<T>>*> listeners;
    TradeBookingConnector<T>* connector;
    TradeBookingServiceListener<T>* tradeBookingListener;
};

template<typename T>
TradeBookingService<T>::TradeBookingService() {
    connector = new TradeBookingConnector<T>(this);
    tradeBookingListener = new TradeBookingServiceListener<T>(this);
}

template<typename T>
Trade<T>& TradeBookingService<T>::GetData(std::string key) {
    auto it = tradeData.find(key);
    if (it == tradeData.end()) {
        throw std::runtime_error("Key not found");
    }
    return it->second;
}

template<typename T>
void TradeBookingService<T>::OnMessage(Trade<T>& data) {
    string key = data.GetTradeId();
    if (tradeData.find(key) != tradeData.end())
      tradeData[key] = data;
    else
      tradeData.insert(pair<string, Trade<T>>(key, data));

    for(auto& listener : listeners)
      listener->ProcessAdd(data);
}

template<typename T>
void TradeBookingService<T>::AddListener(ServiceListener<Trade<T>>* listener) {
    listeners.push_back(listener);
}

template<typename T>
const std::vector<ServiceListener<Trade<T>>*>& TradeBookingService<T>::GetListeners() const {
    return listeners;
}

template<typename T>
TradeBookingConnector<T>* TradeBookingService<T>::GetConnector() {
    return connector;
}

template<typename T>
TradeBookingServiceListener<T>* TradeBookingService<T>::GetTradeBookingServiceListener() {
    return tradeBookingListener;
}

template<typename T>
void TradeBookingService<T>::BookTrade(Trade<T>& trade) {
    for (auto* listener : listeners) {
        listener->ProcessAdd(trade);
    }
}

/**
 * Connector that subscribes data from socket to trade booking service.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingConnector : public Connector<Trade<T>> {
public:
    explicit TradeBookingConnector(TradeBookingService<T>* service);
    ~TradeBookingConnector() = default;

    void Publish(Trade<T>& data) override;
    void Subscribe(std::ifstream& data);

private:
    TradeBookingService<T>* service;
};

template<typename T>
TradeBookingConnector<T>::TradeBookingConnector(TradeBookingService<T>* service) : service(service) {}

template<typename T>
void TradeBookingConnector<T>::Publish(Trade<T>& data) {}

template<typename T>
void TradeBookingConnector<T>::Subscribe(std::ifstream& data) {
    std::string line;
    while (std::getline(data, line)) {
        std::stringstream lineStream(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }

        const std::string& productId = tokens[0];
        T product = ProductFactory<T>::QueryProduct(productId);

        const std::string& tradeId = tokens[1];
        double price = PriceUtils::Frac2Price(tokens[2]);
        const std::string& book = tokens[3];
        long quantity = std::stol(tokens[4]);
        Side side = (tokens[5] == "BUY") ? BUY : SELL;

        Trade<T> trade(product, tradeId, price, book, quantity, side);
        service->OnMessage(trade);
    }
}

/**
 * Trade Booking Execution Listener subscribing from execution service.
 * Transfers ExecutionOrder<T> data to Trade<T> data and calls BookTrade().
 */
template<typename T>
class TradeBookingServiceListener : public ServiceListener<ExecutionOrder<T>> {
public:
    explicit TradeBookingServiceListener(TradeBookingService<T>* service);
    ~TradeBookingServiceListener() = default;

    void ProcessAdd(ExecutionOrder<T>& data) override;
    void ProcessRemove(ExecutionOrder<T>& data) override {}
    void ProcessUpdate(ExecutionOrder<T>& data) override {}

private:
    TradeBookingService<T>* service;
    long count = 0;
};

template<typename T>
TradeBookingServiceListener<T>::TradeBookingServiceListener(TradeBookingService<T>* service) : service(service) {}

template<typename T>
void TradeBookingServiceListener<T>::ProcessAdd(ExecutionOrder<T>& order) {
    T product = order.GetProduct();
    const std::string& orderId = order.GetOrderId();
    double price = order.GetPrice();
    long totalQuantity = order.GetVisibleQuantity() + order.GetHiddenQuantity();
    Side tradeSide = (order.GetSide() == BID) ? BUY : SELL;

    const std::string book = "TRSY" + std::to_string(count++ % 3 + 1);

    Trade<T> trade(product, orderId, price, book, totalQuantity, tradeSide);
    service->BookTrade(trade);
}

#endif


