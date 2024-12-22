// // riskservice.hpp
// //
// // Purpose: 1. Defines the data types and Service for fixed income risk.
// // 
// // @author Breman Thuraisingham
// // @coauthor Yuanting Li
// // @version 2.0 2023/12/22 

// #ifndef RISK_SERVICE_HPP
// #define RISK_SERVICE_HPP

// #include "soa.hpp"
// #include "positionservice.hpp"
// #include "BondAnalytics.hpp"

// /**
//  * PV01 risk.
//  * Type T is the product type.
//  */
// template<typename T>
// class PV01
// {

// public:

//   // Default ctor
//   PV01() = default;

//   // ctor for a PV01 value
//   PV01(const T &_product, double _pv01, long _quantity);

//   // dtor
//   ~PV01() = default;

//   // Get the product on this PV01 value
//   const T& GetProduct() const;

//   // Get the PV01 value
//   double GetPV01() const;

//   // Get the quantity that this risk value is associated with
//   long GetQuantity() const;

//   // Add quantity associated with this risk value
//   void updateQuantity(long _quantity);

//   // Object printer
//   template<typename S>
//   friend ostream& operator<<(ostream& os, const PV01<S>& pv01);

// private:
//   T product;
//   double pv01;
//   long quantity;

// };

// template<typename T>
// PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
//   product(_product)
// {
//   pv01 = _pv01;
//   quantity = _quantity;
// }

// template<typename T>
// const T& PV01<T>::GetProduct() const
// {
//   return product;
// }

// template<typename T>
// double PV01<T>::GetPV01() const
// {
//   return pv01;
// }

// template<typename T>
// long PV01<T>::GetQuantity() const
// {
//   return quantity;
// }

// template<typename T>
// void PV01<T>::updateQuantity(long _quantity)
// {
//   quantity += _quantity;
// }

// template<typename T>
// ostream& operator<<(ostream& output, const PV01<T>& pv01)
// {
//     T product = pv01.GetProduct();
//     string productId = product.GetProductId();
//     double pv01Value = pv01.GetPV01();
//     long quantity = pv01.GetQuantity();

//     output << productId << "," << pv01Value << "," << quantity;

//     return output;
// }

// /**
//  * A bucket sector to bucket a group of securities.
//  * We can then aggregate bucketed risk to this bucket.
//  * Type T is the product type.
//  */
// template<typename T>
// class BucketedSector
// {
// public:
//   // ctor for a bucket sector
//   BucketedSector(const vector<T> &_products, string _name);

//   // Get the products associated with this bucket
//   const vector<T>& GetProducts() const;

//   // Get the name of the bucket
//   const string& GetName() const;

// private:
//   vector<T> products;
//   string name;

// };

// template<typename T>
// BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
//   products(_products)
// {
//   name = _name;
// }

// template<typename T>
// const vector<T>& BucketedSector<T>::GetProducts() const
// {
//   return products;
// }

// template<typename T>
// const string& BucketedSector<T>::GetName() const
// {
//   return name;
// }

// // forward declaration of RiskServiceListener
// template<typename T>
// class RiskServiceListener;


// /**
//  * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
//  * Keyed on product identifier.
//  * Type T is the product type.
//  */
// template<typename T>
// class RiskService : public Service<string,PV01 <T> >
// {
// private:
//   vector<ServiceListener<PV01<T>>*> listeners;
//   map<string, PV01<T>> pv01Data;
//   RiskServiceListener<T>* riskservicelistener;

// public:
//   // ctor and dtor
//   RiskService();
//   ~RiskService() = default;

//   // Get data on our service given a key
//   PV01<T>& GetData(string key);

//   // The callback that a Connector should invoke for any new or updated data
//   void OnMessage(PV01<T> &data);

//   // Add a listener to the Service for callbacks on add, remove, and update events
//   // for data to the Service.
//   void AddListener(ServiceListener<PV01<T>> *listener);

//   // Get all listeners on the Service.
//   const vector< ServiceListener<PV01<T>>* >& GetListeners() const;

