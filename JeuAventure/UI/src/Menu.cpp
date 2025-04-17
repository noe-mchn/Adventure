#include "Menu.h"
#include <iostream>

MenuOption::MenuOption(const std::string& text, const std::function<void()>& action)
    : m_text(text),
    m_value(""),
    m_action(action),
    m_enabled(true),
    m_selected(false)
{
}

MenuOption::MenuOption(const std::string& text, const std::string& value, const std::function<void()>& action)
    : m_text(text),
    m_value(value),
    m_action(action),
    m_enabled(true),
    m_selected(false)
{
}

const std::string& MenuOption::getText() const {
    return m_text;
}

void MenuOption::setText(const std::string& text) {
    m_text = text;
}

const std::string& MenuOption::getValue() const {
    return m_value;
}

void MenuOption::setValue(const std::string& value) {
    m_value = value;
}

void MenuOption::execute() const {
    if (m_enabled && m_action) {
        m_action();
    }
}

void MenuOption::setAction(const std::function<void()>& action) {
    m_action = action;
}

bool MenuOption::isEnabled() const {
    return m_enabled;
}

void MenuOption::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool MenuOption::isSelected() const {
    return m_selected;
}

void MenuOption::setSelected(bool selected) {
    m_selected = selected;
}

Menu::Menu()
    : UIElement(),
    m_type(MenuType::CustomMenu),
    m_title("Menu"),
    m_backgroundTexture(nullptr),
    m_selectedIndex(-1),
    m_backgroundColor(sf::Color(20, 20, 40, 230)),
    m_textColor(sf::Color::White),
    m_highlightColor(sf::Color(100, 150, 255)),
    m_animationTime(0.0f),
    m_isAnimating(false),
    m_optionSpacing(20.0f),
    m_optionSize(sf::Vector2f(200.0f, 40.0f)),
    m_centerOptions(true)
{
    m_background.setFillColor(m_backgroundColor);
}

Menu::Menu(MenuType type, const std::string& title)
    : UIElement(title, sf::Vector2f(0, 0), sf::Vector2f(400, 400)),
    m_type(type),
    m_title(title),
    m_backgroundTexture(nullptr),
    m_selectedIndex(-1),
    m_backgroundColor(sf::Color(20, 20, 40, 230)),
    m_textColor(sf::Color::White),
    m_highlightColor(sf::Color(100, 150, 255)),
    m_animationTime(0.0f),
    m_isAnimating(false),
    m_optionSpacing(20.0f),
    m_optionSize(sf::Vector2f(200.0f, 40.0f)),
    m_centerOptions(true)
{
    m_background.setFillColor(m_backgroundColor);
    m_background.setSize(m_size);

    m_anchor = UIAnchor::Center;
}

void Menu::update(float dt) {
    if (m_isAnimating) {
        m_animationTime += dt * 2.0f;

        if (m_animationTime >= 1.0f) {
            m_animationTime = 1.0f;
            m_isAnimating = false;
        }

        float scale = 0.8f + 0.2f * m_animationTime;
        setScale(sf::Vector2f(scale, scale));

        float opacity = m_animationTime;
        sf::Color bgColor = m_backgroundColor;
        bgColor.a = static_cast<sf::Uint8>(255 * opacity);
        m_background.setFillColor(bgColor);
    }

    m_background.setPosition(getGlobalPosition());
    m_background.setSize(m_size);

    for (size_t i = 0; i < m_optionButtons.size(); ++i) {
        if (m_optionButtons[i]) {
            if (static_cast<int>(i) == m_selectedIndex) {
                m_optionButtons[i]->setNormalColor(m_highlightColor);
            }
            else {
                m_optionButtons[i]->setNormalColor(sf::Color(60, 60, 80));
            }
        }
    }

    UIElement::update(dt);
}

void Menu::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    if (m_backgroundTexture) {
        sf::Sprite bgSprite(*m_backgroundTexture);
        bgSprite.setPosition(getGlobalPosition());
        bgSprite.setScale(
            m_size.x / m_backgroundTexture->getSize().x,
            m_size.y / m_backgroundTexture->getSize().y
        );
        target.draw(bgSprite);
    }
    else {
        target.draw(m_background);
    }

    sf::Text titleText;
    titleText.setString(m_title);
    titleText.setFont(m_font);
    titleText.setCharacterSize(24);
    titleText.setFillColor(m_textColor);

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top);
    titleText.setPosition(
        getGlobalPosition().x + m_size.x / 2.0f,
        getGlobalPosition().y + 30.0f
    );

    target.draw(titleText);

    for (auto& button : m_optionButtons) {
        if (button && button->isVisible()) {
            button->render(target);
        }
    }

    UIElement::render(target);
}

