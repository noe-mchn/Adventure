#include "DialogSystem.h"
#include <iostream>

DialogSystem::DialogSystem()
    : UIElement("DialogSystem", sf::Vector2f(0, 0), sf::Vector2f(800, 600)),
    m_activeDialog(nullptr),
    m_animatingText(false),
    m_textSpeed(30.0f),
    m_currentCharIndex(0),
    m_fullText(""),
    m_boxColor(sf::Color(20, 20, 40, 230)),
    m_textColor(sf::Color::White),
    m_nameColor(sf::Color(255, 200, 100)),
    m_selectedResponseColor(sf::Color(100, 200, 255)),
    m_isActive(false),
    m_waitingForResponse(false),
    m_canSkip(true),
    m_selectedResponse(0)
{
    m_dialogBox.setSize(sf::Vector2f(700, 150));
    m_dialogBox.setFillColor(m_boxColor);
    m_dialogBox.setOutlineThickness(2.0f);
    m_dialogBox.setOutlineColor(sf::Color(100, 100, 150));

    m_characterNameText.setCharacterSize(18);
    m_characterNameText.setFillColor(m_nameColor);

    m_dialogText.setCharacterSize(16);
    m_dialogText.setFillColor(m_textColor);
}

void DialogSystem::update(float dt) {
    if (!m_isActive) return;

    if (m_animatingText) {
        updateTextAnimation(dt);
    }

    UIElement::update(dt);
}

void DialogSystem::render(sf::RenderTarget& target) {
    if (!m_isActive || !isVisible()) return;

    sf::Vector2u windowSize = target.getSize();

    m_dialogBox.setPosition(
        (windowSize.x - m_dialogBox.getSize().x) / 2.0f,
        windowSize.y - m_dialogBox.getSize().y - 20.0f
    );

    target.draw(m_dialogBox);

    if (!m_characterNameText.getString().isEmpty() && m_characterNameText.getFont()) {
        m_characterNameText.setPosition(
            m_dialogBox.getPosition().x + 20.0f,
            m_dialogBox.getPosition().y - m_characterNameText.getCharacterSize() - 5.0f
        );
        target.draw(m_characterNameText);
    }

    if (m_dialogText.getFont()) {
        m_dialogText.setPosition(
            m_dialogBox.getPosition().x + 20.0f,
            m_dialogBox.getPosition().y + 20.0f
        );
        target.draw(m_dialogText);
    }

    if (m_activeDialog && !m_activeDialog->lines.empty()) {
        const DialogLine& currentLine = m_activeDialog->lines[m_activeDialog->currentLine];
        auto portraitIt = m_portraits.find(currentLine.characterName);

        if (portraitIt != m_portraits.end()) {
            m_characterPortrait.setTexture(portraitIt->second);
            m_characterPortrait.setPosition(
                m_dialogBox.getPosition().x - 100.0f,
                m_dialogBox.getPosition().y - 50.0f
            );
            target.draw(m_characterPortrait);
        }
    }

    if (m_waitingForResponse && !m_responseTexts.empty()) {
        float responseY = m_dialogBox.getPosition().y + m_dialogBox.getSize().y + 10.0f;

        for (size_t i = 0; i < m_responseTexts.size(); ++i) {
            m_responseTexts[i].setPosition(
                m_dialogBox.getPosition().x + 30.0f,
                responseY
            );

            if (i == m_selectedResponse) {
                m_responseTexts[i].setFillColor(m_selectedResponseColor);

                sf::ConvexShape arrow;
                arrow.setPointCount(3);
                arrow.setPoint(0, sf::Vector2f(0, 0));
                arrow.setPoint(1, sf::Vector2f(10, 8));
                arrow.setPoint(2, sf::Vector2f(0, 16));
                arrow.setFillColor(m_selectedResponseColor);
                arrow.setPosition(
                    m_dialogBox.getPosition().x + 10.0f,
                    responseY + 2.0f
                );
                target.draw(arrow);
            }
            else {
                m_responseTexts[i].setFillColor(m_textColor);
            }

            target.draw(m_responseTexts[i]);
            responseY += 25.0f;
        }
    }

    UIElement::render(target);
}

