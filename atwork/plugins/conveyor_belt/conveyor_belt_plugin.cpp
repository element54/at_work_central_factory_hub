/***************************************************************************
 *  conveyor_belt_plugin.cpp - Conveyor belt plugin
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

#include "conveyor_belt_thread.h"

using namespace fawkes;
class ConveyorBeltPlugin: public fawkes::Plugin
{
    public:
        ConveyorBeltPlugin(Configuration *config) :
                Plugin(config)
        {
            thread_list.push_back(new ConveyorBeltThread());
        }
};

PLUGIN_DESCRIPTION("Plugin to communicate with the conveyor belt")
EXPORT_PLUGIN(ConveyorBeltPlugin)
