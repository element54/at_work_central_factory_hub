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

#include <core/threading/thread.h>
#include <aspect/logging.h>
#include <aspect/configurable.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <netcomm/dns-sd/avahi_thread.h>
#include <netcomm/dns-sd/avahi_resolver_handler.h>
#include <boost/thread/thread.hpp>
#include <cpprest/http_client.h>
#include <vector>
#include <unordered_map>
#include "listener.h"

namespace PropertyClient {

class Client: public fawkes::Thread, public fawkes::ServiceBrowseHandler
{
    public:
        Client(Listener *listener, const char *device_id);
        ~Client();

        virtual void loop();

        virtual void all_for_now();
        virtual void cache_exhausted();
        virtual void browse_failed(const char *name, const char *type, const char *domain);
        virtual void service_added(const char *name, const char *type, const char *domain, const char *host_name, const struct sockaddr *addr, const socklen_t addr_size, uint16_t port, std::list<std::string> &txt, int flags );
        virtual void service_removed(const char *name, const char *type, const char *domain);

        bool is_connected( void );
        void process_response( web::http::http_response &response );
        bool process_property( web::json::object &property );

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
        struct Property {
            union ValueHolder value_;
            union ValueHolder min_;
            union ValueHolder max_;
            ValueType type_;
            unsigned long timestamp_;
        };

        Listener *listener_;
        const char *device_id_;
        fawkes::Mutex mutex_;
        fawkes::AvahiThread *avahi_thread_;

        const char *server_name_;
        web::http::client::http_client *http_client_;
        std::unordered_map<std::string, struct Property *> properties_;
};
}
