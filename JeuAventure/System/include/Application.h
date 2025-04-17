#pragma once
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>

class Game;
class StateManager;
class RessourceManager;
class InputManager;
class EventSystem;
class Renderer;
class UIManager;
class AudioManager;

class Application {
private:

    static Application* s_instance;

    std::unique_ptr<Game> m_game;

    std::string m_appName;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    bool m_isFullscreen;
    bool m_isVSyncEnabled;
    unsigned int m_targetFPS;

    bool m_isRunning;
    bool m_isPaused;

    float m_frameTime;
    float m_avgFPS;
    int m_frameCount;
    sf::Clock m_fpsClock;

    std::string m_assetPath;
    std::string m_savePath;

    bool m_isDebugMode;

    int m_argc;
    char** m_argv;

    Application();

    void initializeSystems();

    void parseCommandLineArgs(int argc, char** argv);

    void loadConfig(const std::string& configPath);

public:
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    ~Application();

    static Application* getInstance();

    void initialize(int argc, char** argv);

    RessourceManager* getRessourceManager();

    int run();

    Game* getGame() const;
    StateManager* getStateManager() const;
    RessourceManager* getResourceManager() const;
    InputManager* getInputManager() const;
    EventSystem* getEventSystem() const;
    Renderer* getRenderer() const;
    UIManager* getUIManager() const;
    AudioManager* getAudioManager() const;

    void quit(int exitCode = 0);
    void pause();
    void resume();

    void setWindowSize(unsigned int width, unsigned int height);
    void setFullscreen(bool fullscreen);
    void setVSync(bool enabled);
    void setTargetFPS(unsigned int fps);

    void setAssetPath(const std::string& path);
    const std::string& getAssetPath() const;
    void setSavePath(const std::string& path);
    const std::string& getSavePath() const;

    void setDebugMode(bool enabled);
    bool isDebugMode() const;

    float getFrameTime() const;
    float getAverageFPS() const;
};