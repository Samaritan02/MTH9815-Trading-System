// historicaldataservice.hpp
//
// Provides a HistoricalDataService class and associated connector and listener classes.
// This service is designed for persisting historical data into external storage, keyed by a given identifier.
//
// @class HistoricalDataService
// @description Manages persistence of data across various service types including Position, Risk, Execution,
//              Streaming, and Inquiry. Supports data addition, listener notification, and external publishing.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu

#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "soa.hpp"
#include "streamingservice.hpp"
#include "riskservice.hpp"
#include "executionservice.hpp"
#include "inquiryservice.hpp"
#include "positionservice.hpp"
#include "TimeUtils.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

// Enumeration identifying the category of service data to be persisted.
enum ServiceType {POSITION, RISK, EXECUTION, STREAMING, INQUIRY};

// Forward declarations for connector and listener classes.
template<typename T>
class HistoricalDataConnector;

template<typename T>
class HistoricalDataServiceListener;

/**
 * HistoricalDataService class
 * Handles data persistence for historical records, including publishing to external storage and maintaining an internal cache.
 *
 * Template parameter T represents the data type being persisted.
 */
template<typename T>
class HistoricalDataService : public Service<std::string, T>
{
public:
    HistoricalDataService(ServiceType _type);
    ~HistoricalDataService() = default;

    T& GetData(std::string key) override;
    void OnMessage(T& data) override;
    void AddListener(ServiceListener<T>* listener) override;
    const std::vector<ServiceListener<T>*>& GetListeners() const override;
    HistoricalDataServiceListener<T>* GetHistoricalDataServiceListener();
    HistoricalDataConnector<T>* GetConnector();
    ServiceType GetServiceType() const;
    void PersistData(std::string persistKey, T& data);

private:
    std::map<std::string, T> hisData;                 // Internal container for persistent data
    std::vector<ServiceListener<T>*> listeners;       // Registered listeners
    HistoricalDataConnector<T>* connector;            // Connector for external storage
    ServiceType type;                                 // Type of data managed by this service
    HistoricalDataServiceListener<T>* historicalservicelistener; // Associated listener
};

template<typename T>
HistoricalDataService<T>::HistoricalDataService(ServiceType _type)
    : type(_type),
      historicalservicelistener(new HistoricalDataServiceListener<T>(this)),
      connector(new HistoricalDataConnector<T>(this))
{
}

template<typename T>
T& HistoricalDataService<T>::GetData(std::string key)
{
    auto it = hisData.find(key);
    if (it != hisData.end())
    {
        return it->second;
    }
    throw std::runtime_error("Key not found");
}

template<typename T>
void HistoricalDataService<T>::OnMessage(T& data)
{
    // No processing required, publish-only service
}

template<typename T>
void HistoricalDataService<T>::AddListener(ServiceListener<T>* listener)
{
    listeners.push_back(listener);
}

template<typename T>
const std::vector<ServiceListener<T>*>& HistoricalDataService<T>::GetListeners() const
{
    return listeners;
}

template<typename T>
HistoricalDataServiceListener<T>* HistoricalDataService<T>::GetHistoricalDataServiceListener()
{
    return historicalservicelistener;
}

template<typename T>
HistoricalDataConnector<T>* HistoricalDataService<T>::GetConnector()
{
    return connector;
}

template<typename T>
ServiceType HistoricalDataService<T>::GetServiceType() const
{
    return type;
}

template<typename T>
void HistoricalDataService<T>::PersistData(std::string persistKey, T& data)
{
    if (hisData.find(persistKey) == hisData.end())
        hisData.insert(std::make_pair(persistKey, data));
    else
        hisData[persistKey] = data;
    connector->Publish(data);
}

/**
 * HistoricalDataConnector class
 * Responsible for persisting data to external storage.
 */
template<typename T>
class HistoricalDataConnector : public Connector<T>
{
public:
    explicit HistoricalDataConnector(HistoricalDataService<T>* _service);
    void Publish(T& data) override;

private:
    HistoricalDataService<T>* service;
};

template<typename T>
HistoricalDataConnector<T>::HistoricalDataConnector(HistoricalDataService<T>* _service)
    : service(_service)
{
}

template<typename T>
void HistoricalDataConnector<T>::Publish(T& data)
{
    std::ofstream outFile;
    std::string fileName;

    switch (service->GetServiceType())
    {
        case POSITION: fileName = "./result/positions.txt"; break;
        case RISK: fileName = "./result/risk.txt"; break;
        case EXECUTION: fileName = "./result/executions.txt"; break;
        case STREAMING: fileName = "./result/streaming.txt"; break;
        case INQUIRY: fileName = "./result/allinquiries.txt"; break;
    }

    outFile.open(fileName, std::ios::app);
    if (outFile.is_open())
    {
        outFile << TimeUtils::GetCurrentTime() << "," << data << std::endl;
    }
    outFile.close();
}

/**
 * HistoricalDataServiceListener class
 * Receives data from various services and pushes it into the HistoricalDataService for persistence.
 */
template<typename T>
class HistoricalDataServiceListener : public ServiceListener<T>
{
public:
    explicit HistoricalDataServiceListener(HistoricalDataService<T>* _service);

    void ProcessAdd(Position<Bond>& data);
    void ProcessAdd(PV01<Bond>& data);
    void ProcessAdd(PriceStream<Bond>& data);
    void ProcessAdd(ExecutionOrder<Bond>& data);
    void ProcessAdd(Inquiry<Bond>& data);
    void ProcessRemove(T& data) override;
    void ProcessUpdate(T& data) override;

private:
    HistoricalDataService<T>* service;
};

template<typename T>
HistoricalDataServiceListener<T>::HistoricalDataServiceListener(HistoricalDataService<T>* _service)
    : service(_service)
{
}

// Helper methods for each data type
#define PROCESS_ADD_IMPL(DATA_TYPE, KEY_GETTER) \
    template<typename T> \
    void HistoricalDataServiceListener<T>::ProcessAdd(DATA_TYPE& data) \
    { \
        std::string persistKey = data.KEY_GETTER(); \
        service->PersistData(persistKey, data); \
    }

PROCESS_ADD_IMPL(Position<Bond>, GetProduct().GetProductId)
PROCESS_ADD_IMPL(PV01<Bond>, GetProduct().GetProductId)
PROCESS_ADD_IMPL(PriceStream<Bond>, GetProduct().GetProductId)
PROCESS_ADD_IMPL(ExecutionOrder<Bond>, GetOrderId)
PROCESS_ADD_IMPL(Inquiry<Bond>, GetInquiryId)

template<typename T>
void HistoricalDataServiceListener<T>::ProcessRemove(T& data)
{
    // No removal logic implemented
}

template<typename T>
void HistoricalDataServiceListener<T>::ProcessUpdate(T& data)
{
    // No update logic implemented
}

#endif
