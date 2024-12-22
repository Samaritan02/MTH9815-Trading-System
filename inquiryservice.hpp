// InquiryService.hpp
//
// Manages inquiries in trading systems, supporting operations like quoting, rejecting, and handling state transitions.
//
// @class Inquiry
// @description Models a client-initiated inquiry about a product, storing details such as side, quantity, price, and state.
//
// @class InquiryService
// @description Handles the lifecycle and operations of inquiries, integrating with connectors and listeners.
//
// @class InquiryConnector
// @description Manages data flow for the `InquiryService`, supporting both publishing and subscription of inquiries.
//
// @methods (Inquiry)
// - GetInquiryId: Returns the unique identifier of the inquiry.
// - GetProduct: Returns the associated product.
// - GetSide: Returns the side (e.g., BUY or SELL).
// - GetQuantity: Returns the inquiry quantity.
// - GetPrice: Returns the inquiry price.
// - GetState: Returns the current state of the inquiry.
// - SetPrice: Sets the price of the inquiry.
// - SetState: Updates the inquiry's state.
//
// @methods (InquiryService)
// - GetData: Retrieves an inquiry by its ID.
// - OnMessage: Handles updates to inquiries, managing state transitions and broadcasting changes.
// - AddListener: Adds a listener to the service.
// - GetListeners: Retrieves all registered listeners.
// - GetConnector: Returns the associated connector.
// - SendQuote: Sends a price quote for an inquiry.
// - RejectInquiry: Marks an inquiry as rejected.
//
// @methods (InquiryConnector)
// - Publish: Publishes an inquiry, updating its state as necessary.
// - Subscribe: Reads and processes inquiries from an input file.
// - SubscribeUpdate: Subscribes to updates for an inquiry.
//
// @attributes
// - inquiryData: Stores inquiries keyed by their unique identifiers.
// - listeners: A vector of listeners for the service.
// - connector: Handles data flow for the service.
//
// @date 2024-12-20
// @version 1.1
//
// @author Breman Thuraisingham
// @coauthor Junhao Yu


#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include "soa.hpp"
#include "tradebookingservice.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

// Different possible states of an inquiry
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * The Inquiry class models a client-initiated request concerning a product.
 * Template parameter T indicates the product type.
 */
template<typename T>
class Inquiry
{
public:
    Inquiry() = default;
    Inquiry(std::string _inquiryId, const T& _product, Side _side, long _quantity, double _price, InquiryState _state);
    ~Inquiry() = default;

    const std::string& GetInquiryId() const;
    const T& GetProduct() const;
    Side GetSide() const;
    long GetQuantity() const;
    double GetPrice() const;
    InquiryState GetState() const;

    void SetPrice(double _price);
    void SetState(InquiryState state);

    template<typename S>
    friend std::ostream& operator<<(std::ostream& output, const Inquiry<S>& inquiry);

private:
    std::string inquiryId;
    T product;
    Side side;
    long quantity;
    double price;
    InquiryState state;
};

template<typename T>
Inquiry<T>::Inquiry(std::string _inquiryId, const T& _product, Side _side, long _quantity, double _price, InquiryState _state)
    : inquiryId(_inquiryId), product(_product), side(_side), quantity(_quantity), price(_price), state(_state)
{
}

template<typename T>
const std::string& Inquiry<T>::GetInquiryId() const { return inquiryId; }

template<typename T>
const T& Inquiry<T>::GetProduct() const { return product; }

template<typename T>
Side Inquiry<T>::GetSide() const { return side; }

template<typename T>
long Inquiry<T>::GetQuantity() const { return quantity; }

template<typename T>
double Inquiry<T>::GetPrice() const { return price; }

template<typename T>
InquiryState Inquiry<T>::GetState() const { return state; }

template<typename T>
void Inquiry<T>::SetPrice(double _price) { price = _price; }

template<typename T>
void Inquiry<T>::SetState(InquiryState _state) { state = _state; }

template<typename T>
std::ostream& operator<<(std::ostream& output, const Inquiry<T>& inquiry)
{
    std::string sideStr = (inquiry.side == BID) ? "BID" : "OFFER";
    std::string stateStr;
    switch (inquiry.state)
    {
    case RECEIVED: stateStr = "RECEIVED"; break;
    case QUOTED: stateStr = "QUOTED"; break;
    case DONE: stateStr = "DONE"; break;
    case REJECTED: stateStr = "REJECTED"; break;
    case CUSTOMER_REJECTED: stateStr = "CUSTOMER_REJECTED"; break;
    }

    output << inquiry.inquiryId << "," << inquiry.product.GetProductId() << "," << sideStr << ","
           << inquiry.quantity << "," << PriceUtils::Price2Frac(inquiry.price) << "," << stateStr;
    return output;
}

/**
 * Forward declaration for the InquiryConnector class.
 */
template<typename T>
class InquiryConnector;

/**
 * InquiryService manages and handles inquiries keyed by their unique identifiers.
 * T is the product type.
 */
template<typename T>
class InquiryService : public Service<std::string, Inquiry<T>>
{
private:
    InquiryConnector<T>* connector;
    std::unordered_map<std::string, Inquiry<T>> inquiryData;
    std::vector<ServiceListener<Inquiry<T>>*> listeners;

public:
    InquiryService();
    ~InquiryService() = default;

