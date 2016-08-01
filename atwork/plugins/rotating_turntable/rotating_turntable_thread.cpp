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

    client_ = new PropertyClient::Client(this, "rtt");
    client_->start();

    if (!clips->build("(deffacts have-feature-rotating-turntable (have-feature RotatingTurntable))"))
        logger->log_warn("ConveyorBelt", "Failed to build deffacts have-feature-rotating-turntable");
}

void RotatingTurntableThread::finalize()
{
    delete client_;
}

void RotatingTurntableThread::loop()
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}


void RotatingTurntableThread::device_connected(const char* device_id) {
    std::cout << "device_connected: " << device_id << std::endl;
}

void RotatingTurntableThread::device_disconnected(const char* device_id) {
    std::cout << "device_disconnected: " << device_id << std::endl;
}

void RotatingTurntableThread::property_changed(const char* device_id, const char* property_id) {
    std::cout << "property_changed: " << device_id << ", " << property_id << std::endl;
}
void RotatingTurntableThread::device_error(const char* device_id) {
    std::cout << "device_error: " << device_id << std::endl;
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
    return false;
}
bool RotatingTurntableThread::clips_rotating_turntable_is_running() {
    return false;
}