bool Menu::handleEvent(const sf::Event& event) {
    for (auto& button : m_optionButtons) {
        if (button && button->isActive() && button->isInteractive()) {
            if (button->handleEvent(event)) {
                return true;
            }
        }
    }

    if (UIElement::handleEvent(event)) {
        return true;
    }

    if (!isActive() || !isInteractive()) return false;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            selectPreviousOption();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Down) {
            selectNextOption();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Return) {
            executeSelectedOption();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            close();
            return true;
        }
    }

    return false;
}

void Menu::addOption(const MenuOption& option) {
    m_options.push_back(option);

    auto button = std::make_unique<Button>(
        "option_" + std::to_string(m_options.size() - 1),
        option.getText(),
        sf::Vector2f(0, 0),
        m_optionSize
    );

    button->setNormalColor(sf::Color(60, 60, 80));
    button->setHoverColor(sf::Color(80, 80, 100));
    button->setPressedColor(sf::Color(40, 40, 60));
    button->setTextColor(m_textColor);

    button->setOnClickCallback([this, index = m_options.size() - 1]() {
        selectOption(index);
        executeSelectedOption();
        });

    m_optionButtons.push_back(std::move(button));

    if (m_options.size() == 1 && m_selectedIndex == -1) {
        selectOption(0);
    }

    updateLayout();
}

void Menu::addOption(const std::string& text, const std::function<void()>& action) {
    addOption(MenuOption(text, action));
}

void Menu::addOption(const std::string& text, const std::string& value, const std::function<void()>& action) {
    addOption(MenuOption(text, value, action));
}

void Menu::removeOption(int index) {
    if (index >= 0 && index < static_cast<int>(m_options.size())) {
        m_options.erase(m_options.begin() + index);
        m_optionButtons.erase(m_optionButtons.begin() + index);

        if (m_selectedIndex == index) {
            if (m_options.empty()) {
                m_selectedIndex = -1;
            }
            else if (m_selectedIndex >= static_cast<int>(m_options.size())) {
                m_selectedIndex = m_options.size() - 1;
            }
        }
        else if (m_selectedIndex > index) {
            m_selectedIndex--;
        }

        updateLayout();
    }
}

void Menu::clearOptions() {
    m_options.clear();
    m_optionButtons.clear();
    m_selectedIndex = -1;
}

void Menu::selectOption(int index) {
    if (index >= -1 && index < static_cast<int>(m_options.size()) && index != m_selectedIndex) {
        if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_options.size())) {
            m_options[m_selectedIndex].setSelected(false);
        }

        m_selectedIndex = index;

        if (m_selectedIndex >= 0) {
            m_options[m_selectedIndex].setSelected(true);
        }
    }
}

void Menu::selectNextOption() {
    int nextIndex = (m_selectedIndex + 1) % m_options.size();
    selectOption(nextIndex);
}

void Menu::selectPreviousOption() {
    int prevIndex = m_selectedIndex - 1;
    if (prevIndex < 0) {
        prevIndex = m_options.size() - 1;
    }
    selectOption(prevIndex);
}

void Menu::executeSelectedOption() {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_options.size())) {
        m_options[m_selectedIndex].execute();
    }
}

int Menu::getSelectedIndex() const {
    return m_selectedIndex;
}

void Menu::setTitle(const std::string& title) {
    m_title = title;
}

const std::string& Menu::getTitle() const {
    return m_title;
}

void Menu::setType(MenuType type) {
    m_type = type;
}

MenuType Menu::getType() const {
    return m_type;
}

void Menu::setBackgroundTexture(sf::Texture* texture) {
    m_backgroundTexture = texture;
}

void Menu::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
    m_background.setFillColor(color);
}

void Menu::setTextColor(const sf::Color& color) {
    m_textColor = color;

    for (auto& button : m_optionButtons) {
        if (button) {
            button->setTextColor(color);
        }
    }
}

void Menu::setHighlightColor(const sf::Color& color) {
    m_highlightColor = color;
}

void Menu::setFont(const sf::Font& font) {
    m_font = font;

    for (auto& button : m_optionButtons) {
        if (button) {
            button->setFont(font);
        }
    }
}

