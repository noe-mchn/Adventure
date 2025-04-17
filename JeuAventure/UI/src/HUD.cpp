#include "HUD.h"
#include <iostream>

HUD::HUD()
    : UIElement("HUD", sf::Vector2f(0, 0), sf::Vector2f(800, 600)),
    m_backgroundColor(sf::Color::Transparent),
    m_textColor(sf::Color::White),
    m_accentColor(sf::Color(255, 200, 50)),
    m_useFixedLayout(true),
    m_adaptToScreen(true)
{
    m_elementVisibility[HUDElement::HealthBar] = true;
    m_elementVisibility[HUDElement::ManaBar] = true;
    m_elementVisibility[HUDElement::ScoreDisplay] = true;
    m_elementVisibility[HUDElement::CoinDisplay] = true;
    m_elementVisibility[HUDElement::LivesDisplay] = true;
    m_elementVisibility[HUDElement::WeaponDisplay] = false;
    m_elementVisibility[HUDElement::AbilityDisplay] = false;
    m_elementVisibility[HUDElement::BossHealthBar] = false;
    m_elementVisibility[HUDElement::MiniMap] = false;
    m_elementVisibility[HUDElement::ObjectiveMarker] = false;
    m_elementVisibility[HUDElement::ComboCounter] = false;
    m_elementVisibility[HUDElement::Timer] = false;
}

void HUD::initialize() {
    m_healthBar = std::make_unique<ProgressBar>("healthBar", sf::Vector2f(20, 20), sf::Vector2f(200, 20), 0, 100, 100);
    m_healthBar->setFillColor(sf::Color(220, 50, 50));
    m_healthBar->setText("Health");

    m_manaBar = std::make_unique<ProgressBar>("manaBar", sf::Vector2f(20, 50), sf::Vector2f(200, 20), 0, 100, 100);
    m_manaBar->setFillColor(sf::Color(50, 50, 220));
    m_manaBar->setText("Mana");

    m_scoreDisplay = std::make_unique<Label>("scoreDisplay", "Score: 0", sf::Vector2f(m_size.x - 120, 20));
    m_scoreDisplay->setAnchor(UIAnchor::TopRight);

    m_coinDisplay = std::make_unique<Label>("coinDisplay", "Coins: 0", sf::Vector2f(m_size.x - 120, 50));
    m_coinDisplay->setAnchor(UIAnchor::TopRight);

    m_livesDisplay = std::make_unique<Label>("livesDisplay", "Lives: 3", sf::Vector2f(m_size.x - 120, 80));
    m_livesDisplay->setAnchor(UIAnchor::TopRight);

    m_abilityPanel = std::make_unique<Panel>("abilityPanel", sf::Vector2f(m_size.x / 2, m_size.y - 60), sf::Vector2f(300, 50));
    m_abilityPanel->setAnchor(UIAnchor::Bottom);
    m_abilityPanel->setDrawBackground(false);

    applyLayout();
}

void HUD::update(float dt) {
    UIElement::update(dt);
}

void HUD::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    UIElement::render(target);

    if (m_elementVisibility[HUDElement::HealthBar] && m_healthBar) {
        m_healthBar->render(target);
    }

    if (m_elementVisibility[HUDElement::ManaBar] && m_manaBar) {
        m_manaBar->render(target);
    }

    if (m_elementVisibility[HUDElement::ScoreDisplay] && m_scoreDisplay) {
        m_scoreDisplay->render(target);
    }

    if (m_elementVisibility[HUDElement::CoinDisplay] && m_coinDisplay) {
        m_coinDisplay->render(target);
    }

    if (m_elementVisibility[HUDElement::LivesDisplay] && m_livesDisplay) {
        m_livesDisplay->render(target);
    }

    if (m_elementVisibility[HUDElement::WeaponDisplay] && m_weaponDisplay) {
        m_weaponDisplay->render(target);
    }

    if (m_elementVisibility[HUDElement::AbilityDisplay] && m_abilityPanel) {
        m_abilityPanel->render(target);
    }

    if (m_elementVisibility[HUDElement::BossHealthBar] && m_bossHealthBar) {
        m_bossHealthBar->render(target);
    }

    if (m_elementVisibility[HUDElement::MiniMap] && m_miniMap) {
        m_miniMap->render(target);
    }

    if (m_elementVisibility[HUDElement::ObjectiveMarker] && m_objectiveMarker) {
        m_objectiveMarker->render(target);
    }

    if (m_elementVisibility[HUDElement::ComboCounter] && m_comboCounter) {
        m_comboCounter->render(target);
    }

    if (m_elementVisibility[HUDElement::Timer] && m_timer) {
        m_timer->render(target);
    }

    for (const auto& element : m_customElements) {
        if (element.second && element.second->isVisible()) {
            element.second->render(target);
        }
    }
}

