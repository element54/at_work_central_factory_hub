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

    clips->add_function("rotating-turntable-start", sigc::slot<void>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_rotating_turntable_start)));
    clips->add_function("rotating-turntable-stop", sigc::slot<void>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_rotating_turntable_stop)));
    clips->add_function("rotating-turntable-is-running", sigc::slot<bool>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_rotating_turntable_is_running)));
    clips->add_function("rotating-turntable-is-connected", sigc::slot<bool>(sigc::mem_fun(*this, &RotatingTurntableThread::clips_rotating_turntable_is_connected)));

    http_client_ = nullptr;

    avahi_thread_ = new fawkes::AvahiThread();
    avahi_thread_->watch_service("_property_server._tcp", this);
    avahi_thread_->start();
    //if (!clips->build("(deffacts have-feature-conveyor-belt (have-feature ConveyorBelt))"))
    //    logger->log_warn("ConveyorBelt", "Failed to build deffacts have-feature-conveyor-belt");
}

void RotatingTurntableThread::finalize()
{
    delete avahi_thread_;
}

void RotatingTurntableThread::loop()
{
    if(http_client_ != NULL) {
        std::cout << "JEPPP" << std::endl;
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void RotatingTurntableThread::clips_rotating_turntable_start()
{
    std::cout << "STARTSTART" << std::endl;
}

void RotatingTurntableThread::clips_rotating_turntable_stop()
{
    std::cout << "STOPSTOP" << std::endl;
}


bool RotatingTurntableThread::clips_rotating_turntable_is_connected() {
    return http_client_ != NULL;
}
bool RotatingTurntableThread::clips_rotating_turntable_is_running() {
    return false;
}

void RotatingTurntableThread::all_for_now() {}
void RotatingTurntableThread::cache_exhausted() {}
void RotatingTurntableThread::browse_failed(const char *name, const char *type, const char *domain) {}
void RotatingTurntableThread::service_added(const char *name, const char *type, const char *domain, const char *host_name, const struct sockaddr *addr, const socklen_t addr_size, uint16_t port, std::list<std::string> &txt, int flags ) {
    if(http_client_ == NULL) {
        using namespace std;
        using namespace web::http;
        using namespace web::http::client;
        char *ip = inet_ntoa (((const struct sockaddr_in *)addr)->sin_addr);
        string url = "http://";
        url.append(ip);
        url.append(":");
        url.append(std::to_string(port));
        cout << "AVAHI ADDED" << endl;
        cout << "Name: " << name << endl;
        cout << "URL: " << url << endl;
        shared_ptr<http_client> client(new http_client(url));
        client->request(methods::GET, "/devices/rtt").then([=](http_response response) {
            if(response.status_code() == status_codes::OK) {
                server_name_ = name;
                device_uri_ = "/devices/rtt";
                http_client_ = client;
            }
        });
    }
}
void RotatingTurntableThread::service_removed(const char *name, const char *type, const char *domain) {
    using namespace std;
    cout << "AVAHI REMOVED" << endl;
    cout << "Name: " << name << endl;
    if(http_client_ != NULL) {
        if(server_name_ == name) {
            http_client_ = nullptr;
        }
    }
}