void Menu::setOptionSpacing(float spacing) {
    m_optionSpacing = spacing;
    updateLayout();
}

void Menu::setOptionSize(const sf::Vector2f& size) {
    m_optionSize = size;

    for (auto& button : m_optionButtons) {
        if (button) {
            button->setSize(size);
        }
    }

    updateLayout();
}

void Menu::setCenterOptions(bool center) {
    m_centerOptions = center;
    updateLayout();
}

void Menu::startAnimation() {
    m_isAnimating = true;
    m_animationTime = 0.0f;
    setScale(sf::Vector2f(0.8f, 0.8f));
}

bool Menu::isAnimating() const {
    return m_isAnimating;
}

void Menu::setOnCloseCallback(const std::function<void()>& callback) {
    m_onClose = callback;
}

void Menu::close() {
    if (m_onClose) {
        m_onClose();
    }
}

void Menu::updateLayout() {
    float totalOptionsHeight = m_optionButtons.size() * (m_optionSize.y + m_optionSpacing) - m_optionSpacing;

    float startY = 80.0f;

    for (size_t i = 0; i < m_optionButtons.size(); ++i) {
        if (m_optionButtons[i]) {
            float xPos = m_centerOptions ? (m_size.x - m_optionSize.x) / 2.0f : 30.0f;
            float yPos = startY + i * (m_optionSize.y + m_optionSpacing);

            m_optionButtons[i]->setPosition(sf::Vector2f(xPos, yPos));
        }
    }

    float desiredHeight = startY + totalOptionsHeight + 50.0f;
    if (desiredHeight > m_size.y) {
        setSize(sf::Vector2f(m_size.x, desiredHeight));
    }
}

MainMenu::MainMenu()
    : Menu(MenuType::MainMenu, "Main Menu")
{
    setSize(sf::Vector2f(400, 450));
    setBackgroundColor(sf::Color(20, 20, 40, 230));

    addOption("Play", []() {
        std::cout << "Play selected" << std::endl;
        });

    addOption("Options", []() {
        std::cout << "Options selected" << std::endl;
        });

    addOption("Credits", []() {
        std::cout << "Credits selected" << std::endl;
        });

    addOption("Quit", []() {
        std::cout << "Quit selected" << std::endl;
        });

    selectOption(0);
}

PauseMenu::PauseMenu()
    : Menu(MenuType::PauseMenu, "Pause")
{
    setSize(sf::Vector2f(350, 400));
    setBackgroundColor(sf::Color(20, 20, 40, 230));

    addOption("Resume", []() {
        std::cout << "Resume selected" << std::endl;
        });

    addOption("Options", []() {
        std::cout << "Options selected" << std::endl;
        });

    addOption("Quit to Main Menu", []() {
        std::cout << "Quit to Main Menu selected" << std::endl;
        });

    selectOption(0);
}

