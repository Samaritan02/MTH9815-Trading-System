// BaseService.hpp
//
// Provides a generic implementation of a service for managing key-value data and listeners.
// Acts as a foundation for building more specialized services in a trading system.
//
// @class BaseService
// @description This templated base class implements the `Service` interface, offering core functionality
//              for managing data storage and listener notifications. It is designed to be extended for
//              specific use cases.
//
// @methods 
// - GetData: Retrieves the data associated with a given key.
// - OnMessage: Processes incoming messages (default implementation is a no-op).
// - AddListener: Adds a listener to observe changes in the service.
// - GetListeners: Retrieves a list of registered listeners.
//
// @attributes
// - dataMap: A map storing key-value pairs representing the service's data.
// - listeners: A vector of listeners observing changes to the service's data.
//
// @date 2024-12-20
// @version 1.1
//
// @notes This class is templated and can be customized for specific key and value types.
// @author Junhao Yu

#ifndef BASESERVICE_HPP
#define BASESERVICE_HPP

#include "soa.hpp"
#include <map>
#include <vector>
#include <stdexcept>

template<typename Key, typename Value>
class BaseService : public Service<Key, Value> {
public:
    virtual ~BaseService() = default;

    Value& GetData(Key key) override {
        auto it = dataMap.find(key);
        if (it == dataMap.end()) {
            throw std::runtime_error("Key not found");
        }
        return it->second;
    }

    void OnMessage(Value& data) override {
        // default no-op
    }

    void AddListener(ServiceListener<Value>* listener) override {
        listeners.push_back(listener);
    }

    const std::vector<ServiceListener<Value>*>& GetListeners() const override {
        return listeners;
    }

protected:
    std::map<Key, Value> dataMap; 
    std::vector<ServiceListener<Value>*> listeners;
};

#endif
