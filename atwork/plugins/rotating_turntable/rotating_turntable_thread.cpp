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