OptionsMenu::OptionsMenu()
    : Menu(MenuType::OptionsMenu, "Options")
{
    setSize(sf::Vector2f(500, 500));
    setBackgroundColor(sf::Color(20, 20, 40, 230));

    m_controlsPanel = std::make_unique<Panel>("controlsPanel", sf::Vector2f(20, 80), sf::Vector2f(460, 120));
    m_controlsPanel->setDrawBackground(true);
    m_controlsPanel->setBackgroundColor(sf::Color(40, 40, 60));
    m_controlsPanel->setBorder(true);
    m_controlsPanel->setBorderColor(sf::Color(80, 80, 100));

    m_audioPanel = std::make_unique<Panel>("audioPanel", sf::Vector2f(20, 220), sf::Vector2f(460, 100));
    m_audioPanel->setDrawBackground(true);
    m_audioPanel->setBackgroundColor(sf::Color(40, 40, 60));
    m_audioPanel->setBorder(true);
    m_audioPanel->setBorderColor(sf::Color(80, 80, 100));

    m_videoPanel = std::make_unique<Panel>("videoPanel", sf::Vector2f(20, 340), sf::Vector2f(460, 100));
    m_videoPanel->setDrawBackground(true);
    m_videoPanel->setBackgroundColor(sf::Color(40, 40, 60));
    m_videoPanel->setBorder(true);
    m_videoPanel->setBorderColor(sf::Color(80, 80, 100));

    auto controlsLabel = std::make_unique<Label>("controlsLabel", "Controls", sf::Vector2f(20, 10));
    controlsLabel->setTextColor(sf::Color::White);
    controlsLabel->setCharacterSize(18);
    m_controlsPanel->addChild(std::move(controlsLabel));

    auto audioLabel = std::make_unique<Label>("audioLabel", "Audio", sf::Vector2f(20, 10));
    audioLabel->setTextColor(sf::Color::White);
    audioLabel->setCharacterSize(18);
    m_audioPanel->addChild(std::move(audioLabel));

    auto videoLabel = std::make_unique<Label>("videoLabel", "Video", sf::Vector2f(20, 10));
    videoLabel->setTextColor(sf::Color::White);
    videoLabel->setCharacterSize(18);
    m_videoPanel->addChild(std::move(videoLabel));

    m_resolutionDropdown = std::make_unique<Dropdown>("resolutionDropdown", sf::Vector2f(180, 40), sf::Vector2f(200, 30));
    m_resolutionDropdown->addItem("1920x1080 (Full HD)");
    m_resolutionDropdown->addItem("1280x720 (HD)");
    m_resolutionDropdown->addItem("800x600");
    m_videoPanel->addChild(std::move(m_resolutionDropdown));

    m_fullscreenCheckbox = std::make_unique<CheckBox>("fullscreenCheckbox", "Fullscreen", sf::Vector2f(180, 80), false);
    m_videoPanel->addChild(std::move(m_fullscreenCheckbox));

    m_musicVolumeSlider = std::make_unique<ProgressBar>("musicVolumeSlider", sf::Vector2f(180, 40), sf::Vector2f(200, 20), 0, 100, 80);
    m_musicVolumeSlider->setText("Music: 80%");
    m_audioPanel->addChild(std::move(m_musicVolumeSlider));

    m_sfxVolumeSlider = std::make_unique<ProgressBar>("sfxVolumeSlider", sf::Vector2f(180, 70), sf::Vector2f(200, 20), 0, 100, 100);
    m_sfxVolumeSlider->setText("SFX: 100%");
    m_audioPanel->addChild(std::move(m_sfxVolumeSlider));

    auto resolutionLabel = std::make_unique<Label>("resolutionLabel", "Resolution:", sf::Vector2f(20, 50));
    m_videoPanel->addChild(std::move(resolutionLabel));

    auto musicLabel = std::make_unique<Label>("musicLabel", "Music Volume:", sf::Vector2f(20, 40));
    m_audioPanel->addChild(std::move(musicLabel));

    auto sfxLabel = std::make_unique<Label>("sfxLabel", "SFX Volume:", sf::Vector2f(20, 70));
    m_audioPanel->addChild(std::move(sfxLabel));

    addOption("Back", [this]() {
        applySettings();
        close();
        });

    m_keybindings = {
        {"Move Left", "A"},
        {"Move Right", "D"},
        {"Jump", "Space"},
        {"Attack", "LMB"}
    };

    int yOffset = 40;
    for (const auto& [action, key] : m_keybindings) {
        auto actionLabel = std::make_unique<Label>(
            "action_" + action,
            action + ":",
            sf::Vector2f(20, yOffset)
        );

        auto keyLabel = std::make_unique<Label>(
            "key_" + action,
            key,
            sf::Vector2f(180, yOffset)
        );
        keyLabel->setTextColor(sf::Color(200, 200, 255));

        m_controlsPanel->addChild(std::move(actionLabel));
        m_controlsPanel->addChild(std::move(keyLabel));

        yOffset += 25;
    }

    addChild(std::move(m_controlsPanel));
    addChild(std::move(m_audioPanel));
    addChild(std::move(m_videoPanel));
}

void OptionsMenu::update(float dt) {
    Menu::update(dt);
}

void OptionsMenu::render(sf::RenderTarget& target) {
    Menu::render(target);
}

bool OptionsMenu::handleEvent(const sf::Event& event) {
    return Menu::handleEvent(event);
}

void OptionsMenu::loadCurrentSettings() {
}

void OptionsMenu::applySettings() {
    std::cout << "Applying settings..." << std::endl;
}

void OptionsMenu::resetToDefaults() {
}


