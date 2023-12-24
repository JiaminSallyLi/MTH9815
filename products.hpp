
#ifndef Products_HPP
#define Products_HPP

#include <iostream>
#include <string>
#include "boost/date_time/gregorian/gregorian.hpp"


enum ProductType { IRSWAP, BOND };

/**
 * Base class for a product.
 */
class Product
{

public:
    virtual ~Product() = default;

    // ctor for a prduct
    Product(std::string _productId, ProductType _productType);

    // Get the product identifier
    const std::string& GetProductId() const;

    // Ge the product type
    ProductType GetProductType() const;

private:
    std::string productId;
    ProductType productType;

};

enum BondIdType { CUSIP, ISIN };

/**
 * Bond product class
 */
class Bond : public Product
{

public:

    // ctor for a bond
    Bond(std::string _productId, BondIdType _bondIdType, std::string _ticker, float _coupon, boost::gregorian::date _maturityDate);
    Bond();

    // Get the ticker
    const std::string& GetTicker() const;

    // Get the coupon
    float GetCoupon() const;

    // Get the maturity date
    const boost::gregorian::date& GetMaturityDate() const;

    // Get the bond identifier type
    BondIdType GetBondIdType() const;

    // Print the bond
    friend std::ostream& operator<<(std::ostream &output, const Bond &bond);

private:
    std::string productId;
    BondIdType bondIdType;
    std::string ticker;
    float coupon;
    boost::gregorian::date maturityDate;

};

/**
 * Interest Rate Swap enums
 */
enum DayCountConvention { THIRTY_THREE_SIXTY, ACT_THREE_SIXTY };
enum PaymentFrequency { QUARTERLY, SEMI_ANNUAL, ANNUAL };
enum FloatingIndex { LIBOR, EURIBOR };
enum FloatingIndexTenor { TENOR_1M, TENOR_3M, TENOR_6M, TENOR_12M };
enum Currency { USD, EUR, GBP };
enum SwapType { STANDARD, FORWARD, IMM, MAC, BASIS };
enum SwapLegType { OUTRIGHT, CURVE, FLY };

/**
 * Interest Rate Swap product
 */
class IRSwap : public Product
{

public:

    // ctor for a swap
    IRSwap(std::string productId, DayCountConvention _fixedLegDayCountConvention, DayCountConvention _floatingLegDayCountConvention, PaymentFrequency _fixedLegPaymentFrequency, FloatingIndex _floatingIndex, FloatingIndexTenor _floatingIndexTenor, boost::gregorian::date _effectiveDate, boost::gregorian::date _terminationDate, Currency _currency, int termYears, SwapType _swapType, SwapLegType _swapLegType);
    IRSwap();

    // Get the fixed leg daycount convention
    DayCountConvention GetFixedLegDayCountConvention() const;

    // Get the floating leg daycount convention
    DayCountConvention GetFloatingLegDayCountConvention() const;

    // Get the payment frequency on the fixed leg
    PaymentFrequency GetFixedLegPaymentFrequency() const;

    // Get the flaotig leg index
    FloatingIndex GetFloatingIndex() const;

    // Get the floating leg index tenor
    FloatingIndexTenor GetFloatingIndexTenor() const;

    // Get the effective date
    const boost::gregorian::date& GetEffectiveDate() const;

    // Get the termination date
    const boost::gregorian::date& GetTerminationDate() const;

    // Get the currency
    Currency GetCurrency() const;

    // Get the term in years
    int GetTermYears() const;

    // Get the swap type
    SwapType GetSwapType() const;

    // Get the swap leg type
    SwapLegType GetSwapLegType() const;

    // Print the swap
    friend std::ostream& operator<<(std::ostream &output, const IRSwap &swap);

private:
    DayCountConvention fixedLegDayCountConvention;
    DayCountConvention floatingLegDayCountConvention;
    PaymentFrequency fixedLegPaymentFrequency;
    FloatingIndex floatingIndex;
    FloatingIndexTenor floatingIndexTenor;
    boost::gregorian::date effectiveDate;
    boost::gregorian::date terminationDate;
    Currency currency;
    int termYears;
    SwapType swapType;
    SwapLegType swapLegType;
    std::string ToString(DayCountConvention dayCountConvention) const;
    std::string ToString(PaymentFrequency paymentFrequency) const;
    std::string ToString(FloatingIndex floatingIndex) const;
    std::string ToString(FloatingIndexTenor floatingIndexTenor) const;
    std::string ToString(Currency currency) const;
    std::string ToString(SwapType swapType) const;
    std::string ToString(SwapLegType swapLegType) const;

};

Product::Product(std::string _productId, ProductType _productType)
{
    productId = _productId;
    productType = _productType;
}

const std::string& Product::GetProductId() const
{
    return productId;
}

ProductType Product::GetProductType() const
{
    return productType;
}

Bond::Bond(std::string _productId, BondIdType _bondIdType, std::string _ticker, float _coupon, boost::gregorian::date _maturityDate) : Product(_productId, BOND)
{
    bondIdType = _bondIdType;
    ticker = _ticker;
    coupon = _coupon;
    maturityDate = _maturityDate;
}

Bond::Bond() : Product(0, BOND)
{
}

const std::string& Bond::GetTicker() const
{
    return ticker;
}

float Bond::GetCoupon() const
{
    return coupon;
}

