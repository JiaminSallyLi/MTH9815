
#ifndef MarketDataService_HPP
#define MarketDataService_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <string>
#include "soa.hpp"
#include "utilities.hpp"

using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
 * A market data order with price, quantity, and side.
 */
class Order
{

public:

    // ctor for an order
    Order(double _price, long _quantity, PricingSide _side);

    // Get the price on the order
    double GetPrice() const;

    // Get the quantity on the order
    long GetQuantity() const;

    // Get the side on the order
    PricingSide GetSide() const;

private:
    double price;
    long quantity;
    PricingSide side;

};

/**
 * Class representing a bid and offer order
 */
class BidOffer
{

public:

    // ctor for bid/offer
    BidOffer(const Order &_bidOrder, const Order &_offerOrder);

    // Get the bid order
    const Order& GetBidOrder() const;

    // Get the offer order
    const Order& GetOfferOrder() const;

private:
    const Order& bidOrder;
    const Order& offerOrder;
};

/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template <typename T>
class OrderBook
{

public:

    // ctor for the order book
    OrderBook() = default;  // Necessary for map operations
    OrderBook(const T &_product, const std::vector<Order> &_bidStack, const std::vector<Order> &_offerStack);
    OrderBook(const T &_product, std::vector<Order>&& _bidStack, std::vector<Order>&& _offerStack);

    // Get the product
    const T& GetProduct() const;

    // Get the bid stack
    const std::vector<Order>& GetBidStack() const;

    // Get the offer stack
    const std::vector<Order>& GetOfferStack() const;
    
    // Get the best bid/offer order
    const BidOffer GetBidOffer() const;

private:
    T product;
    std::vector<Order> bidStack;
    std::vector<Order> offerStack;

};

template <typename T>
class MarketDataConnector;

/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template <typename T>
class MarketDataService : public Service<string,OrderBook <T> >
{
private:
    
    unordered_map<string, OrderBook<T>> order_books_;
    MarketDataConnector<T>* in_connector_;
    int book_depth_;
    
public:

    MarketDataService();
    ~MarketDataService();
    
    // Get data on our service given a key (orderbook)
    virtual OrderBook<T>& GetData(string product_id) override;
    
    // The callback that a Connector should invoke for any new or updated data
    virtual void OnMessage(OrderBook<T>& book) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    virtual void AddListener(ServiceListener<OrderBook<T>>* listener) override;
    
    // Get all listeners on the Service.
    virtual const std::vector<ServiceListener<OrderBook<T>>*>& GetListeners() const override;
    
    // Get the MarketDataConnector
    MarketDataConnector<T>* GetConnector();
    
    // Get the book depth
    int GetBookDepth() const;
    
    // Get the best bid/offer order
    virtual const BidOffer GetBestBidOffer(const std::string &productId) const;

    // Aggregate the order book
    virtual const OrderBook<T>& AggregateDepth(const std::string &productId);
    
private:
    // AggregateDepth helper function
    std::vector<Order> AggregateStack(const std::vector<Order>& original_stack) const;

};

template <typename T>
class MarketDataConnector : public Connector<OrderBook<T>> {
private:
    MarketDataService<T>* service_;
    
public:
    MarketDataConnector(MarketDataService<T>* service);
    ~MarketDataConnector() = default;
    
    // Publish data to the Connector
    // Does nothing
    // MarketDataConnector is subscribe only
    virtual void Publish(OrderBook<T> &data) override;
    
    // Subscribe data from the Connector
    virtual void Subscribe(ifstream& data) override;
};

Order::Order(double _price, long _quantity, PricingSide _side)
{
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const
{
    return this->price;
}
 
long Order::GetQuantity() const
{
    return this->quantity;
}
 
PricingSide Order::GetSide() const
{
    return this->side;
}

BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
  bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
    return this->bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
    return this->offerOrder;
}

template <typename T>
OrderBook<T>::OrderBook(const T &_product, const std::vector<Order> &_bidStack, const std::vector<Order> &_offerStack) :
  product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template <typename T>
OrderBook<T>::OrderBook(const T &_product, std::vector<Order>&& _bidStack, std::vector<Order>&& _offerStack) : product(_product), bidStack(_bidStack), offerStack(_offerStack) {}

template <typename T>
const T& OrderBook<T>::GetProduct() const
{
    return this->product;
}

template <typename T>
const std::vector<Order>& OrderBook<T>::GetBidStack() const
{
    return this->bidStack;
}

template <typename T>
const std::vector<Order>& OrderBook<T>::GetOfferStack() const
{
    return this->offerStack;
}

template <typename T>
const BidOffer OrderBook<T>::GetBidOffer() const {
    // Get highest bid order
    const Order* highest_bid_order(&this->bidStack[0]);
    double highest_bid_price = highest_bid_order->GetPrice();
    for (std::size_t i = 1; i < bidStack.size(); i++) {
        if (bidStack[i].GetPrice() > highest_bid_price) {
            highest_bid_order = &bidStack[i];
            highest_bid_price = highest_bid_order->GetPrice();
        }
    }

    // Get lowest offer order
    const Order* lowest_offer_order(&this->offerStack[0]);
    double lowest_offer_price = lowest_offer_order->GetPrice();
    for (std::size_t i = 1; i < offerStack.size(); i++) {
        if (offerStack[i].GetPrice() < lowest_offer_price) {
            lowest_offer_order = &offerStack[i];
            lowest_offer_price = lowest_offer_order->GetPrice();
        }
    }

    return BidOffer(*highest_bid_order, *lowest_offer_order);
}

