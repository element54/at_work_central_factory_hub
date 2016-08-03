/***************************************************************************
 *  conveyor_belt_thread.cpp - Thread to communicate with the conveyor belt
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

#include "client.h"

#include <core/threading/mutex.h>
#include <core/threading/mutex_locker.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>


#include <netcomm/dns-sd/avahi_thread.h>
#include <netcomm/dns-sd/avahi_resolver_handler.h>


#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cpprest/json.h>


namespace PropertyClient {

/** @class RotatingTurntableThread
 * Thread to communicate with @Work conveyor belt.
 * @author Frederik Hegger
 */

/** Constructor. */
Client::Client( Listener *listener, std::string device_id ) :
    Thread( "PropertyClientThread", fawkes::Thread::OPMODE_CONTINUOUS ), listener_( listener ), device_id_( device_id ),
    mutex_( fawkes::Mutex::Type::NORMAL ) {
    http_client_ = NULL;

    avahi_thread_ = new fawkes::AvahiThread();
    avahi_thread_->watch_service( "_property_server._tcp", this );
    avahi_thread_->start();
}

Client::~Client() {
    delete avahi_thread_;
}

void Client::loop() {
    {
        fawkes::MutexLocker lock( &mutex_ );
        //std::cout << "YES " << is_connected() << std::endl;
        if( is_connected() ) {
            std::string url = "/devices/";
            url.append( device_id_ );
            try {
                auto response = http_client_->request( web::http::methods::GET, url ).get();
                web::json::value value = response.extract_json().get();
                process_property_list( value );
            } catch ( web::http::http_exception const & ex ) {
                //TODO what?

            }
        }
    }
    boost::this_thread::sleep( boost::posix_time::milliseconds( 250 ) );
}


void Client::process_property_list( web::json::value &value ) {
    using namespace web;
    if( !value.is_object() )
        return;
    json::object device = value.as_object();
    json::value properties_value = device[ "properties" ];
    if( !properties_value.is_array() )
        return;
    json::array properties = properties_value.as_array();
    std::vector<std::shared_ptr<Property> > changed;
    for( auto it = properties.begin(); it != properties.end(); it++ ) {
    json::value property_value = *it;
    if( !property_value.is_object() )
        continue;
    json::object property = property_value.as_object();
    process_property( property, changed );
    }
    for( auto it = changed.begin(); it != changed.end(); it++ )
        listener_->property_changed( device_id_, *it );
}

void Client::process_property( web::json::object &property, std::vector<std::shared_ptr<Property> > &changed ) {
    using namespace web;
    std::string id = property[ "id" ].as_string();
    auto it = properties_.find( id );
    if( it == properties_.end() )
        properties_[ id ] = create_property( property );
    std::shared_ptr<Property> p = properties_[ id ];
    unsigned long old_timestamp = p->timestamp_;
    unsigned long new_timestamp = property[ "timestamp" ].as_number().to_uint64();
    //std::cout << "ID: " << id << std::endl;
    //std::cout << "OLD: " << old_timestamp << std::endl;
    //std::cout << "NEW: " << new_timestamp << std::endl;
    //std::cout << std::endl;
    p->timestamp_ = new_timestamp;
    if( old_timestamp != new_timestamp ) {
    json::value value = property[ "value" ];
    json::value min = property[ "min" ];
    json::value max = property[ "max" ];
    if( p->is_long() ) {
        p->value_.long_ = value.as_number().to_int64();
        p->min_value_.long_ = min.as_number().to_int64();
        p->max_value_.long_ = max.as_number().to_int64();
    } else if( p->is_ulong() ) {
        p->value_.ulong_ = value.as_number().to_uint64();
        p->min_value_.ulong_ = min.as_number().to_uint64();
        p->max_value_.ulong_ = max.as_number().to_uint64();
    } else if( p->is_double() ) {
        p->value_.double_ = value.as_number().to_double();
        p->min_value_.double_ = min.as_number().to_double();
        p->max_value_.double_ = max.as_number().to_double();
    } else if( p->is_bool() ) {
        p->value_.bool_ = value.as_bool();
    } else if( p->is_string() ) {
        p->value_.string_ = value.as_string().c_str();
    }
    changed.push_back( p );
    }
}

