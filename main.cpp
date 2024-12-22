// main.cpp
//
// Entry point for a trading system, integrating various services for market data processing, trade booking,
// pricing, risk management, and inquiries. It also handles data flow management and service initialization.
//
// @file main.cpp
// @description This program sets up directories, generates initial data, initializes services, and processes
//              data flows for a simulated trading environment.
//
// @functions
// - PrepareDirectories: Sets up or resets directories for data and results.
// - GenerateInitialData: Creates initial data for prices, market data, trades, and inquiries.
// - InitializeServices: Links trading services and their listeners to ensure data flows correctly.
// - ProcessDataFlows: Handles the data ingestion and processing for pricing, market data, trades, and inquiries.
//
// @main
// - Sets up directories and file paths.
// - Generates initial datasets.
// - Initializes all trading services.
// - Processes data flows through the services.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#include <iostream>
#include <string>
#include <iomanip>
#include <filesystem>
#include <memory>

#include "soa.hpp"
#include "marketdataservice.hpp"
#include "pricingservice.hpp"
#include "products.hpp"
#include "riskservice.hpp"
#include "executionservice.hpp"
#include "positionservice.hpp"
#include "inquiryservice.hpp"
#include "historicaldataservice.hpp"
#include "streamingservice.hpp"
#include "tradebookingservice.hpp"

#include "AlgoExecution.hpp"
#include "AlgoExecutionService.hpp"
#include "AlgoExecutionServiceListener.hpp"
#include "AlgoStream.hpp"
#include "AlgoStreamingService.hpp"
#include "AlgoStreamingServiceListener.hpp"
#include "BondAnalytics.hpp"
#include "DataGenerator.hpp"
#include "ExecutionOrder.hpp"
#include "GUIConnector.hpp"
#include "GUIService.hpp"
#include "GUIServiceListener.hpp"
#include "Logger.hpp"
#include "PriceStream.hpp"
#include "PriceStreamOrder.hpp"
#include "PriceUtils.hpp"
#include "ProductFactory.hpp"
#include "RandomUtils.hpp"
#include "SimpleAlgoOrderFactory.hpp"
#include "TimeUtils.hpp"

using namespace std;

void PrepareDirectories(const string& dataDir, const string& resultDir)
{
    if (filesystem::exists(dataDir)) {
        filesystem::remove_all(dataDir);
    }
    filesystem::create_directory(dataDir);

    if (filesystem::exists(resultDir)) {
        filesystem::remove_all(resultDir);
    }
    filesystem::create_directory(resultDir);
}

void GenerateInitialData(
    const vector<string>& bondUniverse, 
    const string& priceFile, 
    const string& marketDataFile, 
    const string& tradeFile, 
    const string& inquiryFile,
    int priceCount = 10, 
    int marketCount = 10,
    int tradeCount = 10,
    int inquiryCount = 10
)
{
	Logger::Log(LogLevel::INFO, "Start generating price and order book data...");
    DataGenerator::GenOrderBook(bondUniverse, priceFile, marketDataFile, priceCount, marketCount);
    DataGenerator::GenTrades(bondUniverse, tradeFile, tradeCount);
    DataGenerator::GenInquiries(bondUniverse, inquiryFile, inquiryCount);
	Logger::Log(LogLevel::INFO, "Data generation completed.");
}

void InitializeServices(
    PricingService<Bond>& pricingService,
    AlgoStreamingService<Bond>& algoStreamingService,
    StreamingService<Bond>& streamingService,
    MarketDataService<Bond>& marketDataService,
    AlgoExecutionService<Bond>& algoExecutionService,
    ExecutionService<Bond>& executionService,
    TradeBookingService<Bond>& tradeBookingService,
    PositionService<Bond>& positionService,
    RiskService<Bond>& riskService,
    GUIService<Bond>& guiService,
    InquiryService<Bond>& inquiryService,
    HistoricalDataService<Position<Bond>>& historicalPositionService,
    HistoricalDataService<PV01<Bond>>& historicalRiskService,
    HistoricalDataService<ExecutionOrder<Bond>>& historicalExecutionService,
    HistoricalDataService<PriceStream<Bond>>& historicalStreamingService,
    HistoricalDataService<Inquiry<Bond>>& historicalInquiryService
)
{
	Logger::Log(LogLevel::INFO, "Initializing trading service components...");

    pricingService.AddListener(algoStreamingService.GetAlgoStreamingListener());
    pricingService.AddListener(guiService.GetGUIServiceListener());
    algoStreamingService.AddListener(streamingService.GetStreamingServiceListener());
    marketDataService.AddListener(algoExecutionService.GetAlgoExecutionServiceListener());
    algoExecutionService.AddListener(executionService.GetExecutionServiceListener());
    executionService.AddListener(tradeBookingService.GetTradeBookingServiceListener());
    tradeBookingService.AddListener(positionService.GetPositionListener());
    positionService.AddListener(riskService.GetRiskServiceListener());

    positionService.AddListener(historicalPositionService.GetHistoricalDataServiceListener());
    executionService.AddListener(historicalExecutionService.GetHistoricalDataServiceListener());
    streamingService.AddListener(historicalStreamingService.GetHistoricalDataServiceListener());
    riskService.AddListener(historicalRiskService.GetHistoricalDataServiceListener());
    inquiryService.AddListener(historicalInquiryService.GetHistoricalDataServiceListener());

	Logger::Log(LogLevel::INFO, "Trading service components initialized.");
}

