// IPriceStream.hpp
//
// Defines an interface for price streams, encapsulating bid and offer order details for a product.
//
// @class IPriceStream
// @description This interface provides methods to retrieve product-specific price stream details, 
//              including bid and offer orders.
//
// @methods 
// - GetProduct: Retrieves the product associated with the price stream.
// - GetBidOrder: Returns the bid order details of the price stream.
// - GetOfferOrder: Returns the offer order details of the price stream.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IPRICESTREAM_HPP
#define IPRICESTREAM_HPP

#include "IPriceStreamOrder.hpp"

template<typename T>
class IPriceStream {
public:
    virtual ~IPriceStream() = default;
    virtual const T& GetProduct() const = 0;
    virtual const IPriceStreamOrder& GetBidOrder() const = 0;
    virtual const IPriceStreamOrder& GetOfferOrder() const = 0;
};

#endif
