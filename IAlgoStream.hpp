// IAlgoStream.hpp
//
// Defines an interface for accessing algorithmic price streams.
//
// @class IAlgoStream
// @description This interface provides a method to retrieve the associated price stream
//              for algorithmic trading operations.
//
// @methods 
// - GetPriceStream: Retrieves the price stream associated with the algorithmic stream.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef IALGOSTREAM_HPP
#define IALGOSTREAM_HPP

#include "IPriceStream.hpp"

template<typename T>
class IAlgoStream {
public:
    virtual ~IAlgoStream() = default;
    virtual const IPriceStream<T>& GetPriceStream() const = 0;
};

#endif
