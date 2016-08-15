/***************************************************************************
 *  rotating_turntable_plugin.cpp - Rotating Turntable plugin
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

#include <core/plugin.h>

#include "rotating_turntable_thread.h"

using namespace fawkes;

class RotatingTurntablePlugin : public fawkes::Plugin
{
public:
    RotatingTurntablePlugin( Configuration *config ) :
        Plugin( config ) {
        thread_list.push_back( new RotatingTurntableThread() );
    }
};

PLUGIN_DESCRIPTION( "Plugin to communicate with the rotating turntable" )
EXPORT_PLUGIN( RotatingTurntablePlugin )
