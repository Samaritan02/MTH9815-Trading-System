// AlgoExecutionServiceListener.hpp
//
// Acts as a listener for handling events related to order books and forwarding them to the AlgoExecutionService.
// This enables seamless integration between the market data service and the algorithmic execution service.
//
// @class AlgoExecutionServiceListener
// @description This class listens for changes in order books and triggers appropriate methods in the associated
//              `AlgoExecutionService`. It handles additions, removals, and updates, although only additions
//              are currently implemented.
//
// @methods 
// - ProcessAdd: Processes the addition of new order book data and invokes AlgoExecuteOrder on the service.
// - ProcessRemove: Placeholder for processing removal of order book data (not implemented).
// - ProcessUpdate: Placeholder for processing updates to order book data (not implemented).
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOEXECUTIONSERVICELISTENER_HPP
#define ALGOEXECUTIONSERVICELISTENER_HPP

#include "IAlgoExecutionServiceListener.hpp"
#include "IAlgoExecutionService.hpp"
#include "marketdataservice.hpp"

template<typename T>
class AlgoExecutionServiceListener : public IAlgoExecutionServiceListener<T>
{
public:
    explicit AlgoExecutionServiceListener(IAlgoExecutionService<T>* service) : service(service) {}
    virtual ~AlgoExecutionServiceListener() = default;

    void ProcessAdd(OrderBook<T> &data) override {
        service->AlgoExecuteOrder(data);
    }
    void ProcessRemove(OrderBook<T> &data) override {}
    void ProcessUpdate(OrderBook<T> &data) override {}

private:
    IAlgoExecutionService<T>* service;
};

#endif
