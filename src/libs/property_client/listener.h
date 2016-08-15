/***************************************************************************
 *  listener.h
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

#include <string>
#include <memory>
#include "property.h"

namespace PropertyClient {
class Property;
class Listener {
public:
    virtual ~Listener() {
    };
    virtual void device_connected( const std::string &device_id ) = 0;
    virtual void device_error( const std::string &device_id ) = 0;
    virtual void device_disconnected( const std::string &device_id ) = 0;
    virtual void property_changed( const std::string &device_id, std::shared_ptr<Property> property ) = 0;
};
}
