// AlgoExecutionService.hpp
//
// Manages the lifecycle of algorithmic execution orders in financial systems.
// Provides methods for creating, storing, and notifying listeners about algorithmic execution data.
//
// @class AlgoExecutionService
// @description This class handles the orchestration of algorithmic execution orders by managing
//              data storage, listener notifications, and interfacing with a factory to create execution orders.
//
// @methods 
// - GetData: Retrieves execution data by key.
// - OnMessage: Processes incoming messages related to algorithmic execution (not implemented).
// - AddListener: Adds a listener to observe algorithmic execution events.
// - GetListeners: Retrieves a list of listeners observing the service.
// - AlgoExecuteOrder: Creates and processes algorithmic execution orders from an order book.
// - GetAlgoExecutionServiceListener: Retrieves the listener for handling service-specific events.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOEXECUTIONSERVICE_HPP
#define ALGOEXECUTIONSERVICE_HPP

#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include "IAlgoExecutionService.hpp"
#include "IAlgoExecution.hpp"
#include "ExecutionOrder.hpp"
#include "AlgoExecution.hpp"
#include "AlgoExecutionServiceListener.hpp"
#include "IAlgoOrderFactory.hpp"

template<typename T>
class AlgoExecutionService : public IAlgoExecutionService<T>
{
public:
    AlgoExecutionService(std::unique_ptr<IAlgoOrderFactory<T>> factory) 
        : algoexecservicelistener(new AlgoExecutionServiceListener<T>(this)), 
          count(0), orderFactory(std::move(factory))
    {}

    virtual ~AlgoExecutionService() = default;

    IAlgoExecution<T>& GetData(std::string key) override {
        auto it = algoExecutionData.find(key);
        if (it == algoExecutionData.end()) {
            throw std::runtime_error("Key not found");
        }
        return *(it->second);
    }

    void OnMessage(IAlgoExecution<T>& data) override {
        // no implementation needed
    }

    void AddListener(ServiceListener<IAlgoExecution<T>> *listener) override {
    }
    
    void AddListener(ServiceListener<AlgoExecution<T>> *listener) {
        listeners.push_back(listener);
    }

    const std::vector<ServiceListener<IAlgoExecution<T>>*>& GetListeners() const override {
        static std::vector<ServiceListener<IAlgoExecution<T>>*> baseListeners;
        baseListeners.clear();

        for (auto* listener : listeners) {
            baseListeners.push_back(dynamic_cast<ServiceListener<IAlgoExecution<T>>*>(listener));
        }
        return baseListeners;
    }

    void AlgoExecuteOrder(OrderBook<T>& orderBook) override {
        auto execOrder = orderFactory->CreateExecutionOrder(orderBook, count);
        count++;

        auto algoExecutionObj = std::make_unique<AlgoExecution<T>>(*execOrder, BROKERTEC);

        std::string key = execOrder->GetProduct().GetProductId();

        algoExecutionData[key] = algoExecutionObj.get(); 
        algoExecutionHolder.push_back(std::move(algoExecutionObj)); 
        execOrderHolder.push_back(std::move(execOrder)); 

        for (auto& l : listeners) {
            l->ProcessAdd(*algoExecutionData[key]);
        }
    }

    AlgoExecutionServiceListener<T>* GetAlgoExecutionServiceListener() {
        return algoexecservicelistener;
    }

private:
    std::map<std::string, AlgoExecution<T>*> algoExecutionData;
    std::vector<std::unique_ptr<AlgoExecution<T>>> algoExecutionHolder;
    std::vector<std::unique_ptr<ExecutionOrder<T>>> execOrderHolder;
    std::vector<ServiceListener<AlgoExecution<T>>*> listeners;
    AlgoExecutionServiceListener<T>* algoexecservicelistener;
    long count;

    std::unique_ptr<IAlgoOrderFactory<T>> orderFactory; 
};

#endif
