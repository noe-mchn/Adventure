#include "Application.h"
#include "Game.h"
#include "RessourceManager.h"
#include "InputManager.h"
#include "EventSystem.h"
#include "Render.h"
#include "UIManager.h"
#include "AudioManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>

Application* Application::s_instance = nullptr;

Application::Application()
    : m_appName("Platformer Game"),
    m_windowWidth(2048),
    m_windowHeight(512),
    m_isFullscreen(false),
    m_isVSyncEnabled(true),
    m_targetFPS(60),
    m_isRunning(false),
    m_isPaused(false),
    m_frameTime(0.0f),
    m_avgFPS(0.0f),
    m_frameCount(0),
    m_assetPath(""),
    m_savePath("Saves/"),
    m_isDebugMode(false),
    m_argc(0),
    m_argv(nullptr)
{
}

Application::~Application() {

    InputManager::cleanup();
    EventSystem::cleanup();
    UIManager::cleanup();

    std::cout << "Application shutdown complete." << std::endl;
}

Application* Application::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new Application();
    }
    return s_instance;
}

void Application::initialize(int argc, char** argv) {
    std::cout << "Initializing application..." << std::endl;

    RessourceManager::getInstance();

    m_argc = argc;
    m_argv = argv;

    parseCommandLineArgs(argc, argv);

    loadConfig("data.json");

    m_game = std::make_unique<Game>();

    m_game->initialize(m_appName, m_windowWidth, m_windowHeight, m_isFullscreen);

    m_game->getWindow().setVerticalSyncEnabled(m_isVSyncEnabled);
    m_game->setTargetFPS(m_targetFPS);

    initializeSystems();

    m_fpsClock.restart();

    std::cout << "Application initialized successfully." << std::endl;
}

int Application::run() {
    if (!m_game) {
        std::cerr << "Error: Game not initialized!" << std::endl;
        return 1;
    }

    std::cout << "Starting main loop..." << std::endl;
    m_isRunning = true;

    while (m_isRunning) {
        m_game->run();

        m_frameTime = m_game->getFPS() > 0 ? 1.0f / m_game->getFPS() : 0.0f;
        m_frameCount++;

        if (m_fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            m_avgFPS = static_cast<float>(m_frameCount) / m_fpsClock.getElapsedTime().asSeconds();
            m_frameCount = 0;
            m_fpsClock.restart();

            if (m_isDebugMode) {
                std::cout << "FPS: " << m_avgFPS << std::endl;
            }
        }
    }

    std::cout << "Main loop ended." << std::endl;
    return 0;
}

Game* Application::getGame() const {
    return m_game.get();
}

StateManager* Application::getStateManager() const {
    return m_game ? &m_game->getStateManager() : nullptr;
}

RessourceManager* Application::getResourceManager() const {
    return RessourceManager::getInstance();
}

InputManager* Application::getInputManager() const {
    return InputManager::getInstance();
}

EventSystem* Application::getEventSystem() const {
    return EventSystem::getInstance();
}

Renderer* Application::getRenderer() const {
    return nullptr;
}

UIManager* Application::getUIManager() const {
    return UIManager::getInstance();
}

AudioManager* Application::getAudioManager() const {
    return nullptr;
}

RessourceManager* Application::getRessourceManager() {
    return RessourceManager::getInstance();
}

void Application::quit(int exitCode) {
    std::cout << "Application quit requested with exit code: " << exitCode << std::endl;
    m_isRunning = false;
}

void Application::pause() {
    if (!m_isPaused) {
        m_isPaused = true;
        if (m_game) m_game->pause();
    }
}

void Application::resume() {
    if (m_isPaused) {
        m_isPaused = false;
        if (m_game) m_game->resume();
    }
}

void Application::setWindowSize(unsigned int width, unsigned int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    if (m_game) m_game->setWindowSize(width, height);
}

void Application::setFullscreen(bool fullscreen) {
    m_isFullscreen = fullscreen;
    if (m_game) {
        if (m_isFullscreen != m_game->isFullscreen()) {
            m_game->toggleFullscreen();
        }
    }
}

void Application::setVSync(bool enabled) {
    m_isVSyncEnabled = enabled;
    if (m_game) m_game->getWindow().setVerticalSyncEnabled(enabled);
}

