#pragma once

#include "UIElements.h"
#include "Widget.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <string>

enum class HUDElement {
    HealthBar,
    ManaBar,
    ScoreDisplay,
    CoinDisplay,
    LivesDisplay,
    WeaponDisplay,
    AbilityDisplay,
    BossHealthBar,
    MiniMap,
    ObjectiveMarker,
    ComboCounter,
    Timer,
    Custom
};

class HUD : public UIElements {
private:
    std::unique_ptr<ProgressBar> m_healthBar;
    std::unique_ptr<ProgressBar> m_manaBar;
    std::unique_ptr<Label> m_scoreDisplay;
    std::unique_ptr<Label> m_coinDisplay;
    std::unique_ptr<Label> m_livesDisplay;
    std::unique_ptr<Image> m_weaponDisplay;
    std::unique_ptr<Panel> m_abilityPanel;

    std::unique_ptr<ProgressBar> m_bossHealthBar;
    std::unique_ptr<Panel> m_miniMap;
    std::unique_ptr<Image> m_objectiveMarker;
    std::unique_ptr<Label> m_comboCounter;
    std::unique_ptr<Label> m_timer;

    std::map<std::string, std::unique_ptr<UIElements>> m_customElements;

    std::map<HUDElement, bool> m_elementVisibility;

    sf::Font m_font;
    sf::Color m_backgroundColor;
    sf::Color m_textColor;
    sf::Color m_accentColor;

    bool m_useFixedLayout;
    bool m_adaptToScreen;

public:
    HUD();

    void initialize();
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void configureHealth(float max, float current);
    void configureMana(float max, float current);
    void configureScore(int score);
    void configureCoins(int coins);
    void configureLives(int lives);
    void configureWeapon(sf::Texture* weaponTexture, const std::string& weaponName);
    void configureAbilities(const std::vector<sf::Texture*>& abilityTextures);

    void configureBossHealth(float max, float current, const std::string& bossName);
    void configureMiniMap(const sf::Texture* mapTexture);
    void configureObjectiveMarker(sf::Vector2f worldPosition);
    void configureComboCounter(int combo);
    void configureTimer(float seconds);

    void setHealth(float current);
    void setMana(float current);
    void setScore(int score);
    void setCoins(int coins);
    void setLives(int lives);
    void setWeapon(sf::Texture* weaponTexture, const std::string& weaponName);
    void setBossHealth(float current);
    void setCombo(int combo);
    void setTimer(float seconds);

    UIElements* addCustomElement(const std::string& id, std::unique_ptr<UIElements> element);
    void removeCustomElement(const std::string& id);
    UIElements* getCustomElement(const std::string& id);

    void showElement(HUDElement element, bool show);
    bool isElementVisible(HUDElement element) const;

    void setFont(const sf::Font& font);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setAccentColor(const sf::Color& color);

    void setFixedLayout(bool fixed);
    bool usesFixedLayout() const;

    void setAdaptToScreen(bool adapt);
    bool adaptsToScreen() const;

    void applyLayout();
};