    Inquiry<T>& GetData(std::string key) override;
    void OnMessage(Inquiry<T>& data) override;
    void AddListener(ServiceListener<Inquiry<T>>* listener) override;
    const std::vector<ServiceListener<Inquiry<T>>*>& GetListeners() const override;
    InquiryConnector<T>* GetConnector();

    void SendQuote(const std::string& inquiryId, double price);
    void RejectInquiry(const std::string& inquiryId);
};

template<typename T>
InquiryService<T>::InquiryService() : connector(new InquiryConnector<T>(this)) {}

template<typename T>
Inquiry<T>& InquiryService<T>::GetData(std::string key)
{
    auto it = inquiryData.find(key);
    if (it != inquiryData.end())
    {
        return it->second;
    }
    throw std::runtime_error("Key not found");
}

template<typename T>
void InquiryService<T>::OnMessage(Inquiry<T>& data)
{
    InquiryState state = data.GetState();
    std::string inquiryId = data.GetInquiryId();
    switch (state) 
    {
    case RECEIVED:
        // If just received, respond with a quote by publishing through the connector
        connector->Publish(data);
        break;
    case QUOTED:
        // Once quoted, finalize the inquiry as DONE and broadcast the updated object
        data.SetState(DONE);
        if (inquiryData.find(inquiryId) != inquiryData.end()) 
        {
            inquiryData.erase(inquiryId);
        }
        inquiryData.insert(std::make_pair(inquiryId, data));
        for (auto& listener : listeners)
        {
            listener->ProcessAdd(data);
        }
        break;
    default:
        break;
    }

    // If the inquiry is marked DONE, remove it from the service store
    if (data.GetState() == DONE)
    {
        inquiryData.erase(data.GetInquiryId());
    }
    else
    {
        // Otherwise, update the inquiry record
        inquiryData[data.GetInquiryId()] = data;
    }

    // Notify all listeners about the new data or changes
    for (auto& listener : listeners)
    {
        listener->ProcessAdd(data);
    }
}

template<typename T>
void InquiryService<T>::AddListener(ServiceListener<Inquiry<T>>* listener)
{
    listeners.push_back(listener);
}

template<typename T>
const std::vector<ServiceListener<Inquiry<T>>*>& InquiryService<T>::GetListeners() const
{
    return listeners;
}

template<typename T>
InquiryConnector<T>* InquiryService<T>::GetConnector()
{
    return connector;
}

template<typename T>
void InquiryService<T>::SendQuote(const std::string& inquiryId, double price)
{
    Inquiry<T>& inquiry = GetData(inquiryId);
    inquiry.SetPrice(price);
    inquiry.SetState(QUOTED);
    OnMessage(inquiry);
}

template<typename T>
void InquiryService<T>::RejectInquiry(const std::string& inquiryId)
{
    Inquiry<T>& inquiry = GetData(inquiryId);
    inquiry.SetState(REJECTED);
    OnMessage(inquiry);
}

/**
 * InquiryConnector handles both inbound (subscription) and outbound (publishing) data flow
 * for the InquiryService.
 */
template<typename T>
class InquiryConnector : public Connector<Inquiry<T>>
{
private:
    InquiryService<T>* service;

    InquiryState StringToState(const std::string& stateStr);

public:
    explicit InquiryConnector(InquiryService<T>* _service);
    ~InquiryConnector() = default;

    void Publish(Inquiry<T>& data) override;
    void Subscribe(std::ifstream& _datafile);
    void SubscribeUpdate(Inquiry<T>& data);
};

template<typename T>
InquiryConnector<T>::InquiryConnector(InquiryService<T>* _service) : service(_service) {}

template<typename T>
InquiryState InquiryConnector<T>::StringToState(const std::string& stateStr)
{
    if (stateStr == "RECEIVED") return RECEIVED;
    if (stateStr == "QUOTED") return QUOTED;
    if (stateStr == "DONE") return DONE;
    if (stateStr == "REJECTED") return REJECTED;
    return CUSTOMER_REJECTED;
}

template<typename T>
void InquiryConnector<T>::Publish(Inquiry<T>& data)
{
    if (data.GetState() == RECEIVED)
    {
        data.SetState(QUOTED);
        service->OnMessage(data);
    }
}

template<typename T>
void InquiryConnector<T>::Subscribe(std::ifstream& _datafile)
{
    std::string line;
    while (std::getline(_datafile, line))
    {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, ','))
        {
            tokens.push_back(token);
        }

        T product = ProductFactory<T>::QueryProduct(tokens[1]);
        Side side = (tokens[2] == "BUY") ? BUY : SELL;
        long quantity = std::stol(tokens[3]);
        double price = PriceUtils::Frac2Price(tokens[4]);
        InquiryState state = StringToState(tokens[5]);

        Inquiry<T> inquiry(tokens[0], product, side, quantity, price, state);
        service->OnMessage(inquiry);
    }
}

template<typename T>
void InquiryConnector<T>::SubscribeUpdate(Inquiry<T>& data)
{
    // Forward updated inquiry object back into the service
    service->OnMessage(data);
}

#endif

