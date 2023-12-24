#ifndef executionOrder_HPP
#define executionOrder_HPP

#include <vector>
#include <string>

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };
enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{

public:

    ExecutionOrder() = default;
    // ctor for an order
    ExecutionOrder(const T& _product, PricingSide _side, std::string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, std::string _parentOrderId, bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

    // Get pricing side
    PricingSide GetPricingSide() const;

    // Get the order ID
    const std::string& GetOrderId() const;

    // Get the order type on this order
    OrderType GetOrderType() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity
    long GetHiddenQuantity() const;

    // Get the parent order ID
    const std::string& GetParentOrderId() const;

    // Is child order?
    bool IsChildOrder() const;

    std::vector<std::string> ToString() const;

private:
    T product_;
    PricingSide side_;
    std::string orderId_;
    OrderType orderType_;
    double price_;
    double visibleQuantity_;
    double hiddenQuantity_;
    std::string parentOrderId_;
    bool isChildOrder_;

};


template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T& product, PricingSide side, std::string orderId, OrderType orderType, double price, double visibleQuantity, double hiddenQuantity, std::string parentOrderId, bool isChildOrder) :
    product_(product), side_(side), orderId_(orderId),
    orderType_(orderType), price_(price), visibleQuantity_(visibleQuantity),
    hiddenQuantity_(hiddenQuantity), parentOrderId_(parentOrderId),
    isChildOrder_(isChildOrder) {}


template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
    return this->product_;
}

template<typename T>
PricingSide ExecutionOrder<T>::GetPricingSide() const {
    return this->side_;
}

template<typename T>
const std::string& ExecutionOrder<T>::GetOrderId() const
{
    return this->orderId_;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
    return this->orderType_;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
    return this->price_;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
    return this->visibleQuantity_;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
    return this->hiddenQuantity_;
}

template<typename T>
const std::string& ExecutionOrder<T>::GetParentOrderId() const
{
    return this->parentOrderId_;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
    return this->isChildOrder_;
}

template<typename T>
std::vector<std::string> ExecutionOrder<T>::ToString() const
{
    std::string _product = product_.GetProductId();
    std::string _side;
    switch (this->side_)
    {
    case BID:
        _side = "BID";
        break;
    case OFFER:
        _side = "OFFER";
        break;
    }
    std::string _orderId = this->orderId_;
    std::string _orderType;
    switch (this->orderType_)
    {
    case FOK:
        _orderType = "FOK";
        break;
    case IOC:
        _orderType = "IOC";
        break;
    case MARKET:
        _orderType = "MARKET";
        break;
    case LIMIT:
        _orderType = "LIMIT";
        break;
    case STOP:
        _orderType = "STOP";
        break;
    }

    std::vector<std::string> _strings;
    _strings.push_back(_product);
    _strings.push_back(_side);
    _strings.push_back(_orderId);
    _strings.push_back(_orderType);

    _strings.push_back(ConvertPrice(this->price_));
    _strings.push_back(std::to_string(this->visibleQuantity_));
    _strings.push_back(std::to_string(this->hiddenQuantity_));
    _strings.push_back(this->parentOrderId_);
    _strings.push_back(this->isChildOrder_ ? "YES" : "NO");

    return _strings;
}

#endif