void Application::setTargetFPS(unsigned int fps) {
    m_targetFPS = fps;
    if (m_game) m_game->setTargetFPS(fps);
}

void Application::setAssetPath(const std::string& path) {
    m_assetPath = path;
    if (!m_assetPath.empty() && m_assetPath.back() != '/' && m_assetPath.back() != '\\') {
        m_assetPath += '/';
    }
}

const std::string& Application::getAssetPath() const {
    return m_assetPath;
}

void Application::setSavePath(const std::string& path) {
    m_savePath = path;
    if (!m_savePath.empty() && m_savePath.back() != '/' && m_savePath.back() != '\\') {
        m_savePath += '/';
    }

    std::filesystem::create_directories(m_savePath);
}

const std::string& Application::getSavePath() const {
    return m_savePath;
}

void Application::setDebugMode(bool enabled) {
    m_isDebugMode = enabled;
}

bool Application::isDebugMode() const {
    return m_isDebugMode;
}

float Application::getFrameTime() const {
    return m_frameTime;
}

float Application::getAverageFPS() const {
    return m_avgFPS;
}

void Application::initializeSystems() {

    EventSystem::getInstance()->addEventListener("QuitApplication",
        [this](const std::map<std::string, std::any>&) {
            this->quit();
        }
    );

    InputManager::getInstance()->bindKey(InputAction::PAUSE, sf::Keyboard::Escape);
    InputManager::getInstance()->bindKey(InputAction::MENU, sf::Keyboard::F11);
    InputManager::getInstance()->bindKey(InputAction::CONFIRM, sf::Keyboard::F3);

    EventSystem::getInstance()->addEventListener("InputPressed",
        [this](const std::map<std::string, std::any>& params) {
            if (params.count("action") && std::any_cast<std::string>(params.at("action")) == "ToggleDebug") {
                setDebugMode(!m_isDebugMode);
                std::cout << "Debug mode: " << (m_isDebugMode ? "ON" : "OFF") << std::endl;
            }
            else if (params.count("action") && std::any_cast<std::string>(params.at("action")) == "ToggleFullscreen") {
                setFullscreen(!m_isFullscreen);
            }
        }
    );

    std::filesystem::create_directories(m_savePath);

    std::cout << "All systems initialized." << std::endl;
}

void Application::parseCommandLineArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--debug" || arg == "-d") {
            setDebugMode(true);
        }
        else if (arg == "--fullscreen" || arg == "-f") {
            setFullscreen(true);
        }
        else if (arg == "--window-size" || arg == "-w") {
            if (i + 2 < argc) {
                try {
                    unsigned int width = std::stoi(argv[i + 1]);
                    unsigned int height = std::stoi(argv[i + 2]);
                    setWindowSize(width, height);
                    i += 2;
                }
                catch (const std::exception& e) {
                    std::cerr << "Invalid window size format. Using default." << std::endl;
                }
            }
        }
        else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                loadConfig(argv[i + 1]);
                i++;
            }
        }
    }
}

void Application::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cout << "Config file not found: " << configPath << ". Using default settings." << std::endl;
        return;
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);

                key.erase(key.find_last_not_of(" \t") + 1);
                key.erase(0, key.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t"));

                if (key == "window_width") {
                    m_windowWidth = std::stoi(value);
                }
                else if (key == "window_height") {
                    m_windowHeight = std::stoi(value);
                }
                else if (key == "fullscreen") {
                    m_isFullscreen = (value == "true" || value == "1");
                }
                else if (key == "vsync") {
                    m_isVSyncEnabled = (value == "true" || value == "1");
                }
                else if (key == "target_fps") {
                    m_targetFPS = std::stoi(value);
                }
                else if (key == "debug_mode") {
                    m_isDebugMode = (value == "true" || value == "1");
                }
                else if (key == "asset_path") {
                    setAssetPath(value);
                }
                else if (key == "save_path") {
                    setSavePath(value);
                }
                else if (key == "app_name") {
                    m_appName = value;
                }
            }
        }

        std::cout << "Config loaded from: " << configPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing config file: " << e.what() << std::endl;
    }

    file.close();
}