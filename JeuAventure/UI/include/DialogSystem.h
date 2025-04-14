#pragma once

#include "UIElements.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <memory>

struct DialogLine {
    std::string characterName;
    std::string text;
    std::string portraitPath;
    std::vector<std::pair<std::string, std::string>> responses;
    std::function<void()> onFinish;
};

struct Dialog {
    std::string id;
    std::vector<DialogLine> lines;
    int currentLine;
    bool complete;
    std::function<void()> onComplete;
};

class DialogSystem : public UIElements {
private:
    std::map<std::string, Dialog> m_dialogs;
    Dialog* m_activeDialog;
    sf::RectangleShape m_dialogBox;
    sf::Text m_characterNameText;
    sf::Text m_dialogText;
    sf::Sprite m_characterPortrait;
    std::map<std::string, sf::Texture> m_portraits;
    std::vector<sf::Text> m_responseTexts;
    int m_selectedResponse;
    bool m_animatingText;
    float m_textSpeed;
    float m_textTimer;
    int m_currentCharIndex;
    std::string m_fullText;
    sf::Font m_font;
    sf::Color m_boxColor;
    sf::Color m_textColor;
    sf::Color m_nameColor;
    sf::Color m_selectedResponseColor;
    std::string m_typingSound;
    std::string m_selectSound;
    std::string m_confirmSound;
    bool m_isActive;
    bool m_waitingForResponse;
    bool m_canSkip;
    std::function<void(const std::string&)> m_onResponseSelected;

    void updateDialogDisplay();
    void updateTextAnimation(float dt);
    void completeTextAnimation();

public:
    DialogSystem();

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void loadDialog(const std::string& id, const std::vector<DialogLine>& lines);
    void startDialog(const std::string& id);
    void endDialog();

    bool isDialogActive() const;
    bool isWaitingForResponse() const;

    void advanceDialog();
    void selectResponse(int index);
    void executeSelectedResponse();

    void setTextSpeed(float speed);
    float getTextSpeed() const;
    void setCanSkip(bool canSkip);
    bool canSkip() const;

    void setFont(const sf::Font& font);
    void setBoxColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setNameColor(const sf::Color& color);
    void setSelectedResponseColor(const sf::Color& color);

    void setTypingSound(const std::string& soundPath);
    void setSelectSound(const std::string& soundPath);
    void setConfirmSound(const std::string& soundPath);

    void loadPortrait(const std::string& characterName, const std::string& texturePath);

    void setOnResponseSelectedCallback(const std::function<void(const std::string&)>& callback);

    void clearDialogs();
    void skipCurrentLine();
};