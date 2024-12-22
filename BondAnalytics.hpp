// BondAnalytics.hpp
//
// Defines a class for bond-related analytics, including calculations for PV01 and querying bond information.
// The class provides methods to calculate the Present Value of a basis point (PV01) and query PV01 for specific bonds identified by their CUSIP.
//
// @class BondAnalytics
// @description This class encapsulates methods for bond price and yield calculations, offering modular and reusable components
//              for financial analysis. The implementation optimizes performance and readability.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef BONDANALYTICS_HPP
#define BONDANALYTICS_HPP

#include <cmath>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>

class BondAnalytics {
public:
    static double CalculatePV(double faceValue, double couponRate, double yieldRate, int yearsToMaturity, int frequency) {
        double coupon = faceValue * couponRate / frequency;
        double presentValue = 0.0;

        for (int t = 1; t <= yearsToMaturity * frequency; ++t) {
            presentValue += coupon / std::pow(1.0 + yieldRate / frequency, t);
        }
        presentValue += faceValue / std::pow(1.0 + yieldRate / frequency, yearsToMaturity * frequency);

        return presentValue;
    }

    static double CalculatePV01(double faceValue, double couponRate, double yieldRate, int yearsToMaturity, int frequency) {
        double pvInitial = CalculatePV(faceValue, couponRate, yieldRate, yearsToMaturity, frequency);
        double pvAdjusted = CalculatePV(faceValue, couponRate, yieldRate + 0.0001, yearsToMaturity, frequency);

        return pvInitial - pvAdjusted;
    }

    static double QueryPV01(const std::string& cusip) {
        static const std::map<std::string, std::function<double()>> pv01Map = {
            {"91282CAV3", [] { return CalculatePV01(1000, 0.04500, 0.0464, 2, 2); }},
            {"91282CBL4", [] { return CalculatePV01(1000, 0.04750, 0.0440, 3, 2); }},
            {"91282CCB5", [] { return CalculatePV01(1000, 0.04875, 0.0412, 5, 2); }},
            {"91282CCS8", [] { return CalculatePV01(1000, 0.05000, 0.0430, 7, 2); }},
            {"91282CDH2", [] { return CalculatePV01(1000, 0.05125, 0.0428, 10, 2); }},
            {"912810TM0", [] { return CalculatePV01(1000, 0.05250, 0.0461, 20, 2); }},
            {"912810TL2", [] { return CalculatePV01(1000, 0.05375, 0.0443, 30, 2); }}
        };

        auto it = pv01Map.find(cusip);
        if (it == pv01Map.end()) {
            throw std::invalid_argument("Unknown CUSIP: " + cusip);
        }
        return it->second();
    }
};

#endif