//   // Get the special listener for risk service
//   RiskServiceListener<T>* GetRiskServiceListener();

//   // Add a position that the service will risk
//   void AddPosition(Position<T> &position);

//   // Get the bucketed risk for the bucket sector
//   const PV01< BucketedSector<T> >& GetBucketedRisk(const BucketedSector<T> &sector) const;

// };

// template<typename T>
// RiskService<T>::RiskService() : riskservicelistener(new RiskServiceListener<T>(this))
// {
// }

// template<typename T>
// PV01<T>& RiskService<T>::GetData(string key)
// {
//     auto it = pv01Data.find(key);
//     if (it != pv01Data.end())
//     {
//         return it->second;
//     }
//     else
//     {
//         throw std::runtime_error("Key not found");
//     }
// }

// /**
//  * OnMessage() used to be called by connector to subscribe data
//  * no need to implement here.
//  */
// template<typename T>
// void RiskService<T>::OnMessage(PV01<T> &data)
// {
// }

// template<typename T>
// void RiskService<T>::AddListener(ServiceListener<PV01<T>> *listener)
// {
//   listeners.push_back(listener);
// }

// template<typename T>
// const vector< ServiceListener<PV01<T>>* >& RiskService<T>::GetListeners() const
// {
//   return listeners;
// }

// template<typename T>
// RiskServiceListener<T>* RiskService<T>::GetRiskServiceListener()
// {
//   return riskservicelistener;
// }

// template<typename T>
// void RiskService<T>::AddPosition(Position<T> &position)
// {
//   T product = position.GetProduct();
//   string productId = product.GetProductId();
//   long quantity = position.GetAggregatePosition();
//   // note: this gives the PV01 value for a single unit
//   // double pv01Val = QueryPV01(productId);
//   double pv01Val = BondAnalytics::QueryPV01(productId);

//   // create a PV01 object and publish it to the service
//   PV01<T> pv01(product, pv01Val, quantity);
//   if (pv01Data.find(productId) != pv01Data.end()){
//     pv01Data[productId].updateQuantity(quantity);
//   }else{
//     pv01Data.insert(pair<string, PV01<T>>(productId, pv01));
//   }

//   // notify listeners
//   for(auto& listener : listeners)
//     listener->ProcessAdd(pv01);
// }

// template<typename T>
// const PV01<BucketedSector<T>>& RiskService<T>::GetBucketedRisk(const BucketedSector<T> &sector) const
// {
//   // create a bucketed sector object and publish it to the service
//   vector<T>& products = sector.GetProducts();
//   string name = sector.GetName();
//   double pv01Val = 0.0;
//   long quantity = 0;
//   for (auto& product : products){
//     string productId = product.GetProductId();
//     if (pv01Data.find(productId) != pv01Data.end()){
//       // total pv01 value for the sector is the weighted average
//       pv01Val += pv01Data[productId].GetPV01()*pv01Data[productId].GetQuantity();
//       // total quantity for the sector
//       quantity += pv01Data[productId].GetQuantity();
//     }
//   }
//   // note: for PV01 object of a sector, we store the total PV01 value instead of a single unit
//   PV01<BucketedSector<T>> pv01(sector, pv01Val, quantity);
//   return pv01;
// }


// /**
// * Risk Service Listener subscribing data from Position Service to Risk Service.
// * Type T is the product type.
// */
// template<typename T>
// class RiskServiceListener : public ServiceListener<Position<T>>
// {
// private:
//   RiskService<T>* riskservice;

// public:
//   // ctor and dtor
//   RiskServiceListener(RiskService<T>* _riskservice);
//   ~RiskServiceListener()=default;

//   // Listener callback to process an add event to the Service
//   void ProcessAdd(Position<T> &data);

//   // Listener callback to process a remove event to the Service
//   void ProcessRemove(Position<T> &data);

//   // Listener callback to process an update event to the Service
//   void ProcessUpdate(Position<T> &data);

// };

