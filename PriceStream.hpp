// PriceStream.hpp
//
// Represents a price stream for a financial product, containing bid and offer orders.
//
// @class PriceStream
// @description This class implements the `IPriceStream` interface and encapsulates bid and offer orders
//              for a specific product. It supports formatted output for logging and debugging purposes.
//
// @methods 
// - GetProduct: Returns the product associated with the price stream.
// - GetBidOrder: Retrieves the bid order of the price stream.
// - GetOfferOrder: Retrieves the offer order of the price stream.
//
// @operators
// - operator<<: Provides a formatted output of the price stream, including product ID and order details.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef PRICESTREAM_HPP
#define PRICESTREAM_HPP

#include "IPriceStream.hpp"
#include "PriceStreamOrder.hpp"
#include <utility>
#include <iostream>

template<typename T>
class PriceStream : public IPriceStream<T>
{
public:
    PriceStream() = default; 
    PriceStream(const T &product, const PriceStreamOrder &bidOrder, const PriceStreamOrder &offerOrder)
        : product(product), bidOrder(bidOrder), offerOrder(offerOrder) {}

    virtual ~PriceStream() = default;

    const T& GetProduct() const override { return product; }
    const PriceStreamOrder& GetBidOrder() const override { return bidOrder; }
    const PriceStreamOrder& GetOfferOrder() const override { return offerOrder; }

    friend std::ostream& operator<<(std::ostream& output, const PriceStream<T>& priceStream)
    {
        std::string productId = priceStream.GetProduct().GetProductId();
        const IPriceStreamOrder& bidOrder = priceStream.GetBidOrder();
        const IPriceStreamOrder& offerOrder = priceStream.GetOfferOrder();
        
        const PriceStreamOrder& bid = static_cast<const PriceStreamOrder&>(bidOrder);
        const PriceStreamOrder& offer = static_cast<const PriceStreamOrder&>(offerOrder);

        output << productId << "," << bid << "," << offer;
        return output;
    }

private:
    T product;
    PriceStreamOrder bidOrder;
    PriceStreamOrder offerOrder;
};

#endif
