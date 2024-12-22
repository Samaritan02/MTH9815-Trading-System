// IAlgoExecutionServiceListener.hpp
//
// Defines an interface for listeners monitoring algorithmic execution service events.
//
// @class IAlgoExecutionServiceListener
// @description This interface extends `ServiceListener` to listen for events involving
//              market data such as order books, enabling integration with algorithmic execution services.
//
// @methods 
// - Inherits all methods from `ServiceListener`, such as `ProcessAdd`, `ProcessRemove`, and `ProcessUpdate`.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOEXECUTIONSERVICELISTENER_HPP
#define IALGOEXECUTIONSERVICELISTENER_HPP

#include "soa.hpp"
#include "marketdataservice.hpp" // for OrderBook<T>

template<typename T>
class IAlgoExecutionServiceListener : public ServiceListener<OrderBook<T>> {
public:
    virtual ~IAlgoExecutionServiceListener() = default;
};

#endif
