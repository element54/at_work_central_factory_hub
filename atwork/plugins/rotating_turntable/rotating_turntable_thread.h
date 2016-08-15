/***************************************************************************
 *  rotating_turntable_thread.h - Thread to communicate with the rotating turntable
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

#pragma once

#include <core/threading/thread.h>
#include <aspect/logging.h>
#include <aspect/clips.h>
#include <aspect/configurable.h>
#include <property_client/listener.h>
#include <property_client/client.h>



class RotatingTurntableThread : public fawkes::Thread, public fawkes::LoggingAspect, public fawkes::ConfigurableAspect,
    public fawkes::CLIPSAspect, public PropertyClient::Listener
{
public:
    RotatingTurntableThread();

    virtual void init();
    virtual void finalize();

    virtual void device_connected( const std::string &device_id );
    virtual void device_error( const std::string &device_id );
    virtual void device_disconnected( const std::string &device_id );
    virtual void property_changed( const std::string &device_id, std::shared_ptr<PropertyClient::Property> property );

private:
    PropertyClient::Client *client_;
    bool connected_ = false;
    bool running_ = false;

    void clips_rotating_turntable_start();
    void clips_rotating_turntable_stop();
    bool clips_rotating_turntable_is_connected();
    bool clips_rotating_turntable_is_running();
private:
};
