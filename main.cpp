#include <iostream>
#include "soa.hpp"
#include "products.hpp"
#include "utilities.hpp"
#include "marketDataService.hpp"
#include "executionOrder.hpp"
#include "algoExecutionService.hpp"
#include "executionService.hpp"
#include "tradeBookingService.hpp"
#include "positionService.hpp"
#include "riskService.hpp"
#include "pricingService.hpp"
#include "priceStream.hpp"
#include "algoStreamingService.hpp"
#include "streamingService.hpp"
#include "historicalDataService.hpp"
#include "inquiryService.hpp"
#include "guiService.hpp"


int main() {
    
    std::cout << " Services Initializing..." << std::endl;
    PricingService<Bond> pricing_service;
    TradeBookingService<Bond> trade_booking_service;
    PositionService<Bond> position_service;
    RiskService<Bond> risk_service;
    MarketDataService<Bond> market_data_service;
    AlgoExecutionService<Bond> algo_execution_service;
    AlgoStreamingService<Bond> algo_streaming_service;
    GUIService<Bond> gui_service;
    ExecutionService<Bond> execution_service;
    StreamingService<Bond> streaming_service;
    InquiryService<Bond> inquiry_service;
    HistoricalDataService<Position<Bond>> historical_position_service(POSITION);
    HistoricalDataService<PV01<Bond>> historical_risk_service(RISK);
    HistoricalDataService<ExecutionOrder<Bond>> historical_execution_service(EXECUTION);
    HistoricalDataService<PriceStream<Bond>> historical_streaming_service(STREAMING);
    HistoricalDataService<Inquiry<Bond>> historical_inquiry_service(INQUIRY);

    std::cout << " Services Linking..." << std::endl;
    pricing_service.AddListener(algo_streaming_service.GetInListener());
    pricing_service.AddListener(gui_service.GetInListener());
    algo_streaming_service.AddListener(streaming_service.GetInListener());
    streaming_service.AddListener(historical_streaming_service.GetInListener());
    market_data_service.AddListener(algo_execution_service.GetInListener());
    algo_execution_service.AddListener(execution_service.GetInListener());
    execution_service.AddListener(trade_booking_service.GetInListener());
    execution_service.AddListener(historical_execution_service.GetInListener());
    trade_booking_service.AddListener(position_service.GetInListener());
    position_service.AddListener(risk_service.GetInListener());
    position_service.AddListener(historical_position_service.GetInListener());
    risk_service.AddListener(historical_risk_service.GetInListener());
    inquiry_service.AddListener(historical_inquiry_service.GetInListener());
    std::cout << " Services Linked." << std::endl;

    // Process Price Data
    std::cout << "Price Data Processing..." << std::endl;
    std::ifstream price_data("prices.txt");
    pricing_service.GetConnector()->Subscribe(price_data);

    // Process Trade Data
    std::cout << "Trade Data Processing..." << std::endl;
    std::ifstream trade_data("trades.txt");
    trade_booking_service.GetConnector()->Subscribe(trade_data);

    // Process Market Data
    std::cout << "Market Data Processing..." << std::endl;
    std::ifstream market_data("marketdata.txt");
    market_data_service.GetConnector()->Subscribe(market_data);

    // Process Inquiry Data
    std::cout << "Inquiry Data Processing..." << std::endl;
    std::ifstream inquiry_data("inquiries.txt");
    inquiry_service.GetConnector()->Subscribe(inquiry_data);

    // Complete Trades
    std::cout << "Completed" << std::endl;

}
