// PricingService.hpp
//
// Manages pricing data for financial products, including mid prices and bid/offer spreads.
// Integrates with connectors to subscribe to data feeds and notify listeners of updates.
//
// @class Price
// @description Represents a price object containing mid prices and bid/offer spreads.
//
// @class PricingService
// @description Manages a collection of prices, keyed by product identifiers, and notifies listeners of updates.
//
// @class PricingConnector
// @description An inbound connector that subscribes pricing data from external sources and feeds it into the `PricingService`.
//
// @methods (Price)
// - GetProduct: Retrieves the associated product.
// - GetMid: Returns the mid price.
// - GetBidOfferSpread: Returns the bid/offer spread.
// - operator<<: Outputs a formatted representation of the price.
//
// @methods (PricingService)
// - GetData: Retrieves a price object by product identifier.
// - OnMessage: Handles updates to pricing data.
// - AddListener: Registers a listener for price updates.
// - GetListeners: Returns all registered listeners.
// - GetConnector: Provides access to the associated pricing connector.
//
// @methods (PricingConnector)
// - Publish: No-op, as this connector is inbound only.
// - Subscribe: Reads and parses pricing data from an input stream.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu


#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include <map>
#include <fstream>
#include <utility>
#include "soa.hpp"
#include "products.hpp"
#include "PriceUtils.hpp"
#include "ProductFactory.hpp"

/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price {
public:
    Price() = default;
    Price(const T& _product, double _mid, double _bidOfferSpread);
    ~Price() = default;

    const T& GetProduct() const;
    double GetMid() const;
    double GetBidOfferSpread() const;

    template<typename S>
    friend ostream& operator<<(ostream& output, const Price<S>& price);

private:
    T product;
    double mid;
    double bidOfferSpread;
};

template<typename T>
Price<T>::Price(const T& _product, double _mid, double _bidOfferSpread)
    : product(_product), mid(_mid), bidOfferSpread(_bidOfferSpread) {}

template<typename T>
const T& Price<T>::GetProduct() const {
    return product;
}

template<typename T>
double Price<T>::GetMid() const {
    return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const {
    return bidOfferSpread;
}

template<typename T>
ostream& operator<<(ostream& output, const Price<T>& price) {
    output << price.GetProduct().GetProductId() << " Mid: " << price.GetMid()
           << ", Spread: " << price.GetBidOfferSpread();
    return output;
}

// Forward declaration of PricingConnector
template<typename T>
class PricingConnector;

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string, Price<T>> {
private:
    map<string, Price<T>> priceData;
    vector<ServiceListener<Price<T>>*> listeners;
    unique_ptr<PricingConnector<T>> connector;

public:
    PricingService();
    ~PricingService() = default;

    Price<T>& GetData(string key) override;
    void OnMessage(Price<T>& data) override;
    void AddListener(ServiceListener<Price<T>>* listener) override;
    const vector<ServiceListener<Price<T>>*>& GetListeners() const override;
    PricingConnector<T>* GetConnector();
};

template<typename T>
PricingService<T>::PricingService() : connector(make_unique<PricingConnector<T>>(this)) {}

template<typename T>
Price<T>& PricingService<T>::GetData(string key) {
    auto it = priceData.find(key);
    if (it != priceData.end()) {
        return it->second;
    }
    throw runtime_error("Key not found: " + key);
}

template<typename T>
void PricingService<T>::OnMessage(Price<T>& data) {
    string key = data.GetProduct().GetProductId();
    if (priceData.find(key) != priceData.end()) 
    { 
        priceData.erase(key); 
    }
    priceData.insert(pair<string, Price<Bond> >(key, data));

    for (auto& l : listeners) {
        l->ProcessAdd(data);
    }
}

template<typename T>
void PricingService<T>::AddListener(ServiceListener<Price<T>>* listener) {
    listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>*>& PricingService<T>::GetListeners() const {
    return listeners;
}

template<typename T>
PricingConnector<T>* PricingService<T>::GetConnector() {
    return connector.get();
}

/**
 * PricingConnector: an inbound connector that subscribes data from socket to pricing service.
 * Type T is the product type.
 */
template<typename T>
class PricingConnector : public Connector<Price<T>> {
private:
    PricingService<T>* service;

public:
    explicit PricingConnector(PricingService<T>* _service);
    ~PricingConnector() = default;

    void Publish(Price<T>& data) override;
    void Subscribe(ifstream& _data);
};

template<typename T>
PricingConnector<T>::PricingConnector(PricingService<T>* _service)
    : service(_service) {}

template<typename T>
void PricingConnector<T>::Publish(Price<T>& data) {
    // No-op for inbound-only connector
}

template<typename T>
void PricingConnector<T>::Subscribe(ifstream& _data) {
    string line;
    getline(_data, line); // Skip the header

    while (getline(_data, line)) {
        stringstream rawline(line);
        vector<string> splitdata;
        string block;

        while (getline(rawline, block, ',')) {
            splitdata.push_back(block);
        }

        string productID = splitdata[1];
        double bid = PriceUtils::Frac2Price(splitdata[2]);
        double ask = PriceUtils::Frac2Price(splitdata[3]);

        double mid = (bid + ask) / 2.0;
        double spread = ask - bid;

        T product = ProductFactory<T>::QueryProduct(productID);
        Price<T> price(product, mid, spread);

        service->OnMessage(price);
    }
}

#endif
