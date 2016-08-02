#include "property.h"
namespace PropertyClient {
using namespace std;

Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, const ValueType type)
    : client_(client), id_(id), name_(name), description_(description), readonly_(readonly), timestamp_(timestamp), type_(type) {
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, long value, long min, long max)
    : Property(client, id, name, description, readonly, timestamp, ValueType::long_) {
    value_.long_ = value;
    min_value_.long_ = min;
    max_value_.long_ = max;
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, unsigned long value, unsigned long min, unsigned long max)
    : Property(client, id, name, description, readonly, timestamp, ValueType::ulong_) {
    value_.ulong_ = value;
    min_value_.ulong_ = min;
    max_value_.ulong_ = max;
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, double value, double min, double max)
    : Property(client, id, name, description, readonly, timestamp, ValueType::double_) {
    value_.double_ = value;
    min_value_.double_ = min;
    max_value_.double_ = max;
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, bool value)
    : Property(client, id, name, description, readonly, timestamp, ValueType::bool_) {
    value_.bool_ = value;
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp, const string &value)
    : Property(client, id, name, description, readonly, timestamp, ValueType::string_) {
    value_.string_ = value.c_str();
}
Property::Property(Client *client, const string id, const string name, const string description, const bool readonly, long timestamp)
    : Property(client, id, name, description, readonly, timestamp, ValueType::unknown_) {
};
const string Property::get_id( void ) {
    return id_;
}
const string Property::get_name( void ) {
    return name_;
}
const string Property::get_description( void ) {
    return description_;
}
unsigned long Property::get_timestamp( void ) {
    return timestamp_;
}
bool Property::is_readonly( void ) {
    return readonly_;
}
bool Property::is_value_long( void ) {
    return type_ == ValueType::long_;
}
bool Property::is_value_ulong( void ) {
    return type_ == ValueType::ulong_;
}
bool Property::is_value_double( void ) {
    return type_ == ValueType::double_;
}
bool Property::is_value_bool( void ) {
    return type_ == ValueType::bool_;
}
bool Property::is_value_string( void ) {
    return type_ == ValueType::string_;
}
long Property::as_long( void ) {
    if(!is_value_long())
        return 0;
    return value_.long_;
}
unsigned long Property::as_ulong( void ) {
    if(!is_value_ulong())
        return 0;
    return value_.ulong_;
}
bool Property::as_bool( void ) {
    if(!is_value_bool())
        return false;
    return value_.bool_;
}
double Property::as_double( void ) {
    if(!is_value_double())
        return 0.0;
    return value_.double_;
}
const string Property::as_string( void ) {
    if(!is_value_string())
        return "";
    return value_.string_;
}
long Property::min_long( void ) {
    if(!is_value_long())
        return 0;
    return min_value_.long_;
}
long Property::max_long( void ) {
    if(!is_value_long())
        return 0;
    return max_value_.long_;
}
unsigned long Property::min_ulong( void ) {
    if(!is_value_ulong())
        return 0;
    return min_value_.ulong_;
}
unsigned long Property::max_ulong( void ) {
    if(!is_value_ulong())
        return 0;
    return max_value_.ulong_;
}
double Property::min_double( void ) {
    if(!is_value_double())
        return 0.0;
    return min_value_.double_;
}
double Property::nax_double( void ) {
    if(!is_value_double())
        return 0.0;
    return max_value_.double_;
}
}
