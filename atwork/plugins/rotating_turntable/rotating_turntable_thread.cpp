/***************************************************************************
 *  rotating_turntable_thread.cpp - Thread to communicate with the rotating turntable
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

#include "rotating_turntable_thread.h"

#include <core/threading/mutex_locker.h>

#define LOG_COMPONENT "RotatingTurntable"

RotatingTurntableThread::RotatingTurntableThread() :
    Thread( "RotatingTurntableThread", Thread::OPMODE_WAITFORWAKEUP ) {
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

    client_ = new PropertyClient::Client( this, "rotating_turntable" );
    client_->start();

    if( !clips->build( "(deffacts have-feature-rotating-turntable (have-feature RotatingTurntable))" ) )
        logger->log_warn( "RotatingTurntable", "Failed to build deffacts have-feature-rotating-turntable" );
}

void RotatingTurntableThread::finalize() {
    delete client_;
}


void RotatingTurntableThread::device_connected( const std::string &device_id ) {
    connected_ = true;
    logger->log_info(LOG_COMPONENT, "%s connected", device_id.c_str());
}

void RotatingTurntableThread::device_disconnected( const std::string &device_id ) {
    connected_ = false;
    running_ = false;
    logger->log_info(LOG_COMPONENT, "%s disconnected", device_id.c_str());
}

void RotatingTurntableThread::property_changed( const std::string &device_id, std::shared_ptr<PropertyClient::Property> property ) {
    if( property->get_id() == "current_speed" ) {
        running_ = (property->as_double() != 0.0);
        if(running_) {
            logger->log_info(LOG_COMPONENT, "%s is running", device_id.c_str());
        } else {
            logger->log_info(LOG_COMPONENT, "%s is NOT running", device_id.c_str());
        }
    }
}
void RotatingTurntableThread::device_error( const std::string &device_id ) {
    logger->log_error(LOG_COMPONENT, "%s error", device_id.c_str());
}

void RotatingTurntableThread::clips_rotating_turntable_start() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_double( 180.0 );
}

void RotatingTurntableThread::clips_rotating_turntable_stop() {
    if( !client_->is_connected() )
        return;
    std::shared_ptr<PropertyClient::Property> property = client_->get_property( "target_speed" );
    if( property == NULL )
        return;
    property->set_double( 0.0 );
}


bool RotatingTurntableThread::clips_rotating_turntable_is_connected() {
    return connected_;
}
bool RotatingTurntableThread::clips_rotating_turntable_is_running() {
    return running_;
}
