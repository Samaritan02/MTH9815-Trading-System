// AlgoExecution.hpp
//
// Implements an algorithmic execution strategy for financial orders.
// Provides methods to handle execution orders and associated market contexts.
//
// @class AlgoExecution
// @description This class template defines an algorithmic execution strategy,
//              storing execution orders and market information. It interfaces
//              with `IAlgoExecution` for extensibility and abstraction.
//
// @methods 
// - GetExecutionOrder: Retrieves the execution order reference.
// - GetMarket: Retrieves the market context.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOEXECUTION_HPP
#define ALGOEXECUTION_HPP

#include "IAlgoExecution.hpp"
#include "ExecutionOrder.hpp"

template<typename T>
class AlgoExecution : public IAlgoExecution<T>
{
public:
    AlgoExecution() = default;
    AlgoExecution(const ExecutionOrder<T>& executionOrder, Market market)
        : executionOrderRef(executionOrder), market(market) {}
        
    virtual ~AlgoExecution() = default;

    const ExecutionOrder<T>& GetExecutionOrder() const override { return executionOrderRef; }
    Market GetMarket() const override { return market; }

private:
    // Store a reference or a copy of the execution order
    // Here we store a copy for simplicity
    ExecutionOrder<T> const& executionOrderRef;
    Market market;
};

#endif