bool DialogSystem::handleEvent(const sf::Event& event) {
    if (!m_isActive || !isInteractive()) return false;

    if (event.type == sf::Event::KeyPressed) {
        if (m_waitingForResponse) {
            if (event.key.code == sf::Keyboard::Up) {
                if (m_selectedResponse > 0) {
                    m_selectedResponse--;
                }
                else {
                    m_selectedResponse = m_responseTexts.size() - 1;
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Down) {
                m_selectedResponse = (m_selectedResponse + 1) % m_responseTexts.size();
                return true;
            }
            else if (event.key.code == sf::Keyboard::Return) {
                executeSelectedResponse();
                return true;
            }
        }
        else if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Return) {
            if (m_animatingText && m_canSkip) {
                completeTextAnimation();
            }
            else {
                advanceDialog();
            }
            return true;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (m_waitingForResponse) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            float responseY = m_dialogBox.getPosition().y + m_dialogBox.getSize().y + 10.0f;
            for (size_t i = 0; i < m_responseTexts.size(); ++i) {
                sf::FloatRect responseBounds = m_responseTexts[i].getGlobalBounds();
                if (responseBounds.contains(mousePos)) {
                    m_selectedResponse = i;
                    executeSelectedResponse();
                    return true;
                }
                responseY += 25.0f;
            }
        }
        else {
            if (m_animatingText && m_canSkip) {
                completeTextAnimation();
            }
            else {
                advanceDialog();
            }
            return true;
        }
    }

    return UIElement::handleEvent(event);
}

void DialogSystem::loadDialog(const std::string& id, const std::vector<DialogLine>& lines) {
    Dialog dialog;
    dialog.id = id;
    dialog.lines = lines;
    dialog.currentLine = 0;
    dialog.complete = false;
    dialog.onComplete = nullptr;

    m_dialogs[id] = dialog;
}

void DialogSystem::startDialog(const std::string& id) {
    auto it = m_dialogs.find(id);
    if (it == m_dialogs.end()) {
        std::cerr << "Dialog not found: " << id << std::endl;
        return;
    }

    m_activeDialog = &it->second;
    m_activeDialog->currentLine = 0;
    m_activeDialog->complete = false;

    m_isActive = true;
    m_waitingForResponse = false;

    if (!m_activeDialog->lines.empty()) {
        updateDialogDisplay();
    }
    else {
        endDialog();
    }
}

void DialogSystem::endDialog() {
    if (m_activeDialog && m_activeDialog->onComplete) {
        m_activeDialog->onComplete();
    }

    m_activeDialog = nullptr;
    m_isActive = false;
    m_waitingForResponse = false;
    m_animatingText = false;
}

void DialogSystem::advanceDialog() {
    if (!m_activeDialog || m_waitingForResponse) return;

    if (m_activeDialog->currentLine < m_activeDialog->lines.size()) {
        const DialogLine& currentLine = m_activeDialog->lines[m_activeDialog->currentLine];
        if (currentLine.onFinish) {
            currentLine.onFinish();
        }
    }

    m_activeDialog->currentLine++;

    if (m_activeDialog->currentLine >= m_activeDialog->lines.size()) {
        m_activeDialog->complete = true;
        endDialog();
        return;
    }

    updateDialogDisplay();
}

void DialogSystem::selectResponse(int index) {
    if (m_waitingForResponse && index >= 0 && index < static_cast<int>(m_responseTexts.size())) {
        m_selectedResponse = index;
    }
}

void DialogSystem::executeSelectedResponse() {
    if (!m_waitingForResponse || m_selectedResponse < 0 ||
        m_selectedResponse >= static_cast<int>(m_activeDialog->lines[m_activeDialog->currentLine].responses.size())) {
        return;
    }

    const auto& response = m_activeDialog->lines[m_activeDialog->currentLine].responses[m_selectedResponse];

    if (m_onResponseSelected) {
        m_onResponseSelected(response.second);
    }

    m_waitingForResponse = false;

    advanceDialog();
}

void DialogSystem::setTextSpeed(float speed) {
    m_textSpeed = speed;
}

float DialogSystem::getTextSpeed() const {
    return m_textSpeed;
}

void DialogSystem::setCanSkip(bool canSkip) {
    m_canSkip = canSkip;
}

bool DialogSystem::canSkip() const {
    return m_canSkip;
}

