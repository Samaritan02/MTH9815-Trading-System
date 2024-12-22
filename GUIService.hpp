// GUIService.hpp
//
// Manages and publishes price data to a GUI with throttling support.
//
// @class GUIService
// @description This class handles the flow of price data to a GUI, using a connector and a throttling mechanism
//              to regulate updates.
//
// @methods 
// - OnMessage: Placeholder for processing incoming price data (not implemented).
// - GetGUIServiceListener: Retrieves the service listener for GUI-specific events.
// - GetConnector: Retrieves the connector for publishing price data.
// - GetThrottle: Returns the current throttling interval in milliseconds.
// - PublishThrottledPrice: Publishes price data to the GUI if the throttling condition is met.
//
// @date 2024-12-20
// @version 1.1
//
// @attributes
// - throttle: Time interval in milliseconds for throttling price updates.
// - startTime: Tracks the last time a price was published.
//
// @notes Throttling helps avoid excessive updates to the GUI for performance efficiency.
//
// @author Junhao Yu

#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP

#include "BaseService.hpp"
#include "pricingservice.hpp"

template<typename T>
class GUIConnector;
template<typename T>
class GUIServiceListener;

template<typename T>
class GUIService : public BaseService<std::string, Price<T>>  
{
public:
    GUIService();
    ~GUIService() = default;

    void OnMessage(Price<T>& data) override {
    }

    GUIServiceListener<T>* GetGUIServiceListener();
    GUIConnector<T>* GetConnector();
    int GetThrottle() const;

    void PublishThrottledPrice(Price<T>& price);

private:
    GUIConnector<T>* connector;
    GUIServiceListener<T>* guiservicelistener;
    int throttle;
    std::chrono::system_clock::time_point startTime;
};

#include "GUIConnector.hpp"         
#include "GUIServiceListener.hpp"  

template<typename T>
GUIService<T>::GUIService() :
    connector(new GUIConnector<T>(this)), 
    guiservicelistener(new GUIServiceListener<T>(this)), 
    throttle(300), 
    startTime(std::chrono::system_clock::now())
{
}

template<typename T>
GUIServiceListener<T>* GUIService<T>::GetGUIServiceListener()
{
    return guiservicelistener;
}

template<typename T>
GUIConnector<T>* GUIService<T>::GetConnector()
{
    return connector;
}

template<typename T>
int GUIService<T>::GetThrottle() const
{
    return throttle;
}

template<typename T>
void GUIService<T>::PublishThrottledPrice(Price<T>& price)
{
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    if (diff.count() > throttle) {
        startTime = now;
        connector->Publish(price);
    }
}

#endif
