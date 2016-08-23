/***************************************************************************
 *  conveyor_belt_thread.cpp - Thread to communicate with the rotating turntable
 *
 *  Copyright  2016 Torsten Jandt
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

#include "conveyor_belt_thread.h"

#include <core/threading/mutex_locker.h>

#define LOG_COMPONENT "ConveyorBelt"

ConveyorBeltThread::ConveyorBeltThread() :
    Thread( "ConveyorBeltThread", Thread::OPMODE_WAITFORWAKEUP ) {
}

void ConveyorBeltThread::init() {
    fawkes::MutexLocker lock( clips_mutex );

    clips->add_function( "conveyor-belt-start",
                         sigc::slot<void>( sigc::mem_fun( *this, &ConveyorBeltThread::clips_conveyor_belt_start ) ) );
    clips->add_function( "conveyor-belt-stop",
                         sigc::slot<void>( sigc::mem_fun( *this, &ConveyorBeltThread::clips_conveyor_belt_stop ) ) );
    clips->add_function( "conveyor-belt-is-running",
                         sigc::slot<bool>( sigc::mem_fun( *this, &ConveyorBeltThread::clips_conveyor_belt_is_running ) ) );
    clips->add_function( "conveyor-belt-is-connected",
                         sigc::slot<bool>( sigc::mem_fun( *this, &ConveyorBeltThread::clips_conveyor_belt_is_connected ) ) );

    client_ = new PropertyClient::Client( this, "conveyor_belt" );
    client_->start();

    if( !clips->build( "(deffacts have-feature-conveyor-belt (have-feature ConveyorBelt))" ) )
        logger->log_warn( "ConveyorBelt", "Failed to build deffacts have-feature-conveyor-belt" );
}

void ConveyorBeltThread::finalize() {
    delete client_;
}


void ConveyorBeltThread::device_connected( const std::string &device_id ) {
    connected_ = true;
    logger->log_info(LOG_COMPONENT, "%s connected", device_id.c_str());
}

void ConveyorBeltThread::device_disconnected( const std::string &device_id ) {
    connected_ = false;
    running_ = false;
    logger->log_info(LOG_COMPONENT, "%s disconnected", device_id.c_str());
}

void ConveyorBeltThread::property_changed( const std::string &device_id, std::shared_ptr<PropertyClient::Property> property ) {
    if( property->get_id() == "current_speed" ) {
        running_ = (property->as_double() != 0.0);
        if(running_) {
            logger->log_info(LOG_COMPONENT, "%s is running", device_id.c_str());
        } else {
            logger->log_info(LOG_COMPONENT, "%s is NOT running", device_id.c_str());
        }
    }
}
void ConveyorBeltThread::device_error( const std::string &device_id ) {
    logger->log_error(LOG_COMPONENT, "%s error", device_id.c_str());
}

void ConveyorBeltThread::clips_conveyor_belt_start() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_long( 100 );
}

void ConveyorBeltThread::clips_conveyor_belt_stop() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_long( 0 );
}


bool ConveyorBeltThread::clips_conveyor_belt_is_connected() {
    return connected_;
}
bool ConveyorBeltThread::clips_conveyor_belt_is_running() {
    return running_;
}
