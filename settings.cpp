#include "settings.h"

Settings& Settings::instance()
{
    static Settings instance;
    return instance;
}

Item Settings::item(const std::string& name)
{
    std::lock_guard<std::mutex> lg(m_lock);
    return m_settings[name];
}

void Settings::set(const std::string& name, const Item& item)
{
    std::lock_guard<std::mutex> lg(m_lock);
    m_settings[name] = item;
}