template <typename T>
MarketDataService<T>::MarketDataService() : order_books_(), in_connector_(new MarketDataConnector<T>(this)), book_depth_(10) {}

template <typename T>
MarketDataService<T>::~MarketDataService() {
    delete this->in_connector_;
}

template <typename T>
OrderBook<T>& MarketDataService<T>::GetData(string product_id) {
    return order_books_.at(product_id);
}

template <typename T>
void MarketDataService<T>::OnMessage(OrderBook<T>& book) {
    string product_id = book.GetProduct().GetProductId();
    this->order_books_.insert_or_assign(product_id, book);
    
    // Also notify listeners
    for (auto& l : Service<string, OrderBook<T>>::listeners_) {
        l->ProcessAdd(book);
    }
}

template <typename T>
void MarketDataService<T>::AddListener(ServiceListener<OrderBook<T>>* listener) {
    this->Service<string, OrderBook<T>>::AddListener(listener);
}

template <typename T>
const std::vector<ServiceListener<OrderBook<T>>*>& MarketDataService<T>::GetListeners() const {
    return this->Service<string, OrderBook<T>>::GetListeners();
}

// Get the MarketDataConnector
template <typename T>
MarketDataConnector<T>* MarketDataService<T>::GetConnector() {
    return this->in_connector_;
}

// Get the book depth
template <typename T>
int MarketDataService<T>::GetBookDepth() const {
    return this->book_depth_;
}

// Get the best bid/offer order
template <typename T>
const BidOffer MarketDataService<T>::GetBestBidOffer(const std::string &productId) const {
    return this->order_books_.find(productId)->second.GetBidOffer();
}

// AggregateDepth helper function
template <typename T>
std::vector<Order> MarketDataService<T>::AggregateStack(const std::vector<Order>& original_stack) const {
    
    unordered_map<double, long> order_map;
    
    for (const auto& order : original_stack) {
        if (order_map.find(order.GetPrice()) == order_map.end()) {
            // The price does not exist yet
            order_map[order.GetPrice()] = order.GetQuantity();
        } else {
            // The price already exists
            order_map[order.GetPrice()] += order.GetQuantity();
        }
    }
    
    std::vector<Order> aggregated_stack;
    aggregated_stack.reserve(order_map.size());
    for (auto [price, quantity] : order_map) {
        Order order(price, quantity, BID);
        aggregated_stack.push_back(order);
    }
    
    return aggregated_stack;
}

// Aggregate the order book
// Also modify that book
template <typename T>
const OrderBook<T>& MarketDataService<T>::AggregateDepth(const std::string &productId) {
    const T& product = order_books_.at(productId).GetProduct();
    
    // Aggregate bid orders
    const std::vector<Order>& original_bid_stack = order_books_.at(productId).GetBidStack();
    
    std::vector<Order> aggregated_bid_stack = this->AggregateStack(original_bid_stack);
    
    // Aggregate offer orders
    const std::vector<Order>& original_offer_stack = order_books_.at(productId).GetOfferStack();
    
    std::vector<Order> aggregated_offer_stack = this->AggregateStack(original_offer_stack);
    
    OrderBook<T> aggregated_order_book(product, std::move(aggregated_bid_stack), std::move(aggregated_offer_stack));
//    OrderBook<T> aggregated_order_book(product, aggregated_bid_stack, aggregated_offer_stack);
    
    this->order_books_.at(productId) = aggregated_order_book;
    
    return this->order_books_.at(productId);
}

template <typename T>
MarketDataConnector<T>::MarketDataConnector(MarketDataService<T>* service) : service_(service) {}

template <typename T>
void MarketDataConnector<T>::Publish(OrderBook<T> &data) {
    // Does nothing
    // MarketDataConnector is subscribe only
}

template <typename T>
void MarketDataConnector<T>::Subscribe(ifstream &data) {
    
    int book_depth = this->service_->GetBookDepth();
    int read_lines = book_depth << 1;
    
    vector<Order> bid_stack;
    vector<Order> offer_stack;
    
    string line;
    unsigned order_count = 0;
    while (getline(data, line)) {
        // Separate line with delimiter ','
        stringstream line_stream(line);
        string line_entry;
        vector<string> line_entries;
        while (getline(line_stream, line_entry, ',')) {
            line_entries.push_back(line_entry);
        }
        
        // Parse data into Order
        string product_id = line_entries[0];
        double price = ConvertPrice(line_entries[1]);
        long quantity = stol(line_entries[2]);
        PricingSide side = (line_entries[3] == "BID") ? BID : OFFER;
        Order order(price, quantity, side);
        
        // Push data to stack
        if (side == BID) {
            bid_stack.push_back(order);
        } else {
            offer_stack.push_back(order);
        }
        
        // Publish the entire book if the OrderBook is deep enough
        order_count++;
        if (order_count == (read_lines)) {
            T product = FetchBond(product_id);
            OrderBook<T> orderbook(product, bid_stack, offer_stack);
            this->service_->OnMessage(orderbook);
            
            bid_stack.clear();
            offer_stack.clear();
            // Note: This operation does not shrink the capacity of the vectors.
            //   It is intended behavior since they will be filled to the same size soon.
            
            order_count = 0;
        }
    }
}

#endif /* MARKET_DATA_SERVICE_HPP */
