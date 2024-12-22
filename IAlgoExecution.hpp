// IAlgoExecution.hpp
//
// Defines an interface for algorithmic execution, encapsulating execution orders and market information.
//
// @class IAlgoExecution
// @description This interface provides a blueprint for algorithmic execution objects, requiring methods to
//              access execution order details and associated market information.
//
// @methods 
// - GetExecutionOrder: Retrieves the execution order associated with the algorithmic execution.
// - GetMarket: Retrieves the market associated with the execution.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOEXECUTION_HPP
#define IALGOEXECUTION_HPP

#include "IExecutionOrder.hpp"

template<typename T>
class IAlgoExecution {
public:
    virtual ~IAlgoExecution() = default;
    virtual const IExecutionOrder<T>& GetExecutionOrder() const = 0;
    virtual Market GetMarket() const = 0;
};

#endif