bool HUD::handleEvent(const sf::Event& event) {
    if (UIElement::handleEvent(event)) {
        return true;
    }

    for (const auto& element : m_customElements) {
        if (element.second && element.second->isActive() && element.second->isInteractive() &&
            element.second->handleEvent(event)) {
            return true;
        }
    }

    return false;
}

void HUD::configureHealth(float max, float current) {
    if (m_healthBar) {
        m_healthBar->setMaxValue(max);
        m_healthBar->setCurrentValue(current);
    }
}

void HUD::configureMana(float max, float current) {
    if (m_manaBar) {
        m_manaBar->setMaxValue(max);
        m_manaBar->setCurrentValue(current);
    }
}

void HUD::configureScore(int score) {
    if (m_scoreDisplay) {
        m_scoreDisplay->setText("Score: " + std::to_string(score));
    }
}

void HUD::configureCoins(int coins) {
    if (m_coinDisplay) {
        m_coinDisplay->setText("Coins: " + std::to_string(coins));
    }
}

void HUD::configureLives(int lives) {
    if (m_livesDisplay) {
        m_livesDisplay->setText("Lives: " + std::to_string(lives));
    }
}

void HUD::configureWeapon(sf::Texture* weaponTexture, const std::string& weaponName) {
    if (!m_weaponDisplay) {
        m_weaponDisplay = std::make_unique<Image>("weaponDisplay", weaponTexture, sf::Vector2f(m_size.x - 70, m_size.y - 70));
        m_weaponDisplay->setAnchor(UIAnchor::BottomRight);
        m_weaponDisplay->setSize(sf::Vector2f(50, 50));
    }
    else {
        m_weaponDisplay->setTexture(weaponTexture);
    }

    applyLayout();
}

void HUD::configureAbilities(const std::vector<sf::Texture*>& abilityTextures) {
    if (!m_abilityPanel) {
        m_abilityPanel = std::make_unique<Panel>("abilityPanel", sf::Vector2f(m_size.x / 2, m_size.y - 60), sf::Vector2f(300, 50));
        m_abilityPanel->setAnchor(UIAnchor::Bottom);
        m_abilityPanel->setDrawBackground(false);
    }

    m_abilityPanel->clearChildren();

    int abilityCount = abilityTextures.size();
    float abilitySize = 40.0f;
    float spacing = 10.0f;
    float totalWidth = abilityCount * abilitySize + (abilityCount - 1) * spacing;

    float startX = -totalWidth / 2.0f + abilitySize / 2.0f;

    for (int i = 0; i < abilityCount; ++i) {
        float x = startX + i * (abilitySize + spacing);
        auto abilityIcon = std::make_unique<Image>("ability" + std::to_string(i),
            abilityTextures[i], sf::Vector2f(x, 0));
        abilityIcon->setSize(sf::Vector2f(abilitySize, abilitySize));
        abilityIcon->setAnchor(UIAnchor::Center);
        m_abilityPanel->addChild(std::move(abilityIcon));
    }

    applyLayout();
}

void HUD::configureBossHealth(float max, float current, const std::string& bossName) {
    if (!m_bossHealthBar) {
        m_bossHealthBar = std::make_unique<ProgressBar>("bossHealthBar",
            sf::Vector2f(m_size.x / 2, 40), sf::Vector2f(400, 25), 0, max, current);
        m_bossHealthBar->setAnchor(UIAnchor::Top);
        m_bossHealthBar->setFillColor(sf::Color(200, 50, 50));
        m_bossHealthBar->setText(bossName);
    }
    else {
        m_bossHealthBar->setMaxValue(max);
        m_bossHealthBar->setCurrentValue(current);
        m_bossHealthBar->setText(bossName);
    }

    applyLayout();
}

void HUD::configureMiniMap(const sf::Texture* mapTexture) {
    if (!m_miniMap) {
        m_miniMap = std::make_unique<Panel>("miniMap", sf::Vector2f(m_size.x - 120, m_size.y - 120), sf::Vector2f(100, 100));
        m_miniMap->setAnchor(UIAnchor::BottomRight);
    }
    
    if (mapTexture) {
        auto mapImage = std::make_unique<Image>("minimapImage",
            const_cast<sf::Texture*>(mapTexture), sf::Vector2f(0, 0));
        mapImage->setSize(sf::Vector2f(100, 100));
        mapImage->setAnchor(UIAnchor::Center);
        m_miniMap->clearChildren();
        m_miniMap->addChild(std::move(mapImage));
    }

    applyLayout();
}