const boost::gregorian::date& Bond::GetMaturityDate() const
{
    return maturityDate;
}

BondIdType Bond::GetBondIdType() const
{
    return bondIdType;
}

std::ostream& operator<<(std::ostream& output, const Bond& bond)
{
    output << bond.ticker << " " << bond.coupon << " " << bond.GetMaturityDate();
    return output;
}

IRSwap::IRSwap(std::string _productId, DayCountConvention _fixedLegDayCountConvention, DayCountConvention _floatingLegDayCountConvention, PaymentFrequency _fixedLegPaymentFrequency, FloatingIndex _floatingIndex, FloatingIndexTenor _floatingIndexTenor, boost::gregorian::date _effectiveDate, boost::gregorian::date _terminationDate, Currency _currency, int _termYears, SwapType _swapType, SwapLegType _swapLegType) :
    Product(_productId, IRSWAP)
{
    fixedLegDayCountConvention = _fixedLegDayCountConvention;
    floatingLegDayCountConvention = _floatingLegDayCountConvention;
    fixedLegPaymentFrequency = _fixedLegPaymentFrequency;
    floatingIndex = _floatingIndex;
    floatingIndexTenor = _floatingIndexTenor;
    effectiveDate = _effectiveDate;
    terminationDate = _terminationDate;
    currency = _currency;
    termYears = _termYears;
    swapType = _swapType;
    swapLegType = _swapLegType;
    effectiveDate = _effectiveDate;
    terminationDate = _terminationDate;
}

IRSwap::IRSwap() : Product(0, IRSWAP)
{
}

DayCountConvention IRSwap::GetFixedLegDayCountConvention() const
{
    return fixedLegDayCountConvention;
}

DayCountConvention IRSwap::GetFloatingLegDayCountConvention() const
{
    return floatingLegDayCountConvention;
}

PaymentFrequency IRSwap::GetFixedLegPaymentFrequency() const
{
    return fixedLegPaymentFrequency;
}

FloatingIndex IRSwap::GetFloatingIndex() const
{
    return floatingIndex;
}

FloatingIndexTenor IRSwap::GetFloatingIndexTenor() const
{
    return floatingIndexTenor;
}

const boost::gregorian::date& IRSwap::GetEffectiveDate() const
{
    return effectiveDate;
}

const boost::gregorian::date& IRSwap::GetTerminationDate() const
{
    return terminationDate;
}

Currency IRSwap::GetCurrency() const
{
    return currency;
}

int IRSwap::GetTermYears() const
{
    return termYears;
}

SwapType IRSwap::GetSwapType() const
{
    return swapType;
}

SwapLegType IRSwap::GetSwapLegType() const
{
    return swapLegType;
}


std::ostream& operator<<(std::ostream& output, const IRSwap& swap)
{
    output << "fixedDayCount:" << swap.ToString(swap.GetFixedLegDayCountConvention()) << " floatingDayCount:" << swap.ToString(swap.GetFloatingLegDayCountConvention()) << " paymentFreq:" << swap.ToString(swap.GetFixedLegPaymentFrequency()) << " " << swap.ToString(swap.GetFloatingIndexTenor()) << swap.ToString(swap.GetFloatingIndex()) << " effective:" << swap.GetEffectiveDate() << " termination:" << swap.GetTerminationDate() << " " << swap.ToString(swap.GetCurrency()) << " " << swap.GetTermYears() << "yrs " << swap.ToString(swap.GetSwapType()) << " " << swap.ToString(swap.GetSwapLegType());
    return output;
}

std::string IRSwap::ToString(DayCountConvention dayCountConvention) const
{
    switch (dayCountConvention) {
    case THIRTY_THREE_SIXTY: return "30/360";
    case ACT_THREE_SIXTY: return "Act/360";
    default: return "";
    }
}

std::string IRSwap::ToString(PaymentFrequency paymentFrequency) const
{
    switch (paymentFrequency) {
    case QUARTERLY: return "Quarterly";
    case SEMI_ANNUAL: return "Semi-Annual";
    case ANNUAL: return "Annual";
    default: return "";
    }
}

std::string IRSwap::ToString(FloatingIndex floatingIndex) const
{
    switch (floatingIndex) {
    case LIBOR: return "LIBOR";
    case EURIBOR: return "EURIBOR";
    default: return "";
    }
}

std::string IRSwap::ToString(FloatingIndexTenor floatingIndexTenor) const
{
    switch (floatingIndexTenor) {
    case TENOR_1M: return "1m";
    case TENOR_3M: return "3m";
    case TENOR_6M: return "6m";
    case TENOR_12M: return "12m";
    default: return "";
    }
}

std::string IRSwap::ToString(Currency currency) const
{
    switch (currency) {
    case USD: return "USD";
    case EUR: return "EUR";
    case GBP: return "GBP";
    default: return "";
    }
}

std::string IRSwap::ToString(SwapType swapType) const
{
    switch (swapType) {
    case STANDARD: return "Standard";
    case FORWARD: return "Forward";
    case IMM: return "IMM";
    case MAC: return "MAC";
    case BASIS: return "Basis";
    default: return "";
    }
}

std::string IRSwap::ToString(SwapLegType swapLegType) const
{
    switch (swapLegType) {
    case OUTRIGHT: return "Outright";
    case CURVE: return "Curve";
    case FLY: return "Fly";
    default: return "";
    }
}


#endif
