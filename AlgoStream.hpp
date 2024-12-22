// AlgoStream.hpp
//
// Provides functionality to manage and reference price streams for algorithmic trading systems.
// Acts as an interface to retrieve price stream data associated with a given product.
//
// @class AlgoStream
// @description This class encapsulates a reference to a `PriceStream` object, allowing algorithmic systems
//              to access price stream data efficiently. It implements the `IAlgoStream` interface for standardization.
//
// @methods 
// - GetPriceStream: Retrieves the referenced price stream.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef ALGOSTREAM_HPP
#define ALGOSTREAM_HPP

#include "IAlgoStream.hpp"
#include "PriceStream.hpp"

template<typename T>
class AlgoStream : public IAlgoStream<T>
{
public:
    AlgoStream() = default;
    AlgoStream(const PriceStream<T>& priceStream) 
        : priceStreamRef(priceStream) {}

    virtual ~AlgoStream() = default;

    const PriceStream<T>& GetPriceStream() const override {
        return priceStreamRef;
    }

private:
    const PriceStream<T>& priceStreamRef;
};

#endif
