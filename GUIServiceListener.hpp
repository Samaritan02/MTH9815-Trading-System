// GUIServiceListener.hpp
//
// Listens for price updates and triggers the GUI service to publish throttled data.
//
// @class GUIServiceListener
// @description This class listens for price events and invokes the corresponding methods in the associated
//              `GUIService` to handle the data appropriately.
//
// @methods 
// - ProcessAdd: Passes new price data to the `GUIService` for throttled publishing.
// - ProcessRemove: Placeholder for handling removal of price data (not implemented).
// - ProcessUpdate: Placeholder for handling updates to price data (not implemented).
//
// @date 2024-12-20
// @version 1.1
//
// @attributes
// - guiService: A pointer to the associated `GUIService` instance.
//
// @author Junhao Yu

#ifndef GUISERVICELISTENER_HPP
#define GUISERVICELISTENER_HPP

#include "soa.hpp"
#include "pricingservice.hpp"

template<typename T>
class GUIService; 

template<typename T>
class GUIServiceListener : public ServiceListener<Price<T>>
{
public:
    GUIServiceListener(GUIService<T>* _guiService) : guiService(_guiService) {}

    void ProcessAdd(Price<T>& price) override {
        guiService->PublishThrottledPrice(price);
    }

    void ProcessRemove(Price<T>& price) override {}
    void ProcessUpdate(Price<T>& price) override {}

private:
    GUIService<T>* guiService;
};

#endif
