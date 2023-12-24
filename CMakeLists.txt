# Set minimum version
cmake_minimum_required(VERSION 3.9)
project(tradingsystemJiaminLi)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Find Boost package
find_package(Boost REQUIRED COMPONENTS date_time)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})

# Add your source files
set(SOURCE_FILES

	algoExecutionService.hpp
	algoStreamingService.hpp
	executionOrder.hpp
	executionService.hpp
	guiService.hpp
	historicalDataService.hpp
	inquiryService.hpp
	marketDataService.hpp
	positionService.hpp
	priceStream.hpp
	pricingService.hpp
	products.hpp
	riskService.hpp
	soa.hpp
	streamingService.hpp
	tradeBookingService.hpp
	utilities.hpp
        main.cpp
)

add_executable(tradingsystem ${SOURCE_FILES})

include_directories(/Users/sallyli/Documents/MTH9815/tradingsystem)

# Link Boost libraries
target_link_libraries(tradingsystem ${Boost_LIBRARIES})
