#pragma once

#include <map>
#include <memory>
#include <mutex>

#include <mapbox/variant.hpp>
using mapbox::util::variant;

typedef variant<int, double, std::string> Item;

class Settings
{
public:
    Settings& instance();

    Item item(const std::string& name);

    void set(const std::string& name, const Item& item);

private:
    std::map<std::string, Item> m_settings;
    std::mutex m_lock;
};
