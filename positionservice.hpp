// positionservice.hpp
//
// @file positionservice.hpp
// @brief Defines the PositionService class and supporting classes for managing positions across multiple books.
//
// @class Position
// Represents a financial position in a particular book, providing utilities to manage and query book-specific and aggregate positions.
//
// @class PositionService
// Manages a collection of positions across multiple books and securities. Provides integration with trade booking services and listeners for position updates.
//
// @class PositionServiceListener
// A specialized listener to handle trade events and update the corresponding positions in PositionService.
//
// @features
// - **Position Management**: Handles per-book and aggregate positions for a given financial product.
// - **Integration**: Supports real-time updates from a trade booking service.
// - **Event Notification**: Publishes updates to registered listeners for further processing.
//
// @design
// This service adopts a modular and extensible design, ensuring easy integration with other trading or risk management systems. Emphasis is placed on efficient handling of large-scale position data.
//
// @date 2024-12-20
// @version 1.2
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu

#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include <numeric>
#include "soa.hpp"
#include "tradebookingservice.hpp"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position {
public:
    Position() = default;
    explicit Position(const T& _product);
    ~Position() = default;

    const T& GetProduct() const;
    long GetPosition(const string &book) const;
    long GetAggregatePosition() const;
    void AddPosition(const string &book, long position);

    template<typename S>
    friend ostream& operator<<(ostream& output, const Position<S>& position);

private:
    T product;
    map<string, long> bookPositionData;
};

template<typename T>
Position<T>::Position(const T &productId) : product(productId) {}

template<typename T>
const T& Position<T>::GetProduct() const {
    return product;
}

template<typename T>
long Position<T>::GetPosition(const string &book) const {
    auto it = bookPositionData.find(book);
    return (it != bookPositionData.end()) ? it->second : 0;
}

template<typename T>
long Position<T>::GetAggregatePosition() const {
    return accumulate(bookPositionData.begin(), bookPositionData.end(), 0L, 
                      [](long sum, const auto& pair) { return sum + pair.second; });
}

template<typename T>
void Position<T>::AddPosition(const string &book, long position) {
    bookPositionData[book] += position;
}

template<typename T>
ostream& operator<<(ostream& output, const Position<T>& position) {
    output << position.product.GetProductId();
    for (const auto& [book, quantity] : position.bookPositionData) {
        output << "," << book << "," << quantity;
    }
    return output;
}

// Forward declaration of PositionServiceListener
template<typename T>
class PositionServiceListener;

/**
 * PositionService manages positions across multiple books and securities.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string, Position<T>> {
private:
    map<string, Position<T>> positionData;
    vector<ServiceListener<Position<T>>*> listeners;
    unique_ptr<PositionServiceListener<T>> positionListener;

public:
    PositionService();
    ~PositionService() = default;

    Position<T>& GetData(string key) override;
    void OnMessage(Position<T>& data) override;
    void AddListener(ServiceListener<Position<T>>* listener) override;
    const vector<ServiceListener<Position<T>>*>& GetListeners() const override;

    PositionServiceListener<T>* GetPositionListener();
    void AddTrade(const Trade<T>& trade);
};

template<typename T>
PositionService<T>::PositionService() : positionListener(make_unique<PositionServiceListener<T>>(this)) {}

template<typename T>
Position<T>& PositionService<T>::GetData(string key) {
    auto it = positionData.find(key);
    if (it != positionData.end()) {
        return it->second;
    }
    throw runtime_error("Key not found: " + key);
}

template<typename T>
void PositionService<T>::OnMessage(Position<T>& data) {
    // Implementation not required for now.
}

template<typename T>
void PositionService<T>::AddListener(ServiceListener<Position<T>>* listener) {
    listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Position<T>>*>& PositionService<T>::GetListeners() const {
    return listeners;
}

template<typename T>
PositionServiceListener<T>* PositionService<T>::GetPositionListener() {
    return positionListener.get();
}

template<typename T>
void PositionService<T>::AddTrade(const Trade<T>& trade) {
    const auto& product = trade.GetProduct();
    const string& productId = product.GetProductId();
    const string& book = trade.GetBook();
    long quantity = (trade.GetSide() == BUY ? trade.GetQuantity() : -trade.GetQuantity());

    Position<T>& position = positionData.try_emplace(productId, product).first->second;
    position.AddPosition(book, quantity);

    for (auto* listener : listeners) {
        listener->ProcessAdd(position);
    }
}

/**
 * PositionServiceListener subscribes to trade booking service and updates PositionService.
 */
template<typename T>
class PositionServiceListener : public ServiceListener<Trade<T>> {
private:
    PositionService<T>* positionService;

public:
    explicit PositionServiceListener(PositionService<T>* service);
    void ProcessAdd(Trade<T>& data) override;
    void ProcessRemove(Trade<T>& data) override {}
    void ProcessUpdate(Trade<T>& data) override {}
};

template<typename T>
PositionServiceListener<T>::PositionServiceListener(PositionService<T>* service) : positionService(service) {}

template<typename T>
void PositionServiceListener<T>::ProcessAdd(Trade<T>& data) {
    positionService->AddTrade(data);
}

#endif
