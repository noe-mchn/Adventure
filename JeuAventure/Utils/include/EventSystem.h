#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <any>

using EventCallback = std::function<void(const std::map<std::string, std::any>&)>;

class EventSystem {
private:
    static EventSystem* s_instance;

    std::map<std::string, std::vector<EventCallback>> m_eventListeners;

    EventSystem() = default;

public:
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    static EventSystem* getInstance();

    static void cleanup();

    void addEventListener(const std::string& eventName, EventCallback callback);

    void removeAllEventListeners(const std::string& eventName);

    void triggerEvent(const std::string& eventName, const std::map<std::string, std::any>& params = {});

    bool hasEventListeners(const std::string& eventName);

    size_t getListenerCount(const std::string& eventName);
};
