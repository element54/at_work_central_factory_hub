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
Client::Client(Listener *listener, const char *device_id) :
    Thread("PropertyClientThread", fawkes::Thread::OPMODE_CONTINUOUS), listener_(listener), device_id_(device_id),
    mutex_(fawkes::Mutex::Type::NORMAL)
{
    http_client_ = NULL;

    avahi_thread_ = new fawkes::AvahiThread();
    avahi_thread_->watch_service("_property_server._tcp", this);
    avahi_thread_->start();
}

Client::~Client()
{
    delete avahi_thread_;
}

void Client::loop() {
    {
        fawkes::MutexLocker lock(&mutex_);
        //std::cout << "YES " << is_connected() << std::endl;
        if(is_connected()) {
            std::string url = "/devices/";
            url.append(device_id_);
            try {
                auto response = http_client_->request(web::http::methods::GET, url).get();
                process_response(response);
            } catch(std::exception const & ex) {
                //TODO what?

            }
        }
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}


void Client::process_response( web::http::http_response &response ) {
    using namespace web;
    json::value value = response.extract_json().get();
    if(!value.is_object())
        return;
    json::object device = value.as_object();
    json::value properties_value = device["properties"];
    if(!properties_value.is_array())
        return;
    json::array properties = properties_value.as_array();
    std::vector<std::string> changed;
    for(auto it = properties.begin(); it != properties.end();it++) {
        json::value property_value = *it;
        if(!property_value.is_object())
            continue;
        json::object property = property_value.as_object();
        if(process_property(property)) {
            changed.push_back(property["id"].as_string());
        }
    }
    for(auto it = changed.begin(); it != changed.end();it++) {
        std::string property_id = *it;
        listener_->property_changed(device_id_, property_id.c_str());
    }
}

bool Client::process_property( web::json::object &property ) {
    using namespace web;
    std::string id = property["id"].as_string();
    auto it = properties_.find(id);
    struct Property *p;
    if(it == properties_.end()) {
        p = new struct Property;
        properties_[id] = p;
        std::string type = property["type"].as_string();
        if(type == "bool") {
            p->type_ = ValueType::bool_;
        } else if(type == "long") {
            p->type_ = ValueType::long_;
        } else if(type == "ulong") {
            p->type_ = ValueType::ulong_;
        } else if(type == "string") {
            p->type_ = ValueType::string_;
        } else if(type == "double") {
            p->type_ = ValueType::double_;
        } else {
            p->type_ = ValueType::unknown_;
        }
        p->timestamp_ = 0;
    } else {
        p = properties_[id];
    }
    unsigned long old_timestamp = p->timestamp_;
    unsigned long new_timestamp = property["timestamp"].as_number().to_uint64();
    /*std::cout << "ID: " << id << std::endl;
    std::cout << "OLD: " << old_timestamp << std::endl;
    std::cout << "NEW: " << new_timestamp << std::endl;
    std::cout << std::endl;*/
    p->timestamp_ = new_timestamp;
    if(old_timestamp != new_timestamp) {
        json::value value = property["value"];
        json::value min = property["min"];
        json::value max = property["max"];
        switch (p->type_) {
            case ValueType::bool_:
                p->value_.bool_ = value.as_bool();
                break;
            case ValueType::long_:
                p->value_.long_ = value.as_number().to_int64();
                p->min_.long_ = min.as_number().to_int64();
                p->max_.long_ = max.as_number().to_int64();
                break;
            case ValueType::ulong_:
                p->value_.ulong_ = value.as_number().to_uint64();
                p->min_.ulong_ = min.as_number().to_uint64();
                p->max_.ulong_ = max.as_number().to_uint64();
                break;
            case ValueType::double_:
                p->value_.double_ = value.as_double();
                p->min_.double_ = min.as_double();
                p->max_.double_ = max.as_double();
                break;
            case ValueType::string_:
                p->value_.string_ = value.as_string().c_str();
                break;
            default:
                break;
        }
        return true;
    } else {
        return false;
    }

}

void Client::all_for_now() {}
void Client::cache_exhausted() {}
void Client::browse_failed(const char *name, const char *type, const char *domain) {}
void Client::service_added(const char *name, const char *type, const char *domain, const char *host_name, const struct sockaddr *addr, const socklen_t addr_size, uint16_t port, std::list<std::string> &txt, int flags ) {
    fawkes::MutexLocker lock(&mutex_);
    if(is_connected())
        return;
    using namespace std;
    using namespace web;
    using namespace web::http;
    using namespace web::http::client;
    char *ip = inet_ntoa (((const struct sockaddr_in *)addr)->sin_addr);
    string base_url = "http://";
    base_url.append(ip);
    base_url.append(":");
    base_url.append(std::to_string(port));
    /*cout << "AVAHI ADDED" << endl;
    cout << "Name: " << name << endl;
    cout << "Base URL: " << base_url << endl;*/
    string url = "/devices/";
    url.append(device_id_);
    http_client *client = new http_client(base_url);
    try {
        auto response = client->request(methods::GET, url).get();
        if(response.status_code() != status_codes::OK) {
            delete client;
            client = NULL;
            return;
        } else {
            server_name_ = name;
            http_client_ = client;
            listener_->device_connected(device_id_);
        }
    } catch(std::exception const & ex) {
        if(client != NULL) {
            delete client;
        }
    }
    //client->request(methods::GET, url).then(std::bind(&Client::poll_callback, this, url, std::placeholders::_1));
    /*response.extract_json().then([=](json::value value) {
        if(!value.is_array())
            return;
        json::array list = value.as_array();
        for(auto list_it = list.begin(); list_it != list.end();list_it++) {
            json::value device = *list_it;
            std::string id = device.at("id").as_string();
            auto test = std::find(watchlist_.begin(), watchlist_.end(), id);
            if(test != watchlist_.end()) {
                clients_avahi_id_[name] = client;
                clients_device_id_[id] = client;
            }
        }
    });*/
}
void Client::service_removed(const char *name, const char *type, const char *domain) {
    using namespace std;
    //cout << "AVAHI REMOVED" << endl;
    //cout << "Name: " << name << endl;
    fawkes::MutexLocker lock(&mutex_);
    if(!is_connected())
        return;
    if(strcmp(server_name_, name) != 0)
        return;
    delete http_client_;
    http_client_ = NULL;

    for(auto it = properties_.begin(); it != properties_.end();it++) {
        delete it->second;
    }
    properties_.clear();
    listener_->device_disconnected(device_id_);
}
bool Client::is_connected( void ) {
    //fawkes::MutexLocker lock(&mutex_);
    return http_client_ != NULL;
}
}
