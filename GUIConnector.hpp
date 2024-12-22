// GUIConnector.hpp
//
// Connects the GUI service to external components by publishing pricing data to a file.
// Facilitates the integration of price updates with a GUI through a text file.
//
// @class GUIConnector
// @description This class implements the `Connector` interface for the `Price` type. It publishes price data
//              to a predefined file, appending each update with a timestamp for GUI consumption.
//
// @methods 
// - Publish: Writes the current time and price data to a file in an append mode.
//
// @attributes
// - service: A pointer to the associated `GUIService` that this connector works with.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef GUICONNECTOR_HPP
#define GUICONNECTOR_HPP

#include "soa.hpp"
#include "TimeUtils.hpp"
#include <fstream>

template<typename T>
class GUIService; 

template<typename T>
class GUIConnector : public Connector<Price<T>>
{
public:
    GUIConnector(GUIService<T>* _service) : service(_service) {}
    void Publish(Price<T>& data) override {
        std::ofstream outFile;
        outFile.open("../res/gui.txt", std::ios::app);
        // outFile << getTime() << "," << data << std::endl;
        outFile << TimeUtils::GetCurrentTime() << "," << data << std::endl;
        outFile.close();
    }

private:
    GUIService<T>* service;
};

#endif
