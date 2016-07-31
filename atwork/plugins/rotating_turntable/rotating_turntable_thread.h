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

#ifndef __PLUGINS_ROTATING_TURNTABLE_THREAD_H_
#define __PLUGINS_ROTATING_TURNTABLE_THREAD_H_

#include <core/threading/thread.h>
#include <aspect/logging.h>
#include <aspect/clips.h>
#include <aspect/configurable.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <netcomm/dns-sd/avahi_thread.h>
#include <netcomm/dns-sd/avahi_resolver_handler.h>
#include <boost/thread/thread.hpp>
#include <cpprest/http_client.h>



class RotatingTurntableThread: public fawkes::Thread, public fawkes::LoggingAspect, public fawkes::ConfigurableAspect, public fawkes::CLIPSAspect, public fawkes::ServiceBrowseHandler
{
    public:
        RotatingTurntableThread();

        virtual void init();
        virtual void loop();
        virtual void finalize();

        virtual void all_for_now();
        virtual void cache_exhausted();
        virtual void browse_failed(const char *name, const char *type, const char *domain);
        virtual void service_added(const char *name, const char *type, const char *domain, const char *host_name, const struct sockaddr *addr, const socklen_t addr_size, uint16_t port, std::list<std::string> &txt, int flags );
        virtual void service_removed(const char *name, const char *type, const char *domain);

    private:
        fawkes::AvahiThread *avahi_thread_;

        std::shared_ptr<web::http::client::http_client> http_client_;
        std::string server_name_;
        std::string device_uri_;

        void clips_rotating_turntable_start();
        void clips_rotating_turntable_stop();
        bool clips_rotating_turntable_is_connected();
        bool clips_rotating_turntable_is_running();
    private:
};

#endif
