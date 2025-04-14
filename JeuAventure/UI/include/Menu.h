#pragma once

#include "UIElements.h"
#include "Widget.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <string>

enum class MenuType {
    MainMenu,
    PauseMenu,
    OptionsMenu,
    GameOverMenu,
    CreditsMenu,
    CustomMenu
};

class MenuOption {
private:
    std::string m_text;
    std::string m_value;
    std::function<void()> m_action;
    bool m_enabled;
    bool m_selected;

public:
    MenuOption(const std::string& text, const std::function<void()>& action = nullptr);
    MenuOption(const std::string& text, const std::string& value, const std::function<void()>& action = nullptr);

    const std::string& getText() const;
    void setText(const std::string& text);

    const std::string& getValue() const;
    void setValue(const std::string& value);

    void execute() const;
    void setAction(const std::function<void()>& action);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isSelected() const;
    void setSelected(bool selected);
};

class Menu : public UIElements {
private:
    MenuType m_type;
    std::string m_title;

    sf::RectangleShape m_background;
    sf::Texture* m_backgroundTexture;

    std::vector<std::unique_ptr<Button>> m_optionButtons;
    std::vector<MenuOption> m_options;
    int m_selectedIndex;

    sf::Font m_font;
    sf::Color m_backgroundColor;
    sf::Color m_textColor;
    sf::Color m_highlightColor;

    float m_animationTime;
    bool m_isAnimating;

    float m_optionSpacing;
    sf::Vector2f m_optionSize;
    bool m_centerOptions;

    std::function<void()> m_onClose;

protected:
    void updateLayout();

public:
    Menu();
    Menu(MenuType type, const std::string& title);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void addOption(const MenuOption& option);
    void addOption(const std::string& text, const std::function<void()>& action = nullptr);
    void addOption(const std::string& text, const std::string& value, const std::function<void()>& action = nullptr);
    void removeOption(int index);
    void clearOptions();

    void selectOption(int index);
    void selectNextOption();
    void selectPreviousOption();
    void executeSelectedOption();
    int getSelectedIndex() const;

    void setTitle(const std::string& title);
    const std::string& getTitle() const;

    void setType(MenuType type);
    MenuType getType() const;

    void setBackgroundTexture(sf::Texture* texture);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setHighlightColor(const sf::Color& color);
    void setFont(const sf::Font& font);

    void setOptionSpacing(float spacing);
    void setOptionSize(const sf::Vector2f& size);
    void setCenterOptions(bool center);

    void startAnimation();
    bool isAnimating() const;

    void setOnCloseCallback(const std::function<void()>& callback);
    void close();
};

class MainMenu : public Menu {
public:
    MainMenu();
};

class PauseMenu : public Menu {
public:
    PauseMenu();
};

class OptionsMenu : public Menu {
private:
    std::unique_ptr<Panel> m_controlsPanel;
    std::unique_ptr<Panel> m_audioPanel;
    std::unique_ptr<Panel> m_videoPanel;

    std::unique_ptr<Dropdown> m_resolutionDropdown;
    std::unique_ptr<CheckBox> m_fullscreenCheckbox;
    std::unique_ptr<ProgressBar> m_musicVolumeSlider;
    std::unique_ptr<ProgressBar> m_sfxVolumeSlider;

    std::vector<std::pair<std::string, std::string>> m_keybindings;

    void applySettings();
    void resetToDefaults();

public:
    OptionsMenu();

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void loadCurrentSettings();
};

class GameOverMenu : public Menu {
private:
    std::unique_ptr<Label> m_scoreLabel;

public:
    GameOverMenu(int score = 0);

    void setScore(int score);
};

class CreditsMenu : public Menu {
private:
    std::unique_ptr<Panel> m_creditsPanel;
    std::vector<std::unique_ptr<Label>> m_creditLabels;

    float m_scrollSpeed;
    float m_scrollPosition;
    bool m_autoScroll;

public:
    CreditsMenu();

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void setScrollSpeed(float speed);
    void setAutoScroll(bool autoScroll);
    void addCreditLine(const std::string& text, unsigned int size = 24, const sf::Color& color = sf::Color::White);
};
