#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "soa.hpp"
#include <unordered_map>
#include "utilities.hpp"

enum ServiceType { POSITION, RISK, EXECUTION, STREAMING, INQUIRY };

template<typename T>
class HistoricalDataConnector;
template<typename T>
class HistoricalDataListener;

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService : Service<string, T>
{
private:
    unordered_map<string, T> historical_datas_;
    HistoricalDataConnector<T>* out_connector_;
    ServiceListener<T>* in_listener_;
    ServiceType type_;

public:
    HistoricalDataService();
    HistoricalDataService(ServiceType _type);
    ~HistoricalDataService();

    // Get data on our service given a key (orderbook)
    virtual T& GetData(string product_id) override;

    // The callback that a Connector should invoke for any new or updated data
    virtual void OnMessage(T& data) override;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    virtual void AddListener(ServiceListener<T>* listener) override;

    // Get all listeners on the Service.
    virtual const vector<ServiceListener<T>*>& GetListeners() const override;

    // Get the connector of the service
    HistoricalDataConnector<T>* GetConnector();

    // Get the listener of the service
    ServiceListener<T>* GetInListener();

    // Persist data to a store
    void PersistData(string persistKey, T data);

    ServiceType GetServiceType() const;
};

template<typename T>
class HistoricalDataConnector : public Connector<T> {
private:
    HistoricalDataService<T>* service_;

public:
    HistoricalDataConnector(HistoricalDataService<T>* service_);
    ~HistoricalDataConnector() = default;

    // Publish data to the Connector
    virtual void Publish(T& data) override;

    // Subscribe data from the Connector
    virtual void Subscribe(ifstream& data) override;

};

template<typename T>
class HistoricalDataListener : public ServiceListener<T> {
private:
    HistoricalDataService<T>* service_;

public:
    HistoricalDataListener(HistoricalDataService<T>* service);
    ~HistoricalDataListener() = default;

    // MARK: SERVICELISTENER CLASS OVERRIDE BELOW
    // Listener callback to process an add event to the Service
    virtual void ProcessAdd(T& data) override;

    // Listener callback to process a remove event to the Service
    virtual void ProcessRemove(T& data) override;

    // Listener callback to process an update event to the Service
    virtual void ProcessUpdate(T& data) override;
    // MARK: SERVICELISTENER CLASS OVERRIDE ABOVE

};


template<typename T>
HistoricalDataService<T>::HistoricalDataService() : type_(INQUIRY) {
    this->out_connector_ = new HistoricalDataConnector<T>(this);
    this->in_listener_ = new HistoricalDataListener<T>(this);
}

template<typename T>
HistoricalDataService<T>::HistoricalDataService(ServiceType type) : type_(type) {
    this->out_connector_ = new HistoricalDataConnector<T>(this);
    this->in_listener_ = new HistoricalDataListener<T>(this);
}

template<typename T>
HistoricalDataService<T>::~HistoricalDataService() {
    delete this->out_connector_;
    delete this->in_listener_;
}

template <typename T>
T& HistoricalDataService<T>::GetData(string product_id) {
    return this->historical_datas_[product_id];
}

template <typename T>
void HistoricalDataService<T>::OnMessage(T& data) {
    string product_id = data.GetProduct().GetProductId();
    this->historical_datas_[product_id] = data;
}

template <typename T>
void HistoricalDataService<T>::AddListener(ServiceListener<T>* listener) {
    this->Service<string, T>::AddListener(listener);
}

template <typename T>
const vector<ServiceListener<T>*>& HistoricalDataService<T>::GetListeners() const {
    return this->Service<string, T>::GetListeners();
}

template <typename T>
ServiceListener<T>* HistoricalDataService<T>::GetInListener() {
    return this->in_listener_;
}

template <typename T>
HistoricalDataConnector<T>* HistoricalDataService<T>::GetConnector() {
    return this->out_connector_;
}

template<typename T>
ServiceType HistoricalDataService<T>::GetServiceType() const
{
    return this->type_;
}

template<typename T>
void HistoricalDataService<T>::PersistData(string persistKey, T data) {
    this->out_connector_->Publish(data);
}

template<typename T>
HistoricalDataConnector<T>::HistoricalDataConnector(HistoricalDataService<T>* service) : service_(service) {}

template<typename T>
void HistoricalDataConnector<T>::Publish(T& data)
{
    ServiceType type = this->service_->GetServiceType();
    ofstream file;
    switch (type)
    {
    case POSITION:
        file.open("positions.txt", ios::app);
        break;
    case RISK:
        file.open("risk.txt", ios::app);
        break;
    case EXECUTION:
        file.open("executions.txt", ios::app);
        break;
    case STREAMING:
        file.open("streaming.txt", ios::app);
        break;
    case INQUIRY:
        file.open("allinquiries.txt", ios::app);
        break;
    }

    file << ",";
    vector<string> strings = data.ToString();
    for (auto& s : strings)
    {
        file << s << ",";
    }
    file << endl;
}

template<typename T>
void HistoricalDataConnector<T>::Subscribe(ifstream& data) {}

template<typename T>
HistoricalDataListener<T>::HistoricalDataListener(HistoricalDataService<T>* service)
{
    this->service_ = service;
}

template<typename T>
void HistoricalDataListener<T>::ProcessAdd(T& data)
{
    string product_id = data.GetProduct().GetProductId();
    this->service_->PersistData(product_id, data);
}

template<typename T>
void HistoricalDataListener<T>::ProcessRemove(T& data) {}

template<typename T>
void HistoricalDataListener<T>::ProcessUpdate(T& data) {}

#endif