// template<typename T>
// RiskServiceListener<T>::RiskServiceListener(RiskService<T>* _riskservice)
// {
//   riskservice = _riskservice;
// }

// /**
//  * ProcessAdd() method is used to transfer Position<T> data to PV01<T> data,
//  * then publish the PV01<T> data to Risk Service.
//  */
// template<typename T>
// void RiskServiceListener<T>::ProcessAdd(Position<T> &data)
// {
//   riskservice->AddPosition(data);
// }

// template<typename T>
// void RiskServiceListener<T>::ProcessRemove(Position<T> &data)
// {
// }

// template<typename T>
// void RiskServiceListener<T>::ProcessUpdate(Position<T> &data)
// {
// }



// #endif

// RiskService.hpp
//
// Provides functionality for managing and calculating PV01 risks for financial products, 
// including bucketed risks for grouped securities.
//
// @class PV01
// @description Represents the PV01 risk for a financial product, including the product, PV01 value, and total quantity.
//
// @class BucketedSector
// @description Groups a set of financial products into a bucketed sector for aggregated risk analysis.
//
// @class RiskService
// @description Manages PV01 risks, calculates bucketed risks, and integrates with position data.
//
// @class RiskServiceListener
// @description Links the PositionService with the RiskService, enabling automatic updates to PV01 data.
//
// @methods (PV01)
// - GetProduct: Retrieves the associated product.
// - GetPV01: Returns the PV01 value.
// - GetQuantity: Returns the total quantity.
// - UpdateQuantity: Updates the quantity by adding the provided value.
// - operator<<: Outputs a formatted representation of the PV01 object.
//
// @methods (BucketedSector)
// - GetProducts: Retrieves the products in the sector.
// - GetName: Returns the name of the sector.
//
// @methods (RiskService)
// - GetData: Retrieves PV01 data for a specific product.
// - OnMessage: Placeholder for handling inbound PV01 messages.
// - AddListener: Registers a listener for PV01 updates.
// - GetListeners: Retrieves all registered listeners.
// - GetRiskServiceListener: Returns the associated risk service listener.
// - AddPosition: Updates PV01 data using position information.
// - GetBucketedRisk: Calculates the aggregated PV01 for a bucketed sector.
//
// @methods (RiskServiceListener)
// - ProcessAdd: Processes new position data to update PV01 risks.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu


#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include "soa.hpp"
#include "positionservice.hpp"
#include "BondAnalytics.hpp"
#include <numeric>

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01 {
public:
    PV01() = default;
    PV01(const T& _product, double _pv01, long _quantity);
    ~PV01() = default;

    const T& GetProduct() const;
    double GetPV01() const;
    long GetQuantity() const;
    void UpdateQuantity(long _quantity);

    template<typename S>
    friend ostream& operator<<(ostream& os, const PV01<S>& pv01);

private:
    T product;
    double pv01;
    long quantity;
};

template<typename T>
PV01<T>::PV01(const T& _product, double _pv01, long _quantity)
    : product(_product), pv01(_pv01), quantity(_quantity) {}

template<typename T>
const T& PV01<T>::GetProduct() const {
    return product;
}

