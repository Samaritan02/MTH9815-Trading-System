// IAlgoStreamingServiceListener.hpp
//
// Defines an interface for listeners that handle events related to algorithmic streaming services.
//
// @class IAlgoStreamingServiceListener
// @description This interface extends `ServiceListener` to listen for events involving price data,
//              enabling integration with algorithmic streaming services.
//
// @methods 
// - Inherits methods from `ServiceListener`, including `ProcessAdd`, `ProcessRemove`, and `ProcessUpdate`.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOSTREAMINGSERVICELISTENER_HPP
#define IALGOSTREAMINGSERVICELISTENER_HPP

#include "soa.hpp"
#include "pricingservice.hpp" // Price<T>

template<typename T>
class IAlgoStreamingServiceListener : public ServiceListener<Price<T>> {
public:
    virtual ~IAlgoStreamingServiceListener() = default;
};

#endif
