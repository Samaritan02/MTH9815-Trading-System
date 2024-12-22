// AlgoStreamingServiceListener.hpp
//
// Listens for price updates and triggers the publication of algorithmic streaming data.
// Acts as an intermediary between the pricing service and the algorithmic streaming service.
//
// @class AlgoStreamingServiceListener
// @description This class listens for events related to price updates and forwards them
//              to the `AlgoStreamingService` to generate and publish streaming data.
//
// @methods 
// - ProcessAdd: Processes the addition of a price update and invokes `PublishAlgoStream` on the service.
// - ProcessRemove: Placeholder for handling removal of price updates (not implemented).
// - ProcessUpdate: Placeholder for handling updates to existing prices (not implemented).
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOSTREAMINGSERVICELISTENER_HPP
#define ALGOSTREAMINGSERVICELISTENER_HPP

#include "IAlgoStreamingServiceListener.hpp"
#include "IAlgoStreamingService.hpp"

template<typename T>
class AlgoStreamingServiceListener : public IAlgoStreamingServiceListener<T>
{
public:
    explicit AlgoStreamingServiceListener(IAlgoStreamingService<T>* service) : service(service) {}
    virtual ~AlgoStreamingServiceListener() = default;

    void ProcessAdd(Price<T> &price) override {
        service->PublishAlgoStream(price);
    }
    void ProcessRemove(Price<T> &price) override {}
    void ProcessUpdate(Price<T> &price) override {}

private:
    IAlgoStreamingService<T>* service;
};

#endif
