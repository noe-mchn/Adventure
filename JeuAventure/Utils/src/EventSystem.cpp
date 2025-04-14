#include "EventSystem.h"
#include <iostream>

EventSystem* EventSystem::s_instance = nullptr;

EventSystem* EventSystem::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new EventSystem();
    }
    return s_instance;
}

void EventSystem::cleanup() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void EventSystem::addEventListener(const std::string& eventName, EventCallback callback) {
    m_eventListeners[eventName].push_back(callback);
}

void EventSystem::removeAllEventListeners(const std::string& eventName) {
    auto it = m_eventListeners.find(eventName);
    if (it != m_eventListeners.end()) {
        m_eventListeners.erase(it);
    }
}

void EventSystem::triggerEvent(const std::string& eventName, const std::map<std::string, std::any>& params) {
    auto it = m_eventListeners.find(eventName);
    if (it != m_eventListeners.end()) {
        for (auto& callback : it->second) {
            try {
                callback(params);
            }
            catch (const std::exception& e) {
                std::cerr << "Error in event callback for event '" << eventName << "': " << e.what() << std::endl;
            }
        }
    }
}

bool EventSystem::hasEventListeners(const std::string& eventName) {
    auto it = m_eventListeners.find(eventName);
    return it != m_eventListeners.end() && !it->second.empty();
}

size_t EventSystem::getListenerCount(const std::string& eventName) {
    auto it = m_eventListeners.find(eventName);
    if (it != m_eventListeners.end()) {
        return it->second.size();
    }
    return 0;
}