template<typename T>
double PV01<T>::GetPV01() const {
    return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const {
    return quantity;
}

template<typename T>
void PV01<T>::UpdateQuantity(long _quantity) {
    quantity += _quantity;
}

template<typename T>
ostream& operator<<(ostream& os, const PV01<T>& pv01) {
    os << pv01.product.GetProductId() << "," << pv01.pv01 << "," << pv01.quantity;
    return os;
}

/**
 * A bucket sector to group a set of securities.
 */
template<typename T>
class BucketedSector {
public:
    BucketedSector(const vector<T>& _products, string _name);
    const vector<T>& GetProducts() const;
    const string& GetName() const;

private:
    vector<T> products;
    string name;
};

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name)
    : products(_products), name(_name) {}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const {
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const {
    return name;
}

// Forward declaration of RiskServiceListener
template<typename T>
class RiskServiceListener;

/**
 * Risk Service for managing PV01 risks.
 */
template<typename T>
class RiskService : public Service<string, PV01<T>> {
private:
    vector<ServiceListener<PV01<T>>*> listeners;
    map<string, PV01<T>> pv01Data;
    unique_ptr<RiskServiceListener<T>> riskServiceListener;

    double CalculateSectorPV01(const vector<T>& products, long& totalQuantity) const;

public:
    RiskService();
    ~RiskService() = default;

    PV01<T>& GetData(string key) override;
    void OnMessage(PV01<T>& data) override;
    void AddListener(ServiceListener<PV01<T>>* listener) override;
    const vector<ServiceListener<PV01<T>>*>& GetListeners() const override;

    RiskServiceListener<T>* GetRiskServiceListener();
    void AddPosition(Position<T>& position);
    PV01<BucketedSector<T>> GetBucketedRisk(const BucketedSector<T>& sector) const;
};

template<typename T>
RiskService<T>::RiskService()
    : riskServiceListener(make_unique<RiskServiceListener<T>>(this)) {}

template<typename T>
PV01<T>& RiskService<T>::GetData(string key) {
    auto it = pv01Data.find(key);
    if (it != pv01Data.end()) {
        return it->second;
    }
    throw runtime_error("Key not found: " + key);
}

template<typename T>
void RiskService<T>::OnMessage(PV01<T>& data) {
    // Not implemented
}

template<typename T>
void RiskService<T>::AddListener(ServiceListener<PV01<T>>* listener) {
    listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<PV01<T>>*>& RiskService<T>::GetListeners() const {
    return listeners;
}

template<typename T>
RiskServiceListener<T>* RiskService<T>::GetRiskServiceListener() {
    return riskServiceListener.get();
}

template<typename T>
void RiskService<T>::AddPosition(Position<T>& position) {
    const auto& product = position.GetProduct();
    string productId = product.GetProductId();
    long quantity = position.GetAggregatePosition();
    double pv01Value = BondAnalytics::QueryPV01(productId);

    PV01<T> pv01(product, pv01Value, quantity);
    if (pv01Data.find(productId) != pv01Data.end()){
      pv01Data[productId].UpdateQuantity(quantity);
    }else{
      pv01Data.insert(pair<string, PV01<T>>(productId, pv01));
    }


    for (auto* listener : listeners) {
        listener->ProcessAdd(pv01);
    }
}

template<typename T>
PV01<BucketedSector<T>> RiskService<T>::GetBucketedRisk(const BucketedSector<T>& sector) const {
    long totalQuantity = 0;
    double totalPV01 = CalculateSectorPV01(sector.GetProducts(), totalQuantity);
    return PV01<BucketedSector<T>>(sector, totalPV01, totalQuantity);
}

template<typename T>
double RiskService<T>::CalculateSectorPV01(const vector<T>& products, long& totalQuantity) const {
    return accumulate(products.begin(), products.end(), 0.0, [&](double sum, const T& product) {
        const string& productId = product.GetProductId();
        auto it = pv01Data.find(productId);
        if (it != pv01Data.end()) {
            const auto& pv01 = it->second;
            totalQuantity += pv01.GetQuantity();
            return sum + pv01.GetPV01() * pv01.GetQuantity();
        }
        return sum;
    });
}

/**
 * Risk Service Listener to integrate PositionService with RiskService.
 */
template<typename T>
class RiskServiceListener : public ServiceListener<Position<T>> {
private:
    RiskService<T>* riskService;

public:
    explicit RiskServiceListener(RiskService<T>* service);
    void ProcessAdd(Position<T>& data) override;
    void ProcessRemove(Position<T>& data) override {}
    void ProcessUpdate(Position<T>& data) override {}
};

template<typename T>
RiskServiceListener<T>::RiskServiceListener(RiskService<T>* service) : riskService(service) {}

template<typename T>
void RiskServiceListener<T>::ProcessAdd(Position<T>& data) {
    riskService->AddPosition(data);
}

#endif
