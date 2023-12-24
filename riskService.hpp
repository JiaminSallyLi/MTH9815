
#ifndef riskService_HPP
#define riskService_HPP

#include "soa.hpp"
#include "positionService.hpp"
#include <vector>
#include <unordered_map>
#include "utilities.hpp"
#include <string>

/**
 * PV01 risk.
 * Type T is the product type.
 */
template <typename T>
class PV01
{

public:
    
    PV01() = default;
    // ctor for a PV01 value
    PV01(const T &_product, double _pv01, long _quantity);

    // Get the product on this PV01 value
    const T& GetProduct() const;

    // Get the PV01 value
    double GetPV01() const;

    // Get the quantity that this risk value is associated with
    long GetQuantity() const;
    
    void SetQuantity(long _quantity);
    
    std::vector<std::string> ToString() const;

private:
    T product;
    double pv01;
    long quantity;

};

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template <typename T>
class BucketedSector
{

public:

    BucketedSector() = default;
    // ctor for a bucket sector
    BucketedSector(const std::vector<T> &_products, std::string _name);

    // Get the products associated with this bucket
    const std::vector<T>& GetProducts() const;

    // Get the name of the bucket
    const std::string& GetName() const;

private:
    vector<T> products;
    string name;

};

template <typename T>
class PositionToRiskListener;

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template <typename T>
class RiskService : public Service<string,PV01 <T> >
{
private:
    unordered_map<std::string, PV01<T>> pv01s_;
    PositionToRiskListener<T>* in_listener_;
    
public:
    RiskService();
    ~RiskService();
    
    // Get data on our service given a key (orderbook)
    virtual PV01<T>& GetData(std::string product_id) override;
    
    // The callback that a Connector should invoke for any new or updated data
    virtual void OnMessage(PV01<T>& data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    virtual void AddListener(ServiceListener<PV01<T>>* listener) override;
    
    // Get all listeners on the Service.
    virtual const std::vector<ServiceListener<PV01<T>>*>& GetListeners() const override;
    
    PositionToRiskListener<T>* GetInListener();

    // Add a position that the service will risk
    void AddPosition(Position<T> &position);

    // Get the bucketed risk for the bucket sector
    const PV01<BucketedSector<T> >& GetBucketedRisk(const BucketedSector<T> &sector) const;

};

template <typename T>
class PositionToRiskListener : public ServiceListener<Position<T>> {
private:
    RiskService<T>* service_;

public:

    PositionToRiskListener(RiskService<T>* _service);
    ~PositionToRiskListener() = default;

    // Listener callback to process an add event to the Service
    virtual void ProcessAdd(Position<T> &data) override;

    // Listener callback to process a remove event to the Service
    virtual void ProcessRemove(Position<T> &data) override;

    // Listener callback to process an update event to the Service
    virtual void ProcessUpdate(Position<T> &data) override;

};

template <typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
  product(_product), pv01(_pv01), quantity(_quantity) {}

template<typename T>
const T& PV01<T>::GetProduct() const {
    return this->product;
}

template<typename T>
double PV01<T>::GetPV01() const {
    return this->pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const {
    return this->quantity;
}

template<typename T>
void PV01<T>::SetQuantity(long _quantity) {
    this->quantity = _quantity;
}

template <typename T>
BucketedSector<T>::BucketedSector(const std::vector<T>& _products, std::string _name) :
  products(_products), name(_name) {}

template <typename T>
const std::vector<T>& BucketedSector<T>::GetProducts() const
{
    return this->products;
}

template <typename T>
const string& BucketedSector<T>::GetName() const
{
    return this->name;
}

template <typename T>
RiskService<T>::RiskService() {
    this->in_listener_ = new PositionToRiskListener<T>(this);
}

template <typename T>
RiskService<T>::~RiskService() {
    delete this->in_listener_;
}

template <typename T>
PV01<T>& RiskService<T>::GetData(std::string product_id) {
    return this->pv01s_[product_id];
}

template <typename T>
void RiskService<T>::OnMessage(PV01<T>& data) {
    std::string product_id = data.GetProduct().GetProductId();
    this->pv01s_[product_id] = data;
}

template <typename T>
void RiskService<T>::AddListener(ServiceListener<PV01<T>>* listener) {
    this->Service<std::string, PV01<T>>::AddListener(listener);
}

template <typename T>
const std::vector<ServiceListener<PV01<T>>*>& RiskService<T>::GetListeners() const {
    return this->Service<std::string, PV01<T>>::GetListeners();
}

template <typename T>
PositionToRiskListener<T>* RiskService<T>::GetInListener() {
    return this->in_listener_;
}

// Add a position that the service will risk
template <typename T>
void RiskService<T>::AddPosition(Position<T>& position) {
    
    // Parse info from position
    T product = position.GetProduct();
    std::string product_id = product.GetProductId();
    long quantity = position.GetAggregatePosition();
    
    // Convert to PV01 obj
    double pv01_value = GetPV01Value(product_id);
    PV01<T> pv01(product, pv01_value, quantity);
    this->pv01s_.insert_or_assign(product_id, pv01);

    // Notify listeners
    for (auto& l : Service<std::string, PV01<T>>::listeners_)
    {
        l->ProcessAdd(pv01);
    }
}

// Get the bucketed risk for the bucket sector
template <typename T>
const PV01<BucketedSector<T>>& RiskService<T>::GetBucketedRisk(const BucketedSector<T>& sector) const {
    
    BucketedSector<T> product = sector;
    
    double pv01 = 0.;
    long quantity = 1;  // Dummy

    std::vector<T>& products = sector.GetProducts();
    for (auto& product : products)
    {
        std::string product_id = product.GetProductId();
        pv01 += pv01s_[product_id].GetPV01() * pv01s_[product_id].GetQuantity();
    }

    return PV01<BucketedSector<T>>(product, pv01, quantity);
}

template<typename T>
PositionToRiskListener<T>::PositionToRiskListener(RiskService<T>* service) : service_(service) {}

template<typename T>
void PositionToRiskListener<T>::ProcessAdd(Position<T>& data)
{
    this->service_->AddPosition(data);
}

template<typename T>
void PositionToRiskListener<T>::ProcessRemove(Position<T>& data) {}

template<typename T>
void PositionToRiskListener<T>::ProcessUpdate(Position<T>& data) {}

template<typename T>
std::vector<string> PV01<T>::ToString() const
{
    std::string _product = product.GetProductId();
    std::string _pv01 = to_string(pv01);
    std::string _quantity = to_string(quantity);

    std::vector<string> _strings;
    _strings.push_back(_product);
    _strings.push_back(_pv01);
    _strings.push_back(_quantity);
    return _strings;
}

#endif
