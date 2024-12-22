// DataGenerator.hpp
//
// Provides methods to generate synthetic financial data including order books, trades, and inquiries.
// This utility is designed for testing and simulation purposes in financial systems.
//
// @class DataGenerator
// @description This class contains static methods for creating mock data for various financial entities.
//              Data includes realistic timestamps, random pricing, and systematic oscillations to mimic market behavior.
//
// @methods 
// - GenOrderBook: Generates order book data for specified products.
// - GenTrades: Generates trade data for specified products.
// - GenInquiries: Generates inquiry data for specified products.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef DATAGENERATOR_HPP
#define DATAGENERATOR_HPP

#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <chrono>
#include "TimeUtils.hpp"
#include "RandomUtils.hpp"
#include "PriceUtils.hpp"

class DataGenerator {
private:
    static void WriteOrderBookHeader(std::ofstream& pFile, std::ofstream& oFile) {
        pFile << "Timestamp,CUSIP,Bid,Ask,Spread" << std::endl;
        oFile << "Timestamp,CUSIP,Bid1,BidSize1,Ask1,AskSize1,Bid2,BidSize2,Ask2,AskSize2,Bid3,BidSize3,Ask3,AskSize3,Bid4,BidSize4,Ask4,AskSize4,Bid5,BidSize5,Ask5,AskSize5" << std::endl;
    }

    static void OscillateValue(double& value, bool& increasing, double step, double upperBound, double lowerBound) {
        if (increasing) {
            value += step;
            if (value >= upperBound) increasing = false;
        } else {
            value -= step;
            if (value <= lowerBound) increasing = true;
        }
    }

    static void WriteOrderBookData(std::ofstream& pFile, std::ofstream& oFile, const std::string& timestamp, const std::string& product, double midPrice, double randomSpread, double fixSpread) {
        double randomBid = midPrice - randomSpread / 2.0;
        double randomAsk = midPrice + randomSpread / 2.0;

        pFile << timestamp << "," << product << "," << PriceUtils::Price2Frac(randomBid) << "," 
              << PriceUtils::Price2Frac(randomAsk) << "," << randomSpread << std::endl;

        oFile << timestamp << "," << product;
        for (int level = 1; level <= 5; ++level) {
            double fixBid = midPrice - fixSpread * level / 2.0;
            double fixAsk = midPrice + fixSpread * level / 2.0;
            int size = level * 1'000'000;
            oFile << "," << PriceUtils::Price2Frac(fixBid) << "," << size << "," 
                  << PriceUtils::Price2Frac(fixAsk) << "," << size;
        }
        oFile << std::endl;
    }

public:
    static void GenOrderBook(const std::vector<std::string>& products,
                             const std::string& priceFile,
                             const std::string& orderbookFile,
                             long long seed,
                             int numDataPoints) {
        std::ofstream pFile(priceFile);
        std::ofstream oFile(orderbookFile);
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> ms_dist(1, 20);

        WriteOrderBookHeader(pFile, oFile);

        for (const auto& product : products) {
            double midPrice = 99.00;
            bool priceIncreasing = true;
            bool spreadIncreasing = true;
            double fixSpread = 1.0 / 128.0;
            auto curTime = std::chrono::system_clock::now();

            for (int i = 0; i < numDataPoints; ++i) {
                double randomSpread = RandomUtils::GenRandomSpread(gen);
                curTime += std::chrono::milliseconds(ms_dist(gen));
                std::string timestamp = TimeUtils::FormatTime(curTime);

                WriteOrderBookData(pFile, oFile, timestamp, product, midPrice, randomSpread, fixSpread);

                OscillateValue(midPrice, priceIncreasing, 1.0 / 256.0, 101.0, 99.0);
                OscillateValue(fixSpread, spreadIncreasing, 1.0 / 128.0, 1.0 / 32.0, 1.0 / 128.0);
            }
        }

        pFile.close();
        oFile.close();
    }

    static void GenTrades(const std::vector<std::string>& products,
                          const std::string& tradeFile,
                          long long seed) {
        std::vector<std::string> books = {"TRSY1", "TRSY2", "TRSY3"};
        std::vector<long> quantities = {1000000, 2000000, 3000000, 4000000, 5000000};
        std::ofstream tFile(tradeFile);
        std::mt19937 gen(seed);

        for (const auto& product : products) {
            for (int i = 0; i < 10; ++i) {
                std::string side = (i % 2 == 0) ? "BUY" : "SELL";
                std::string tradeId = RandomUtils::GenerateRandomId(12);
                std::uniform_real_distribution<double> dist(side == "BUY" ? 99.0 : 100.0, side == "BUY" ? 100.0 : 101.0);
                double price = dist(gen);
                long quantity = quantities[i % quantities.size()];
                std::string book = books[i % books.size()];

                tFile << product << "," << tradeId << "," << PriceUtils::Price2Frac(price) << "," 
                      << book << "," << quantity << "," << side << std::endl;
            }
        }

        tFile.close();
    }

    static void GenInquiries(const std::vector<std::string>& products,
                             const std::string& inquiryFile,
                             long long seed) {
        std::ofstream iFile(inquiryFile);
        std::mt19937 gen(seed);
        std::vector<long> quantities = {1000000, 2000000, 3000000, 4000000, 5000000};

        for (const auto& product : products) {
            for (int i = 0; i < 10; ++i) {
                std::string side = (i % 2 == 0) ? "BUY" : "SELL";
                std::string inquiryId = RandomUtils::GenerateRandomId(12);
                std::uniform_real_distribution<double> dist(side == "BUY" ? 99.0 : 100.0, side == "BUY" ? 100.0 : 101.0);
                double price = dist(gen);
                long quantity = quantities[i % quantities.size()];
                std::string status = "RECEIVED";

                iFile << inquiryId << "," << product << "," << side << "," 
                      << quantity << "," << PriceUtils::Price2Frac(price) << "," << status << std::endl;
            }
        }

        iFile.close();
    }
};

#endif