void HUD::configureObjectiveMarker(sf::Vector2f worldPosition) {
    if (!m_objectiveMarker) {
        m_objectiveMarker = std::make_unique<Image>("objectiveMarker", nullptr, sf::Vector2f(0, 0));
        m_objectiveMarker->setSize(sf::Vector2f(32, 32));
    }

    m_objectiveMarker->setPosition(sf::Vector2f(m_size.x / 2, m_size.y / 2));

    applyLayout();
}

void HUD::configureComboCounter(int combo) {
    if (!m_comboCounter) {
        m_comboCounter = std::make_unique<Label>("comboCounter", "Combo: 0", sf::Vector2f(m_size.x / 2, 80));
        m_comboCounter->setAnchor(UIAnchor::Top);
    }

    m_comboCounter->setText("Combo: " + std::to_string(combo));

    applyLayout();
}

void HUD::configureTimer(float seconds) {
    if (!m_timer) {
        m_timer = std::make_unique<Label>("timer", "Time: 0:00", sf::Vector2f(m_size.x / 2, 20));
        m_timer->setAnchor(UIAnchor::Top);
    }

    int minutes = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;

    std::string timeStr = "Time: " + std::to_string(minutes) + ":" +
        (secs < 10 ? "0" : "") + std::to_string(secs);

    m_timer->setText(timeStr);

    applyLayout();
}

void HUD::setHealth(float current) {
    if (m_healthBar) {
        m_healthBar->setCurrentValue(current);
    }
}

void HUD::setMana(float current) {
    if (m_manaBar) {
        m_manaBar->setCurrentValue(current);
    }
}

void HUD::setScore(int score) {
    if (m_scoreDisplay) {
        m_scoreDisplay->setText("Score: " + std::to_string(score));
    }
}

void HUD::setCoins(int coins) {
    if (m_coinDisplay) {
        m_coinDisplay->setText("Coins: " + std::to_string(coins));
    }
}

void HUD::setLives(int lives) {
    if (m_livesDisplay) {
        m_livesDisplay->setText("Lives: " + std::to_string(lives));
    }
}

void HUD::setWeapon(sf::Texture* weaponTexture, const std::string& weaponName) {
    if (m_weaponDisplay) {
        m_weaponDisplay->setTexture(weaponTexture);
    }
    else {
        configureWeapon(weaponTexture, weaponName);
    }
}

void HUD::setBossHealth(float current) {
    if (m_bossHealthBar) {
        m_bossHealthBar->setCurrentValue(current);
    }
}

void HUD::setCombo(int combo) {
    if (m_comboCounter) {
        m_comboCounter->setText("Combo: " + std::to_string(combo));
    }
    else {
        configureComboCounter(combo);
    }
}

void HUD::setTimer(float seconds) {
    if (m_timer) {
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;

        std::string timeStr = "Time: " + std::to_string(minutes) + ":" +
            (secs < 10 ? "0" : "") + std::to_string(secs);

        m_timer->setText(timeStr);
    }
    else {
        configureTimer(seconds);
    }
}

UIElement* HUD::addCustomElement(const std::string& id, std::unique_ptr<UIElement> element) {
    if (!element) return nullptr;

    UIElement* elementPtr = element.get();
    m_customElements[id] = std::move(element);

    applyLayout();
    return elementPtr;
}

void HUD::removeCustomElement(const std::string& id) {
    auto it = m_customElements.find(id);
    if (it != m_customElements.end()) {
        m_customElements.erase(it);
    }
}

UIElement* HUD::getCustomElement(const std::string& id) {
    auto it = m_customElements.find(id);
    if (it != m_customElements.end()) {
        return it->second.get();
    }
    return nullptr;
}

void HUD::showElement(HUDElement element, bool show) {
    m_elementVisibility[element] = show;
}

bool HUD::isElementVisible(HUDElement element) const {
    auto it = m_elementVisibility.find(element);
    if (it != m_elementVisibility.end()) {
        return it->second;
    }
    return false;
}

void HUD::setFont(const sf::Font& font) {
    if (m_healthBar) m_healthBar->setFont(font);
    if (m_manaBar) m_manaBar->setFont(font);
    if (m_scoreDisplay) m_scoreDisplay->setFont(font);
    if (m_coinDisplay) m_coinDisplay->setFont(font);
    if (m_livesDisplay) m_livesDisplay->setFont(font);
    if (m_bossHealthBar) m_bossHealthBar->setFont(font);
    if (m_comboCounter) m_comboCounter->setFont(font);
    if (m_timer) m_timer->setFont(font);
}

void HUD::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
}

