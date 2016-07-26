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

#include "rotating_turntable_thread.h"

#include <core/threading/mutex_locker.h>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>


#include <netcomm/dns-sd/avahi_thread.h>
#include <netcomm/dns-sd/avahi_resolver_handler.h>


#include <netinet/in.h>
#include <arpa/inet.h>

class AHandler: public fawkes::ServiceBrowseHandler {
    public:
        virtual void all_for_now() {}
        virtual void cache_exhausted() {}
        virtual void browse_failed(const char *name, const char *type, const char *domain) {}
        virtual void service_added(const char *name, const char *type, const char *domain, const char *host_name, const struct sockaddr *addr, const socklen_t addr_size, uint16_t port, std::list<std::string> &txt, int flags ) {
            /*char *ip = inet_ntoa (((const struct sockaddr_in *)addr)->sin_addr);
            std::cout << "Name: " << name << std::endl;
            std::cout << "Type: " << type << std::endl;
            std::cout << "Domain: " << domain << std::endl;
            std::cout << "Hostname: " << host_name << std::endl;
            std::cout << "Addr: " << ip << std::endl;
            std::cout << "Port: " << port << std::endl;*/
            std::cout << "ADDED" << std::endl;
        }
        virtual void service_removed(const char *name, const char *type, const char *domain) {
            std::cout << "REMOVED" << std::endl;
        }
};

/** @class RotatingTurntableThread
 * Thread to communicate with @Work conveyor belt.
 * @author Frederik Hegger
 */

/** Constructor. */
RotatingTurntableThread::RotatingTurntableThread() :
        Thread("RotatingTurntableThread", Thread::OPMODE_CONTINUOUS)
{
}

void RotatingTurntableThread::init()
{
    fawkes::MutexLocker lock(clips_mutex);

    clips->add_function("rotating-turntable-start", sigc::slot<void>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_start_rotating_turntable)));
    clips->add_function("rotating-turntable-stop", sigc::slot<void>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_stop_rotating_turntable)));
    avahi_resolver_handler_ = new AHandler();
    avahi_thread_ = new fawkes::AvahiThread();
    avahi_thread_->watch_service("_property_server._tcp", avahi_resolver_handler_);
    avahi_thread_->start();
    //if (!clips->build("(deffacts have-feature-conveyor-belt (have-feature ConveyorBelt))"))
    //    logger->log_warn("ConveyorBelt", "Failed to build deffacts have-feature-conveyor-belt");
}

void RotatingTurntableThread::finalize()
{
}

void RotatingTurntableThread::loop()
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void RotatingTurntableThread::clips_start_rotating_turntable()
{
    std::cout << "STARTSTART" << std::endl;
}

void RotatingTurntableThread::clips_stop_rotating_turntable()
{
    std::cout << "STOPSTOP" << std::endl;
}