void DialogSystem::setFont(const sf::Font& font) {
    m_characterNameText.setFont(font);
    m_dialogText.setFont(font);

    for (auto& text : m_responseTexts) {
        text.setFont(font);
    }
}

void DialogSystem::setBoxColor(const sf::Color& color) {
    m_boxColor = color;
    m_dialogBox.setFillColor(color);
}

void DialogSystem::setTextColor(const sf::Color& color) {
    m_textColor = color;
    m_dialogText.setFillColor(color);

    for (auto& text : m_responseTexts) {
        if (text.getFillColor() != m_selectedResponseColor) {
            text.setFillColor(color);
        }
    }
}

void DialogSystem::setNameColor(const sf::Color& color) {
    m_nameColor = color;
    m_characterNameText.setFillColor(color);
}

void DialogSystem::setSelectedResponseColor(const sf::Color& color) {
    m_selectedResponseColor = color;

    if (m_waitingForResponse && m_selectedResponse >= 0 &&
        m_selectedResponse < static_cast<int>(m_responseTexts.size())) {
        m_responseTexts[m_selectedResponse].setFillColor(color);
    }
}

void DialogSystem::setTypingSound(const std::string& soundPath) {
    m_typingSound = soundPath;
}

void DialogSystem::setSelectSound(const std::string& soundPath) {
    m_selectSound = soundPath;
}

void DialogSystem::setConfirmSound(const std::string& soundPath) {
    m_confirmSound = soundPath;
}

void DialogSystem::loadPortrait(const std::string& characterName, const std::string& texturePath) {
    sf::Texture texture;
    if (texture.loadFromFile(texturePath)) {
        m_portraits[characterName] = texture;
    }
    else {
        std::cerr << "Failed to load portrait texture: " << texturePath << std::endl;
    }
}

void DialogSystem::setOnResponseSelectedCallback(const std::function<void(const std::string&)>& callback) {
    m_onResponseSelected = callback;
}

void DialogSystem::clearDialogs() {
    m_dialogs.clear();
    if (m_isActive) {
        endDialog();
    }
}

void DialogSystem::skipCurrentLine() {
    if (m_animatingText) {
        completeTextAnimation();
    }
}

void DialogSystem::updateDialogDisplay() {
    if (!m_activeDialog || m_activeDialog->currentLine >= m_activeDialog->lines.size()) {
        return;
    }

    const DialogLine& currentLine = m_activeDialog->lines[m_activeDialog->currentLine];

    m_characterNameText.setString(currentLine.characterName);

    m_fullText = currentLine.text;
    m_currentCharIndex = 0;
    m_dialogText.setString("");
    m_animatingText = true;
    m_textTimer = 0.0f;

    m_waitingForResponse = !currentLine.responses.empty();

    m_responseTexts.clear();
    if (m_waitingForResponse) {
        for (const auto& response : currentLine.responses) {
            sf::Text responseText;
            responseText.setString(response.first);
            responseText.setCharacterSize(16);
            responseText.setFillColor(m_textColor);

            if (m_dialogText.getFont()) {
                responseText.setFont(*m_dialogText.getFont());
            }

            m_responseTexts.push_back(responseText);
        }

        m_selectedResponse = 0;
    }
}

void DialogSystem::updateTextAnimation(float dt) {
    if (!m_animatingText) return;

    m_textTimer += dt;
    int targetCharCount = static_cast<int>(m_textTimer * m_textSpeed);

    if (targetCharCount > m_currentCharIndex) {
        int charsToAdd = targetCharCount - m_currentCharIndex;

        for (int i = 0; i < charsToAdd; ++i) {
            if (m_currentCharIndex < static_cast<int>(m_fullText.length())) {
                m_currentCharIndex++;
                m_dialogText.setString(m_fullText.substr(0, m_currentCharIndex));
            }
            else {
                completeTextAnimation();
                break;
            }
        }
    }
}

void DialogSystem::completeTextAnimation() {
    m_animatingText = false;
    m_dialogText.setString(m_fullText);
    m_currentCharIndex = m_fullText.length();
}

bool DialogSystem::isDialogActive() const {
    return m_isActive;
}

bool DialogSystem::isWaitingForResponse() const {
    return m_waitingForResponse;
}