void HUD::setTextColor(const sf::Color& color) {
    m_textColor = color;

    if (m_scoreDisplay) m_scoreDisplay->setTextColor(color);
    if (m_coinDisplay) m_coinDisplay->setTextColor(color);
    if (m_livesDisplay) m_livesDisplay->setTextColor(color);
    if (m_comboCounter) m_comboCounter->setTextColor(color);
    if (m_timer) m_timer->setTextColor(color);
}

void HUD::setAccentColor(const sf::Color& color) {
    m_accentColor = color;

    if (m_healthBar) m_healthBar->setFillColor(sf::Color(color.r, 50, 50));
    if (m_manaBar) m_manaBar->setFillColor(sf::Color(50, 50, color.b));
    if (m_bossHealthBar) m_bossHealthBar->setFillColor(sf::Color(color.r, 50, 50));
}

void HUD::setFixedLayout(bool fixed) {
    m_useFixedLayout = fixed;
    applyLayout();
}

bool HUD::usesFixedLayout() const {
    return m_useFixedLayout;
}

void HUD::setAdaptToScreen(bool adapt) {
    m_adaptToScreen = adapt;
    applyLayout();
}

bool HUD::adaptsToScreen() const {
    return m_adaptToScreen;
}

void HUD::applyLayout() {
    if (m_useFixedLayout) {
        if (m_healthBar) m_healthBar->setPosition(sf::Vector2f(20, 20));
        if (m_manaBar) m_manaBar->setPosition(sf::Vector2f(20, 50));

        if (m_scoreDisplay) {
            m_scoreDisplay->setPosition(sf::Vector2f(m_size.x - 120, 20));
            m_scoreDisplay->setAnchor(UIAnchor::TopRight);
        }
        if (m_coinDisplay) {
            m_coinDisplay->setPosition(sf::Vector2f(m_size.x - 120, 50));
            m_coinDisplay->setAnchor(UIAnchor::TopRight);
        }
        if (m_livesDisplay) {
            m_livesDisplay->setPosition(sf::Vector2f(m_size.x - 120, 80));
            m_livesDisplay->setAnchor(UIAnchor::TopRight);
        }

        if (m_bossHealthBar) {
            m_bossHealthBar->setPosition(sf::Vector2f(m_size.x / 2, 40));
            m_bossHealthBar->setAnchor(UIAnchor::Top);
        }

        if (m_comboCounter) {
            m_comboCounter->setPosition(sf::Vector2f(m_size.x / 2, 80));
            m_comboCounter->setAnchor(UIAnchor::Top);
        }

        if (m_timer) {
            m_timer->setPosition(sf::Vector2f(m_size.x / 2, 20));
            m_timer->setAnchor(UIAnchor::Top);
        }

        if (m_weaponDisplay) {
            m_weaponDisplay->setPosition(sf::Vector2f(m_size.x - 70, m_size.y - 70));
            m_weaponDisplay->setAnchor(UIAnchor::BottomRight);
        }

        if (m_abilityPanel) {
            m_abilityPanel->setPosition(sf::Vector2f(m_size.x / 2, m_size.y - 60));
            m_abilityPanel->setAnchor(UIAnchor::Bottom);
        }

        if (m_miniMap) {
            m_miniMap->setPosition(sf::Vector2f(m_size.x - 120, m_size.y - 120));
            m_miniMap->setAnchor(UIAnchor::BottomRight);
        }
    }
    else if (m_adaptToScreen) {
        float margin = std::min(m_size.x, m_size.y) * 0.02f;

        if (m_healthBar) {
            m_healthBar->setPosition(sf::Vector2f(margin, margin));
            m_healthBar->setSize(sf::Vector2f(m_size.x * 0.25f, m_size.y * 0.03f));
        }
        if (m_manaBar) {
            m_manaBar->setPosition(sf::Vector2f(margin, margin * 2 + m_size.y * 0.03f));
            m_manaBar->setSize(sf::Vector2f(m_size.x * 0.25f, m_size.y * 0.03f));
        }

        float rightAlign = m_size.x - margin;
        if (m_scoreDisplay) {
            m_scoreDisplay->setPosition(sf::Vector2f(rightAlign, margin));
            m_scoreDisplay->setAnchor(UIAnchor::TopRight);
        }
        if (m_coinDisplay) {
            m_coinDisplay->setPosition(sf::Vector2f(rightAlign, margin * 2 + m_size.y * 0.03f));
            m_coinDisplay->setAnchor(UIAnchor::TopRight);
        }
        if (m_livesDisplay) {
            m_livesDisplay->setPosition(sf::Vector2f(rightAlign, margin * 3 + m_size.y * 0.06f));
            m_livesDisplay->setAnchor(UIAnchor::TopRight);
        }
    }

}