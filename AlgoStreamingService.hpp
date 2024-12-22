// AlgoStreamingService.hpp
//
// Manages the lifecycle and publication of algorithmic streaming data in financial systems.
// Facilitates the creation, storage, and dissemination of streaming data to listeners.
//
// @class AlgoStreamingService
// @description This class handles the generation and management of `AlgoStream` objects,
//              which encapsulate `PriceStream` data. It also notifies registered listeners
//              about updates to streaming data.
//
// @methods 
// - GetData: Retrieves streaming data by key.
// - OnMessage: Placeholder for handling incoming messages (not implemented).
// - AddListener: Adds listeners to observe streaming events.
// - GetListeners: Retrieves a list of listeners observing the service.
// - GetAlgoStreamingListener: Provides access to the associated streaming service listener.
// - PublishAlgoStream: Publishes new algorithmic streaming data based on pricing information.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOSTREAMINGSERVICE_HPP
#define ALGOSTREAMINGSERVICE_HPP

#include "IAlgoStreamingService.hpp"
#include "AlgoStreamingServiceListener.hpp"
#include "AlgoStream.hpp"
#include "PriceStream.hpp"
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>

template<typename T>
class AlgoStreamingService : public IAlgoStreamingService<T>
{
public:
    AlgoStreamingService() 
        : algostreamlistener(new AlgoStreamingServiceListener<T>(this)), count(0) {}

    virtual ~AlgoStreamingService() {
        delete algostreamlistener;
    }
    
    IAlgoStream<T>& GetData(std::string key) override {
        auto it = algoStreamData.find(key);
        if (it == algoStreamData.end())
            throw std::runtime_error("Key not found");
        return *(it->second);
    }

    void OnMessage(IAlgoStream<T>& data) override {
        // not needed here
    }

    void AddListener(ServiceListener<IAlgoStream<T>> *listener) override {
    }

    void AddListener(ServiceListener<AlgoStream<T>> *listener) {
        listeners.push_back(listener);
    }

    const std::vector<ServiceListener<IAlgoStream<T>>*>& GetListeners() const override {
        std::vector<ServiceListener<IAlgoStream<T>>*> result;
        for (auto* listener : listeners) {
            result.push_back(dynamic_cast<ServiceListener<IAlgoStream<T>>*>(listener));
        }
        return result;
    }


    AlgoStreamingServiceListener<T>* GetAlgoStreamingListener() {
        return algostreamlistener;
    }

    void PublishAlgoStream(const Price<T>& price) override {
        T product = price.GetProduct();
        std::string key = product.GetProductId();
        double mid = price.GetMid();
        double spread = price.GetBidOfferSpread();
        double bidPrice = mid - spread/2;
        double offerPrice = mid + spread/2;
        
        long visibleQuantity = (count % 2 == 0) ? 1000000 : 2000000;
        long hiddenQuantity = visibleQuantity * 2;
        count++;

        PriceStreamOrder bidOrder(bidPrice, visibleQuantity, hiddenQuantity, BID);
        PriceStreamOrder offerOrder(offerPrice, visibleQuantity, hiddenQuantity, OFFER);

        auto priceStream = std::make_unique<PriceStream<T>>(product, bidOrder, offerOrder);
        auto algoStream = std::make_unique<AlgoStream<T>>(*priceStream);

        if (algoStreamData.find(key) != algoStreamData.end()) {
            algoStreamData.erase(key); 
        }
        algoStreamData[key] = std::move(algoStreamHolder.emplace_back(std::move(algoStream))).get();

        for (auto& listener : listeners) {
            listener->ProcessAdd(*(algoStreamData[key]));
        }

        priceStreamsStorage[key] = std::move(priceStream);
    }

private:
    std::map<std::string, AlgoStream<T>*> algoStreamData;
    std::vector<std::unique_ptr<AlgoStream<T>>> algoStreamHolder; 
    std::map<std::string, std::unique_ptr<PriceStream<T>>> priceStreamsStorage;
    std::vector<ServiceListener<AlgoStream<T>>*> listeners;
    AlgoStreamingServiceListener<T>* algostreamlistener;
    long count;
};

#endif