GameOverMenu::GameOverMenu(int score)
    : Menu(MenuType::GameOverMenu, "Game Over")
{
    setSize(sf::Vector2f(400, 300));
    setBackgroundColor(sf::Color(20, 20, 40, 230));

    m_scoreLabel = std::make_unique<Label>("scoreLabel", "Score: " + std::to_string(score), sf::Vector2f(m_size.x / 2, 80));
    m_scoreLabel->setAnchor(UIAnchor::Top);
    addChild(std::move(m_scoreLabel));

    addOption("Retry", []() {
        std::cout << "Retry selected" << std::endl;
        });

    addOption("Main Menu", []() {
        std::cout << "Main Menu selected" << std::endl;
        });

    selectOption(0);
}

void GameOverMenu::setScore(int score) {
    if (m_scoreLabel) {
        m_scoreLabel->setText("Score: " + std::to_string(score));
    }
}


CreditsMenu::CreditsMenu()
    : Menu(MenuType::CreditsMenu, "Credits"),
    m_scrollSpeed(30.0f),
    m_scrollPosition(0.0f),
    m_autoScroll(true)
{
    setSize(sf::Vector2f(500, 500));
    setBackgroundColor(sf::Color(10, 10, 30, 230));

    m_creditsPanel = std::make_unique<Panel>("creditsPanel", sf::Vector2f(50, 80), sf::Vector2f(400, 360));
    m_creditsPanel->setDrawBackground(false);

    addCreditLine("Game Title", 32);
    addCreditLine("Version 1.0", 16);
    addCreditLine("", 16);
    addCreditLine("Created by", 20);
    addCreditLine("Your Name Here", 24, sf::Color(200, 200, 255));
    addCreditLine("", 16);
    addCreditLine("Programming", 20);
    addCreditLine("Programmer 1", 16);
    addCreditLine("Programmer 2", 16);
    addCreditLine("", 16);
    addCreditLine("Art", 20);
    addCreditLine("Artist 1", 16);
    addCreditLine("Artist 2", 16);
    addCreditLine("", 16);
    addCreditLine("Music", 20);
    addCreditLine("Composer Name", 16);
    addCreditLine("", 16);
    addCreditLine("Special Thanks", 20);
    addCreditLine("Person 1", 16);
    addCreditLine("Person 2", 16);
    addCreditLine("Person 3", 16);
    addCreditLine("", 16);
    addCreditLine("Thank You for Playing!", 24, sf::Color(255, 200, 100));

    addOption("Back", [this]() {
        close();
        });

    addChild(std::move(m_creditsPanel));
}

void CreditsMenu::update(float dt) {
    if (m_autoScroll) {
        m_scrollPosition -= m_scrollSpeed * dt;

        float currentY = m_scrollPosition;
        for (auto& label : m_creditLabels) {
            if (label) {
                label->setPosition(sf::Vector2f(200.0f, currentY));
                currentY += 40.0f;
            }
        }

        float totalHeight = m_creditLabels.size() * 40.0f;
        if (m_scrollPosition < -totalHeight) {
            m_scrollPosition = m_size.y;
        }
    }

    Menu::update(dt);
}

void CreditsMenu::render(sf::RenderTarget& target) {
    Menu::render(target);
}

bool CreditsMenu::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        m_scrollPosition += event.mouseWheelScroll.delta * 20.0f;

        float totalHeight = m_creditLabels.size() * 40.0f;
        float minY = m_size.y - totalHeight;

        if (m_scrollPosition < minY) {
            m_scrollPosition = minY;
        }
        else if (m_scrollPosition > m_size.y) {
            m_scrollPosition = m_size.y;
        }

        float currentY = m_scrollPosition;
        for (auto& label : m_creditLabels) {
            if (label) {
                label->setPosition(sf::Vector2f(200.0f, currentY));
                currentY += 40.0f;
            }
        }

        m_autoScroll = false;

        return true;
    }

    return Menu::handleEvent(event);
}

void CreditsMenu::setScrollSpeed(float speed) {
    m_scrollSpeed = speed;
}

void CreditsMenu::setAutoScroll(bool autoScroll) {
    m_autoScroll = autoScroll;
}

void CreditsMenu::addCreditLine(const std::string& text, unsigned int size, const sf::Color& color) {
    auto label = std::make_unique<Label>("credit_" + std::to_string(m_creditLabels.size()), text, sf::Vector2f(200.0f, m_size.y + m_creditLabels.size() * 40.0f));
    label->setCharacterSize(size);
    label->setTextColor(color);
    label->setAnchor(UIAnchor::Top);

    m_creditLabels.push_back(std::move(label));

    if (m_creditsPanel && !m_creditLabels.empty()) {
        m_creditsPanel->addChild(std::move(m_creditLabels.back()));
    }
}