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
