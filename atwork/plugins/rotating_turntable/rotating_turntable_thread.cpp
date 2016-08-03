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
    Thread( "RotatingTurntableThread", Thread::OPMODE_CONTINUOUS ) {
}

void RotatingTurntableThread::init() {
    fawkes::MutexLocker lock( clips_mutex );

    clips->add_function( "rotating-turntable-start",
                         sigc::slot<void>( sigc::mem_fun( *this, &RotatingTurntableThread::clips_rotating_turntable_start ) ) );
    clips->add_function( "rotating-turntable-stop",
                         sigc::slot<void>( sigc::mem_fun( *this, &RotatingTurntableThread::clips_rotating_turntable_stop ) ) );
    clips->add_function( "rotating-turntable-is-running",
                         sigc::slot<bool>( sigc::mem_fun( *this, &RotatingTurntableThread::clips_rotating_turntable_is_running ) ) );
    clips->add_function( "rotating-turntable-is-connected",
                         sigc::slot<bool>( sigc::mem_fun( *this, &RotatingTurntableThread::clips_rotating_turntable_is_connected ) ) );

    client_ = new PropertyClient::Client( this, "rtt" );
    client_->start();

    if( !clips->build( "(deffacts have-feature-rotating-turntable (have-feature RotatingTurntable))" ) )
        logger->log_warn( "ConveyorBelt", "Failed to build deffacts have-feature-rotating-turntable" );
}

void RotatingTurntableThread::finalize() {
    delete client_;
}

void RotatingTurntableThread::loop() {
    /*if(client_->is_connected()) {
        std::shared_ptr<PropertyClient::Property> property = client_->get_property("target_speed");
        std::cout << property->get_id() << std::endl;
        std::cout << property->set_long(100) << std::endl;
       }*/
    boost::this_thread::sleep( boost::posix_time::milliseconds( 1000 ) );
}


void RotatingTurntableThread::device_connected( const std::string &device_id ) {
    //std::cout << "device_connected: " << device_id << std::endl;
    connected_ = true;
}

void RotatingTurntableThread::device_disconnected( const std::string &device_id ) {
    //std::cout << "device_disconnected: " << device_id << std::endl;
    connected_ = false;
    running_ = false;
}

void RotatingTurntableThread::property_changed( const std::string &device_id, std::shared_ptr<PropertyClient::Property> property ) {
    //std::cout << "property_changed: " << device_id << ", " << property->get_id() << std::endl;
    if( property->get_id() == "current_speed" ) {
        running_ = (property->as_long() != 0l);
    }
}
void RotatingTurntableThread::device_error( const std::string &device_id ) {
    std::cout << "device_error: " << device_id << std::endl;
}

void RotatingTurntableThread::clips_rotating_turntable_start() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_long( 100 );
}

void RotatingTurntableThread::clips_rotating_turntable_stop() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_long( 0 );
}


bool RotatingTurntableThread::clips_rotating_turntable_is_connected() {
    return connected_;
}
bool RotatingTurntableThread::clips_rotating_turntable_is_running() {
    return running_;
}
