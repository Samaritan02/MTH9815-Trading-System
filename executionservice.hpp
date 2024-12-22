// executionservice.hpp
//
// Provides the ExecutionService class and related utilities.
//
// @class ExecutionService
// @description Manages the execution of orders, integrates with connectors, and supports listeners for market execution updates.
//              The service operates on execution orders tied to specific products.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "soa.hpp"
#include "ExecutionOrder.hpp"
#include "AlgoExecution.hpp"

/**
 * Forward declaration of ExecutionServiceConnector and ExecutionServiceListener.
 */
template <typename T>
class ExecutionServiceConnector;
template <typename T>
class ExecutionServiceListener;

/**
 * ExecutionService class
 * Responsible for placing and managing execution orders, publishing executions, and notifying listeners.
 * Template parameter T denotes the product type.
 */
template <typename T>
class ExecutionService : public Service<std::string, ExecutionOrder<T>> {
public:
    ExecutionService();
    ~ExecutionService() = default;

    ExecutionOrder<T> &GetData(std::string key) override;
    void OnMessage(ExecutionOrder<T> &data) override;
    void AddListener(ServiceListener<ExecutionOrder<T>> *listener) override;
    const std::vector<ServiceListener<ExecutionOrder<T>> *> &GetListeners() const override;

    ExecutionServiceListener<T> *GetExecutionServiceListener();
    ExecutionServiceConnector<T> *GetConnector();

    void ExecuteOrder(const ExecutionOrder<T> &order, Market market);
    void AddExecutionOrder(const AlgoExecution<T> &algoExecution);

private:
    std::map<std::string, ExecutionOrder<T>> executionOrderData;
    std::vector<ServiceListener<ExecutionOrder<T>> *> listeners;
    ExecutionServiceConnector<T> *connector;
    ExecutionServiceListener<T> *executionServiceListener;
};

template <typename T>
ExecutionService<T>::ExecutionService() : executionServiceListener(new ExecutionServiceListener<T>(this)), connector(nullptr) {}

template <typename T>
ExecutionOrder<T> &ExecutionService<T>::GetData(std::string key) {
    auto it = executionOrderData.find(key);
    if (it != executionOrderData.end()) {
        return it->second;
    }
    throw std::runtime_error("Specified key not found");
}

template <typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T> &data) {
    // No processing required for incoming messages as per current design.
}

template <typename T>
void ExecutionService<T>::AddListener(ServiceListener<ExecutionOrder<T>> *listener) {
    listeners.push_back(listener);
}

template <typename T>
const std::vector<ServiceListener<ExecutionOrder<T>> *> &ExecutionService<T>::GetListeners() const {
    return listeners;
}

template <typename T>
ExecutionServiceListener<T> *ExecutionService<T>::GetExecutionServiceListener() {
    return executionServiceListener;
}

template <typename T>
ExecutionServiceConnector<T> *ExecutionService<T>::GetConnector() {
    return connector;
}

template <typename T>
void ExecutionService<T>::ExecuteOrder(const ExecutionOrder<T> &order, Market market) {
    if (connector) {
        connector->Publish(order, market);
    }
}

template <typename T>
void ExecutionService<T>::AddExecutionOrder(const AlgoExecution<T> &algoExecution) {
    ExecutionOrder<T> executionOrder = algoExecution.GetExecutionOrder();
    std::string orderId = executionOrder.GetOrderId();

    if (executionOrderData.find(orderId) != executionOrderData.end())
    {
        executionOrderData.erase(orderId);
    }
    executionOrderData.insert(std::make_pair(orderId, executionOrder));

    for (auto &listener : listeners) {
        listener->ProcessAdd(executionOrder);
    }
}

/**
 * ExecutionServiceConnector class
 * Publishes ExecutionOrder data to markets.
 */
template <typename T>
class ExecutionServiceConnector : public Connector<ExecutionOrder<T>> {
public:
    explicit ExecutionServiceConnector(ExecutionService<T> *_service);
    ~ExecutionServiceConnector() = default;

    void Publish(const ExecutionOrder<T> &order, Market &market);

private:
    ExecutionService<T> *service;
};

template <typename T>
ExecutionServiceConnector<T>::ExecutionServiceConnector(ExecutionService<T> *_service) : service(_service) {}

template <typename T>
void ExecutionServiceConnector<T>::Publish(const ExecutionOrder<T> &order, Market &market) {
    std::string orderType;
    switch (order.GetOrderType()) {
        case FOK: orderType = "FOK"; break;
        case MARKET: orderType = "MARKET"; break;
        case LIMIT: orderType = "LIMIT"; break;
        case STOP: orderType = "STOP"; break;
        case IOC: orderType = "IOC"; break;
    }

    std::string tradeMarket;
    switch (market) {
        case BROKERTEC: tradeMarket = "BROKERTEC"; break;
        case ESPEED: tradeMarket = "ESPEED"; break;
        case CME: tradeMarket = "CME"; break;
    }

    std::cout << "ExecutionOrder: \n"
              << "\tProduct: " << order.GetProduct().GetProductId() << "\tOrderId: " << order.GetOrderId() << "\tMarket: " << tradeMarket << "\n"
              << "\tPricingSide: " << (order.GetSide() == BID ? "Bid" : "Offer")
              << "\tOrderType: " << orderType << "\tChildOrder: " << (order.IsChildOrder() ? "Yes" : "No") << "\n"
              << "\tPrice: " << order.GetPrice() << "\tVisibleQty: " << order.GetVisibleQuantity() << "\tHiddenQty: " << order.GetHiddenQuantity() << std::endl;
}

/**
 * ExecutionServiceListener class
 * Listens to AlgoExecution data and translates it into ExecutionOrders.
 */
template <typename T>
class ExecutionServiceListener : public ServiceListener<AlgoExecution<T>> {
public:
    explicit ExecutionServiceListener(ExecutionService<T> *_executionService);
    ~ExecutionServiceListener() = default;

    void ProcessAdd(AlgoExecution<T> &data) override;
    void ProcessRemove(AlgoExecution<T> &data) override;
    void ProcessUpdate(AlgoExecution<T> &data) override;

private:
    ExecutionService<T> *executionService;
};

template <typename T>
ExecutionServiceListener<T>::ExecutionServiceListener(ExecutionService<T> *_executionService) : executionService(_executionService) {}

template <typename T>
void ExecutionServiceListener<T>::ProcessAdd(AlgoExecution<T> &data) {
    executionService->AddExecutionOrder(data);
    executionService->ExecuteOrder(data.GetExecutionOrder(), data.GetMarket());
}

template <typename T>
void ExecutionServiceListener<T>::ProcessRemove(AlgoExecution<T> &data) {
    // No removal logic implemented
}

template <typename T>
void ExecutionServiceListener<T>::ProcessUpdate(AlgoExecution<T> &data) {
    // No update logic implemented
}

#endif
