// IAlgoExecutionService.hpp
//
// Defines an interface for an algorithmic execution service, managing and executing orders.
//
// @class IAlgoExecutionService
// @description This interface extends the `Service` class and provides a method for executing orders
//              using market data such as order books.
//
// @methods 
// - AlgoExecuteOrder: Executes an algorithmic order based on the provided order book.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOEXECUTIONSERVICE_HPP
#define IALGOEXECUTIONSERVICE_HPP

#include "soa.hpp"
#include "IAlgoExecution.hpp"
#include "marketdataservice.hpp" // for OrderBook<T>

template<typename T>
class IAlgoExecutionService : public Service<std::string, IAlgoExecution<T>> {
public:
    virtual ~IAlgoExecutionService() = default;
    virtual void AlgoExecuteOrder(OrderBook<T>& orderBook) = 0;
};

#endif
