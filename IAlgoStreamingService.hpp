// IAlgoStreamingService.hpp
//
// Defines an interface for a service that publishes algorithmic price streams.
//
// @class IAlgoStreamingService
// @description This interface extends the `Service` class and provides a method for publishing
//              algorithmic price streams based on pricing data.
//
// @methods 
// - PublishAlgoStream: Publishes an algorithmic price stream using the provided price data.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOSTREAMINGSERVICE_HPP
#define IALGOSTREAMINGSERVICE_HPP

#include "soa.hpp"
#include "AlgoStream.hpp"
#include "pricingservice.hpp" // for Price<T>

template<typename T>
class IAlgoStreamingService : public Service<std::string, IAlgoStream<T>> {
public:
    virtual ~IAlgoStreamingService() = default;
    virtual void PublishAlgoStream(const Price<T>& price) = 0;
};

#endif
