#pragma once

/***************************************************************************
 *  conveyour_belt_thread.h - Thread to communicate with the conveyor belt
 *
 *  Created: Mon Oct 06 16:39:11 2014
 *  Copyright  2014 Frederik Hegger
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */
#include "listener.h"
#include "client.h"
#include <memory>

namespace PropertyClient {
using namespace std;

class Client;
class Property {
    friend class Client;
public:
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              long value,
              long min,
              long max );
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              unsigned long value,
              unsigned long min,
              unsigned long max );
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              double value,
              double min,
              double max );
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              bool value );
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              const string &value );
    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp );
    const string get_id( void );
    const string get_name( void );
    const string get_description( void );
    unsigned long get_timestamp( void );
    bool is_readonly( void );

    bool is_long( void );
    bool is_ulong( void );
    bool is_double( void );
    bool is_bool( void );
    bool is_string( void );

    long as_long( void );
    unsigned long as_ulong( void );
    bool as_bool( void );
    double as_double( void );
    const string as_string( void );

    operator double( void ) {
        return as_double();
    };
    operator long( void ) {
        return as_long();
    };
    operator unsigned long( void ) {
        return as_ulong();
    };
    operator bool( void ) {
        return as_bool();
    };
    operator const string( void ) {
        return as_string();
    };

    long min_long( void );
    long max_long( void );
    unsigned long min_ulong( void );
    unsigned long max_ulong( void );
    double min_double( void );
    double max_double( void );

    bool set_long( long value );
    bool set_ulong( unsigned long value );
    bool set_bool( bool value );
    bool set_double( double value );
    bool set_string( const std::string &value );

    bool operator=( long value ) {
        return set_long( value );
    };
    bool operator=( unsigned long value ) {
        return set_ulong( value );
    };
    bool operator=( bool value ) {
        return set_bool( value );
    };
    bool operator=( double value ) {
        return set_double( value );
    };
    bool operator=( const string &value ) {
        return set_string( value );
    };
private:
    enum ValueType {
        bool_,
        string_,
        long_,
        ulong_,
        double_,
        unknown_
    };
    union ValueHolder {
        long long_;
        double double_;
        bool bool_;
        unsigned long ulong_;
        const char *string_;
    };
    Client *client_;
    const string uri_;
    const string id_;
    const string name_;
    const string description_;
    const bool readonly_;
    unsigned long timestamp_;
    const ValueType type_;
    union ValueHolder value_;
    union ValueHolder min_value_;
    union ValueHolder max_value_;

    Property( Client *client,
              const string uri,
              const string id,
              const string name,
              const string description,
              const bool readonly,
              long timestamp,
              const ValueType type );
};
}
