#pragma once
namespace PropertyClient {
class Listener {
public:
    virtual ~Listener() {};
    virtual void device_connected(const char* device_id) = 0;
    virtual void device_error(const char* device_id) = 0;
    virtual void device_disconnected(const char* device_id) = 0;
    virtual void property_changed(const char* device_id, const char* property_id) = 0;
};
}