std::shared_ptr<Property> Client::create_property( web::json::object &property ) {
    std::shared_ptr<Property> p;
    std::string id = property[ "id" ].as_string();
    std::string name = property[ "name" ].as_string();
    std::string uri = property[ "uri" ].as_string();
    std::string description = property[ "description" ].as_string();
    bool readonly = property[ "readonly" ].as_bool();
    std::string type = property[ "type" ].as_string();
    if( type == "bool" ) {
        bool value = property[ "value" ].as_bool();
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0, value );
    } else if( type == "long" ) {
        long value = property[ "value" ].as_number().to_int64();
        long min = property[ "min" ].as_number().to_int64();
        long max = property[ "max" ].as_number().to_int64();
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0, value, min, max );
    } else if( type == "ulong" ) {
        unsigned long value = property[ "value" ].as_number().to_uint64();
        unsigned long min = property[ "min" ].as_number().to_uint64();
        unsigned long max = property[ "max" ].as_number().to_uint64();
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0, value, min, max );
    } else if( type == "string" ) {
        std::string value = property[ "value" ].as_string();
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0, value );
    } else if( type == "double" ) {
        double value = property[ "value" ].as_number().to_double();
        double min = property[ "min" ].as_number().to_double();
        double max = property[ "max" ].as_number().to_double();
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0, value, min, max );
    } else {
        p = std::make_shared<Property>( this, uri, id, name, description, readonly, 0 );
    }
    return p;
}

bool Client::send_property_update( Property *property, web::json::value &object ) {
    try {
        auto response = http_client_->request( web::http::methods::PUT, property->uri_, object ).get();
        return response.status_code() == web::http::status_codes::OK;
    } catch ( web::http::http_exception const & ex ) {
        return false;
        //TODO what?
    }
}

bool Client::update_property( Property *property, long value ) {
    using namespace web;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return false;
    if( !property->is_long() )
        return false;
    json::value object = json::value::object();
    object[ "value" ] = json::value::number( value );
    return send_property_update( property, object );
}
bool Client::update_property( Property *property, unsigned long value ) {
    using namespace web;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return false;
    if( !property->is_ulong() )
        return false;
    json::value object = json::value::object();
    object[ "value" ] = json::value::number( value );
    return send_property_update( property, object );
}
bool Client::update_property( Property *property, double value ) {
    using namespace web;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return false;
    if( !property->is_double() )
        return false;
    json::value object = json::value::object();
    object[ "value" ] = json::value::number( value );
    return send_property_update( property, object );
}
bool Client::update_property( Property *property, bool value ) {
    using namespace web;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return false;
    if( !property->is_bool() )
        return false;
    json::value object = json::value::object();
    object[ "value" ] = json::value::boolean( value );
    return send_property_update( property, object );
}
bool Client::update_property( Property *property, const std::string &value ) {
    using namespace web;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return false;
    if( !property->is_string() )
        return false;
    json::value object = json::value::object();
    object[ "value" ] = json::value::string( value );
    return send_property_update( property, object );
}

void Client::all_for_now() {
}
void Client::cache_exhausted() {
}
void Client::browse_failed( const char *name, const char *type, const char *domain ) {
}
void Client::service_added( const char *name,
                            const char *type,
                            const char *domain,
                            const char *host_name,
                            const struct sockaddr *addr,
                            const socklen_t addr_size,
                            uint16_t port,
                            std::list<std::string> &txt,
                            int flags ) {
    fawkes::MutexLocker lock( &mutex_ );

    if( is_connected() )
        return;
    using namespace std;
    using namespace web;
    using namespace web::http;
    using namespace web::http::client;
    char *ip = inet_ntoa( ( (const struct sockaddr_in *)addr )->sin_addr );
    string base_url = "http://";
    base_url.append( ip );
    base_url.append( ":" );
    base_url.append( std::to_string( port ) );
    //cout << "AVAHI ADDED" << endl;
    //cout << "Name: " << name << endl;
    //cout << "Base URL: " << base_url << endl;
    string url = "/devices/";
    url.append( device_id_ );
    http_client *client = new http_client( base_url );
    try {
    auto response = client->request( methods::GET, url ).get();
    if( response.status_code() != status_codes::OK ) {
        delete client;
        client = NULL;
        return;
    } else {
        server_name_ = name;
        http_client_ = client;
        web::json::value value = response.extract_json().get();
        process_property_list( value );
        listener_->device_connected( device_id_ );
    }
    } catch ( web::http::http_exception const & ex ) {
        if( client != NULL ) {
            delete client;
        }
    }
}
void Client::service_removed( const char *name, const char *type, const char *domain ) {
    using namespace std;
    //cout << "AVAHI REMOVED" << endl;
    //cout << "Name: " << name << endl;
    fawkes::MutexLocker lock( &mutex_ );
    if( !is_connected() )
        return;
    if( server_name_ != name )
        //if(strcmp(server_name_, name) != 0)
        return;
    delete http_client_;
    http_client_ = NULL;

    properties_.clear();
    listener_->device_disconnected( device_id_ );
}
bool Client::is_connected( void ) {
    //fawkes::MutexLocker lock(&mutex_);
    return http_client_ != NULL;
}
bool Client::exists_property( const std::string &id ) {
    fawkes::MutexLocker lock( &mutex_ );

    return properties_.find( id ) != properties_.end();
}
std::shared_ptr<Property> Client::get_property( const std::string &id ) {
    fawkes::MutexLocker lock( &mutex_ );

    return properties_[ id ];
}
}
