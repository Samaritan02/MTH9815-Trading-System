// IAlgoOrderFactory.hpp
//
// Defines an interface for factories that create algorithmic execution orders.
//
// @class IAlgoOrderFactory
// @description This interface provides a method for creating execution orders based on market data
//              such as order books and a unique count identifier.
//
// @methods 
// - CreateExecutionOrder: Creates and returns a unique pointer to an `ExecutionOrder` based on the provided
//                         order book and count.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOORDERFACTORY_HPP
#define ALGOORDERFACTORY_HPP

#include "ExecutionOrder.hpp"

template<typename T>
class IAlgoOrderFactory {
public:
    virtual ~IAlgoOrderFactory() = default;
    virtual std::unique_ptr<ExecutionOrder<T>> CreateExecutionOrder(const OrderBook<T>& orderBook, long count) = 0;
};

#endif
