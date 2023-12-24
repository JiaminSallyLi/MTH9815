
#ifndef Utilities_hpp
#define Utilities_hpp

#include <iostream>
#include <string>
#include <utility>
#include <map>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <chrono>
#include <ctime>
#include "products.hpp"

// Convert numeric price to bond notation
float ConvertPrice(const string& str_price) {
    // "100-xyz" -> 100 + xy / 32 + z / 256
    auto delimiter_pos = str_price.find('-');
    
    // Integer
    float res = stoi(str_price.substr(0, delimiter_pos));
    
    // xy
    res += stoi(str_price.substr(delimiter_pos + 1, 2)) / 32.;
    
    // z
    char last_char = str_price[delimiter_pos + 3];
    if (last_char == '+') {
        res += 1. / 64.;
    } else {
        res += (last_char - '0') / 256.;  // int('0') == 48
    }
    return res;
}

string ConvertPrice(float f_price) {
    int integer = int(f_price);
    string res = to_string(integer) + '-';
    
    f_price -= integer;
    f_price *= 32.;
    integer = int(f_price);
    if (integer < 10) {
        // Pad with 0 if only one digit
        res += '0';
    }
    
    res += to_string(integer);
    
    f_price -= integer;
    f_price *= 8.;
    integer = int(f_price);
    if (integer == 4.) {
        res += '+';
    } else {
        res += to_string(integer);
    }
    
    return res;
}


std::map<int, std::pair<string, boost::gregorian::date>> kBondMapMaturity({
    {2, {"BONDNO1", {2025, boost::gregorian::Nov, 30}}},
    {3, {"BONDNO2", {2026, boost::gregorian::Nov, 15}}},
    {5, {"BONDNO3", {2028, boost::gregorian::Nov, 30}}},
    {7, {"BONDNO4", {2030, boost::gregorian::Nov, 30}}},
    {10, {"BONDNO5", {2033, boost::gregorian::Nov, 15}}},
    {20, {"BONDNO6", {2043, boost::gregorian::Nov, 30}}},
    {30, {"BONDNO7", {2053, boost::gregorian::Nov, 15}}}
});

std::map<string, std::pair<int, boost::gregorian::date>> kBondMapCusip({
    {"BONDNO1", {2, {2025, boost::gregorian::Nov, 30}}},
    {"BONDNO2", {3, {2026, boost::gregorian::Nov, 15}}},
    {"BONDNO3", {5, {2028, boost::gregorian::Nov, 30}}},
    {"BONDNO4", {7, {2030, boost::gregorian::Nov, 30}}},
    {"BONDNO5", {10, {2033, boost::gregorian::Nov, 15}}},
    {"BONDNO6", {20, {2043, boost::gregorian::Nov, 30}}},
    {"BONDNO7", {30, {2053, boost::gregorian::Nov, 15}}}
});

std::map<std::string, double> kPV01Map({
    {"BONDNO1", 0.019851},
    {"BONDNO2", 0.029309},
    {"BONDNO3", 0.048643},
    {"BONDNO4", 0.065843},
    {"BONDNO5", 0.087939},
    {"BONDNO6", 0.012346},
    {"BONDNO7", 0.018469}
});


// Fetch cusip object from maturity (years)
string FetchCusip(int maturity) {
    return kBondMapMaturity[maturity].first;
}

Bond FetchBond(int maturity) {
    return Bond(kBondMapMaturity[maturity].first, CUSIP, "US" + to_string(maturity) + "Y", 0., kBondMapMaturity[maturity].second);
}

Bond FetchBond(const string& cusip) {
    return Bond(cusip, CUSIP, "US" + to_string(kBondMapCusip[cusip].first) + "Y", 0., kBondMapCusip[cusip].second);
}

double GetPV01Value(const string& cusip) {
    return kPV01Map[cusip];
}


long GetMillisecond() {
    auto time_point = chrono::system_clock::now();
    auto sec = chrono::time_point_cast<chrono::seconds>(time_point);
    auto millisec = chrono::duration_cast<chrono::milliseconds>(time_point - sec);
    long millisec_count = millisec.count();
    return millisec_count;
}


#endif
