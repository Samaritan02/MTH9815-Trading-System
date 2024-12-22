// StreamingService.hpp
//
// Manages price streams and integrates with algorithmic streams and connectors to handle data flow and updates.
//
// @class StreamingService
// @description Manages a collection of price streams, facilitates publishing updates, and integrates with listeners
//              and connectors for real-time data processing.
//
// @class StreamingServiceConnector
// @description Handles outbound connections for streaming price data, allowing the publication of price streams.
//
// @class StreamingServiceListener
// @description Listens for updates from algorithmic streams and forwards them to the streaming service.
//
// @methods (StreamingService)
// - GetData: Retrieves a price stream by its product identifier.
// - OnMessage: Placeholder for handling incoming messages (not implemented).
// - AddListener: Registers a listener for updates.
// - GetListeners: Retrieves all registered listeners.
// - GetStreamingServiceListener: Provides access to the service listener.
// - GetConnector: Provides access to the associated connector.
// - PublishPrice: Publishes a price stream using the connector.
// - AddPriceStream: Adds or updates a price stream and notifies listeners.
//
// @methods (StreamingServiceConnector)
// - Publish: Outputs a formatted representation of the price stream to the console.
//
// @methods (StreamingServiceListener)
// - ProcessAdd: Forwards algorithmic stream data to the streaming service for processing and publication.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu


#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include "soa.hpp"
#include "PriceStream.hpp"
#include "AlgoStream.hpp"

template<typename T>
class StreamingServiceConnector;

template<typename T>
class StreamingServiceListener;

template<typename T>
class StreamingService : public Service<std::string, PriceStream<T>> {
public:
    StreamingService();
    ~StreamingService() = default;

    PriceStream<T>& GetData(std::string key) override;
    void OnMessage(PriceStream<T>& data) override {}
    void AddListener(ServiceListener<PriceStream<T>>* listener) override;
    const std::vector<ServiceListener<PriceStream<T>>*>& GetListeners() const override;

    StreamingServiceListener<T>* GetStreamingServiceListener();
    StreamingServiceConnector<T>* GetConnector();

    void PublishPrice(const PriceStream<T>& priceStream);
    void AddPriceStream(const AlgoStream<T>& algoStream);

private:
    std::map<std::string, PriceStream<T>> priceStreamData;
    std::vector<ServiceListener<PriceStream<T>>*> listeners;
    StreamingServiceConnector<T>* connector;
    StreamingServiceListener<T>* streamingServiceListener;
};

template<typename T>
StreamingService<T>::StreamingService()
    : streamingServiceListener(new StreamingServiceListener<T>(this)) {}

template<typename T>
PriceStream<T>& StreamingService<T>::GetData(std::string key) {
    auto it = priceStreamData.find(key);
    if (it == priceStreamData.end()) {
        throw std::runtime_error("Key not found");
    }
    return it->second;
}

template<typename T>
void StreamingService<T>::AddListener(ServiceListener<PriceStream<T>>* listener) {
    listeners.push_back(listener);
}

template<typename T>
const std::vector<ServiceListener<PriceStream<T>>*>& StreamingService<T>::GetListeners() const {
    return listeners;
}

template<typename T>
StreamingServiceListener<T>* StreamingService<T>::GetStreamingServiceListener() {
    return streamingServiceListener;
}

template<typename T>
StreamingServiceConnector<T>* StreamingService<T>::GetConnector() {
    return connector;
}

template<typename T>
void StreamingService<T>::PublishPrice(const PriceStream<T>& priceStream) {
    // if (connector) {
    //     connector->Publish(priceStream);
    // }
    connector->Publish(priceStream);
}

template<typename T>
void StreamingService<T>::AddPriceStream(const AlgoStream<T>& algoStream) {
    PriceStream<T> priceStream = algoStream.GetPriceStream();
    string key = priceStream.GetProduct().GetProductId();
    // update the price stream map
    if (priceStreamData.find(key) != priceStreamData.end()) 
    {
        priceStreamData.erase(key);
    }
    priceStreamData.insert(pair<string, PriceStream<T> > (key, priceStream));

    // flow the data to listeners
    for (auto& l : listeners) {
        l -> ProcessAdd(priceStream);
    }
}

template<typename T>
class StreamingServiceConnector : public Connector<PriceStream<T>> {
public:
    explicit StreamingServiceConnector(StreamingService<T>* service);
    ~StreamingServiceConnector() = default;

    void Publish(const PriceStream<T>& data);

private:
    StreamingService<T>* service;
};

template<typename T>
StreamingServiceConnector<T>::StreamingServiceConnector(StreamingService<T>* service)
    : service(service) {}

template<typename T>
void StreamingServiceConnector<T>::Publish(const PriceStream<T>& data) {
    const auto& product = data.GetProduct();
    const auto& productId = product.GetProductId();
    const auto& bid = data.GetBidOrder();
    const auto& offer = data.GetOfferOrder();

    std::cout << "Price Stream (Product " << productId << "):\n"
              << "\tBid\tPrice: " << bid.GetPrice()
              << "\tVisibleQuantity: " << bid.GetVisibleQuantity()
              << "\tHiddenQuantity: " << bid.GetHiddenQuantity() << "\n"
              << "\tAsk\tPrice: " << offer.GetPrice()
              << "\tVisibleQuantity: " << offer.GetVisibleQuantity()
              << "\tHiddenQuantity: " << offer.GetHiddenQuantity() << "\n";
}


template<typename T>
class StreamingServiceListener : public ServiceListener<AlgoStream<T>> {
public:
    explicit StreamingServiceListener(StreamingService<T>* streamingService);

    void ProcessAdd(AlgoStream<T>& data) override;
    void ProcessRemove(AlgoStream<T>& data) override {}
    void ProcessUpdate(AlgoStream<T>& data) override {}

private:
    StreamingService<T>* streamingService;
};

template<typename T>
StreamingServiceListener<T>::StreamingServiceListener(StreamingService<T>* streamingService)
    : streamingService(streamingService) {}

template<typename T>
void StreamingServiceListener<T>::ProcessAdd(AlgoStream<T>& data) {
    streamingService->AddPriceStream(data);
    streamingService->PublishPrice(data.GetPriceStream());
}

#endif