void ProcessDataFlows(
    PricingService<Bond>& pricingService,
    MarketDataService<Bond>& marketDataService,
    TradeBookingService<Bond>& tradeBookingService,
    InquiryService<Bond>& inquiryService,
    const string& priceFilePath,
    const string& marketDataFilePath,
    const string& tradeFilePath,
    const string& inquiryFilePath
)
{
	Logger::Log(LogLevel::INFO, "Processing price data..."); 
    {
        ifstream priceStream(priceFilePath.c_str());
        pricingService.GetConnector()->Subscribe(priceStream);
		Logger::Log(LogLevel::INFO, "Price data processing completed.");
    }

	Logger::Log(LogLevel::INFO, "Processing market data...");
    {
        ifstream marketStream(marketDataFilePath.c_str());
        marketDataService.GetConnector()->Subscribe(marketStream);
		Logger::Log(LogLevel::INFO, "Market data processing completed.");
    }

	Logger::Log(LogLevel::INFO, "Processing trade data...");
    {
        ifstream tradeStream(tradeFilePath.c_str());
        tradeBookingService.GetConnector()->Subscribe(tradeStream);
		Logger::Log(LogLevel::INFO, "Trade data processing completed.");
    }

	Logger::Log(LogLevel::INFO, "Processing inquiry data...");
    {
        ifstream inquiryStream(inquiryFilePath.c_str());
        inquiryService.GetConnector()->Subscribe(inquiryStream);
		Logger::Log(LogLevel::INFO, "Inquiry data processing completed.");
    }
}


int main() {
    const string dataDirectory = "./data";
    const string resultDirectory = "./result";

    PrepareDirectories(dataDirectory, resultDirectory);

    const string pricePath = dataDirectory + "/prices.txt";
    const string marketDataPath = dataDirectory + "/marketdata.txt";
    const string tradePath = dataDirectory + "/trades.txt";
    const string inquiryPath = dataDirectory + "/inquiries.txt";

    vector<string> bonds = { "91282CAV3", "91282CBL4", "91282CCB5", "91282CCS8", "91282CDH2", "912810TM0", "912810TL2" };

    GenerateInitialData(bonds, pricePath, marketDataPath, tradePath, inquiryPath);

    PricingService<Bond> pricingService;
    AlgoStreamingService<Bond> algoStreamingService;
    StreamingService<Bond> streamingService;
    MarketDataService<Bond> marketDataService;
    auto algoOrderFactory = std::make_unique<SimpleAlgoOrderFactory<Bond>>();
    AlgoExecutionService<Bond> algoExecutionService(std::move(algoOrderFactory));
    ExecutionService<Bond> executionService;
    TradeBookingService<Bond> tradeBookingService;
    PositionService<Bond> positionService;
    RiskService<Bond> riskService;
    GUIService<Bond> guiService;
    InquiryService<Bond> inquiryService;

    HistoricalDataService<Position<Bond>> historicalPositionService(POSITION);
    HistoricalDataService<PV01<Bond>> historicalRiskService(RISK);
    HistoricalDataService<ExecutionOrder<Bond>> historicalExecutionService(EXECUTION);
    HistoricalDataService<PriceStream<Bond>> historicalStreamingService(STREAMING);
    HistoricalDataService<Inquiry<Bond>> historicalInquiryService(INQUIRY);

    InitializeServices(
        pricingService,
        algoStreamingService,
        streamingService,
        marketDataService,
        algoExecutionService,
        executionService,
        tradeBookingService,
        positionService,
        riskService,
        guiService,
        inquiryService,
        historicalPositionService,
        historicalRiskService,
        historicalExecutionService,
        historicalStreamingService,
        historicalInquiryService
    );

    cout << fixed << setprecision(6);

    ProcessDataFlows(pricingService, marketDataService, tradeBookingService, inquiryService,
                     pricePath, marketDataPath, tradePath, inquiryPath);

	Logger::Log(LogLevel::FINAL, "Trading system built successfully.");
    return 0;
}


