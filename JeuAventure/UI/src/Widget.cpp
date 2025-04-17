#include "Widget.h"
#include <iostream>
#include <cmath>

// ----------------- Button Implementation -----------------

Button::Button()
    : UIElement(),
    m_texture(nullptr),
    m_normalColor(sf::Color(100, 100, 100)),
    m_hoverColor(sf::Color(120, 120, 120)),
    m_pressedColor(sf::Color(80, 80, 80)),
    m_disabledColor(sf::Color(70, 70, 70, 200)),
    m_isPressed(false),
    m_isHovered(false)
{
    m_shape.setSize(m_size);
    m_shape.setFillColor(m_normalColor);

    m_text.setFillColor(sf::Color::White);
    m_text.setCharacterSize(16);
}

Button::Button(const std::string& id, const std::string& text, const sf::Vector2f& position, const sf::Vector2f& size)
    : UIElement(id, position, size),
    m_texture(nullptr),
    m_normalColor(sf::Color(100, 100, 100)),
    m_hoverColor(sf::Color(120, 120, 120)),
    m_pressedColor(sf::Color(80, 80, 80)),
    m_disabledColor(sf::Color(70, 70, 70, 200)),
    m_isPressed(false),
    m_isHovered(false)
{
    m_shape.setSize(size);
    m_shape.setPosition(position);
    m_shape.setFillColor(m_normalColor);

    m_text.setString(text);
    m_text.setFillColor(sf::Color::White);
    m_text.setCharacterSize(16);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void Button::update(float dt) {
    m_shape.setPosition(getGlobalPosition());

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(getGlobalPosition() + sf::Vector2f(m_size.x / 2.0f, m_size.y / 2.0f));

    if (!isActive()) {
        m_shape.setFillColor(m_disabledColor);
    }
    else if (m_isPressed) {
        m_shape.setFillColor(m_pressedColor);
    }
    else if (m_isHovered) {
        m_shape.setFillColor(m_hoverColor);
    }
    else {
        m_shape.setFillColor(m_normalColor);
    }

    m_shape.setSize(m_size);

    UIElement::update(dt);
}

void Button::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    if (m_texture) {
        sf::Sprite sprite(*m_texture);
        sprite.setPosition(getGlobalPosition());
        sprite.setScale(m_size.x / m_texture->getSize().x, m_size.y / m_texture->getSize().y);
        sprite.setColor(m_shape.getFillColor());
        target.draw(sprite);
    }
    else {
        target.draw(m_shape);
    }

    target.draw(m_text);

    UIElement::render(target);
}

bool Button::handleEvent(const sf::Event& event) {
    if (!isActive() || !isInteractive()) return false;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (containsPoint(mousePos)) {
            m_isPressed = true;
            return true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (m_isPressed && containsPoint(mousePos)) {
            m_isPressed = false;

            if (m_onClick) {
                m_onClick();
            }
            click();

            return true;
        }

        m_isPressed = false;
    }
    else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));

        bool wasHovered = m_isHovered;
        m_isHovered = containsPoint(mousePos);

        if (wasHovered != m_isHovered) {
            hover(m_isHovered);
        }

        if (m_isHovered) {
            return true;
        }
    }

    return false;
}

void Button::setText(const std::string& text) {
    m_text.setString(text);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
}

const std::string& Button::getText() const {
    return m_text.getString();
}

void Button::setFont(const sf::Font& font) {
    m_text.setFont(font);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
}

void Button::setTextColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

sf::Color Button::getTextColor() const {
    return m_text.getFillColor();
}

void Button::setTexture(sf::Texture* texture) {
    m_texture = texture;
}

sf::Texture* Button::getTexture() const {
    return m_texture;
}

void Button::setNormalColor(const sf::Color& color) {
    m_normalColor = color;
    if (!m_isHovered && !m_isPressed && isActive()) {
        m_shape.setFillColor(color);
    }
}

void Button::setHoverColor(const sf::Color& color) {
    m_hoverColor = color;
    if (m_isHovered && !m_isPressed && isActive()) {
        m_shape.setFillColor(color);
    }
}

void Button::setPressedColor(const sf::Color& color) {
    m_pressedColor = color;
    if (m_isPressed && isActive()) {
        m_shape.setFillColor(color);
    }
}

void Button::setDisabledColor(const sf::Color& color) {
    m_disabledColor = color;
    if (!isActive()) {
        m_shape.setFillColor(color);
    }
}

void Button::setOnClickCallback(const std::function<void()>& callback) {
    m_onClick = callback;
}

bool Button::isPressed() const {
    return m_isPressed;
}

bool Button::isHovered() const {
    return m_isHovered;
}

// ----------------- Label Implementation -----------------

Label::Label()
    : UIElement(),
    m_autoResize(true)
{
    m_text.setFillColor(sf::Color::White);
    m_text.setCharacterSize(16);
}

Label::Label(const std::string& id, const std::string& text, const sf::Vector2f& position)
    : UIElement(id, position, sf::Vector2f(0, 0)),
    m_autoResize(true)
{
    m_text.setString(text);
    m_text.setFillColor(sf::Color::White);
    m_text.setCharacterSize(16);
    m_text.setPosition(position);

    if (m_autoResize && m_text.getFont()) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }
}

void Label::update(float dt) {
    m_text.setPosition(getGlobalPosition());

    if (m_autoResize && m_text.getFont()) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }

    UIElement::update(dt);
}

void Label::render(sf::RenderTarget& target) {
    if (!isVisible() || !m_text.getFont()) return;

    target.draw(m_text);

    UIElement::render(target);
}

void Label::setText(const std::string& text) {
    m_text.setString(text);

    if (m_autoResize && m_text.getFont()) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }
}

const std::string& Label::getText() const {
    return m_text.getString();
}

void Label::setFont(const sf::Font& font) {
    m_text.setFont(font);

    if (m_autoResize) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }
}

void Label::setTextColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

sf::Color Label::getTextColor() const {
    return m_text.getFillColor();
}

void Label::setCharacterSize(unsigned int size) {
    m_text.setCharacterSize(size);

    if (m_autoResize && m_text.getFont()) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }
}

unsigned int Label::getCharacterSize() const {
    return m_text.getCharacterSize();
}

void Label::setAutoResize(bool autoResize) {
    m_autoResize = autoResize;

    if (m_autoResize && m_text.getFont()) {
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_size = sf::Vector2f(bounds.width + bounds.left, bounds.height + bounds.top);
    }
}

bool Label::getAutoResize() const {
    return m_autoResize;
}

void Label::setAlignment(sf::Text::Style alignment) {
    m_text.setStyle(alignment);
}

sf::Text::Style Label::getAlignment() const {
    return static_cast<sf::Text::Style>(m_text.getStyle());
}

// ----------------- Image Implementation -----------------

Image::Image()
    : UIElement(),
    m_texture(nullptr)
{
}

Image::Image(const std::string& id, sf::Texture* texture, const sf::Vector2f& position)
    : UIElement(id, position, texture ? sf::Vector2f(texture->getSize().x, texture->getSize().y) : sf::Vector2f(0, 0)),
    m_texture(texture)
{
    if (texture) {
        m_sprite.setTexture(*texture);
        m_sprite.setPosition(position);
    }
}

void Image::update(float dt) {
    m_sprite.setPosition(getGlobalPosition());

    if (m_texture) {
        m_sprite.setScale(
            m_size.x / m_texture->getSize().x * m_scale.x,
            m_size.y / m_texture->getSize().y * m_scale.y
        );
    }

    UIElement::update(dt);
}

void Image::render(sf::RenderTarget& target) {
    if (!isVisible() || !m_texture) return;

    target.draw(m_sprite);

    UIElement::render(target);
}

void Image::setTexture(sf::Texture* texture) {
    m_texture = texture;

    if (texture) {
        m_sprite.setTexture(*texture);

        if (m_size.x == 0 && m_size.y == 0) {
            m_size = sf::Vector2f(texture->getSize().x, texture->getSize().y);
        }

        m_sprite.setScale(
            m_size.x / texture->getSize().x * m_scale.x,
            m_size.y / texture->getSize().y * m_scale.y
        );
    }
}

sf::Texture* Image::getTexture() const {
    return m_texture;
}

void Image::setTextureRect(const sf::IntRect& rect) {
    m_sprite.setTextureRect(rect);
}

sf::IntRect Image::getTextureRect() const {
    return m_sprite.getTextureRect();
}

// ----------------- ProgressBar Implementation -----------------

ProgressBar::ProgressBar()
    : UIElement(),
    m_minValue(0.0f),
    m_maxValue(100.0f),
    m_currentValue(0.0f),
    m_backgroundColor(sf::Color(50, 50, 50)),
    m_fillColor(sf::Color(0, 200, 0)),
    m_textColor(sf::Color::White),
    m_showText(true)
{
    m_background.setFillColor(m_backgroundColor);
    m_fillBar.setFillColor(m_fillColor);

    m_text.setFillColor(m_textColor);
    m_text.setCharacterSize(12);
}

ProgressBar::ProgressBar(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size,
    float minValue, float maxValue, float currentValue)
    : UIElement(id, position, size),
    m_minValue(minValue),
    m_maxValue(maxValue),
    m_currentValue(currentValue),
    m_backgroundColor(sf::Color(50, 50, 50)),
    m_fillColor(sf::Color(0, 200, 0)),
    m_textColor(sf::Color::White),
    m_showText(true)
{
    m_background.setSize(size);
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundColor);

    float fillWidth = (m_currentValue - m_minValue) / (m_maxValue - m_minValue) * size.x;
    m_fillBar.setSize(sf::Vector2f(fillWidth, size.y));
    m_fillBar.setPosition(position);
    m_fillBar.setFillColor(m_fillColor);

    m_text.setString(std::to_string(static_cast<int>(m_currentValue)));
    m_text.setFillColor(m_textColor);
    m_text.setCharacterSize(12);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void ProgressBar::update(float dt) {
    sf::Vector2f pos = getGlobalPosition();
    m_background.setPosition(pos);
    m_fillBar.setPosition(pos);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(pos.x + m_size.x / 2.0f, pos.y + m_size.y / 2.0f);

    m_background.setSize(m_size);

    float fillWidth = (m_currentValue - m_minValue) / (m_maxValue - m_minValue) * m_size.x;
    fillWidth = std::max(0.0f, std::min(fillWidth, m_size.x));
    m_fillBar.setSize(sf::Vector2f(fillWidth, m_size.y));

    UIElement::update(dt);
}

void ProgressBar::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    target.draw(m_background);

    target.draw(m_fillBar);

    if (m_showText && m_text.getFont()) {
        target.draw(m_text);
    }

    UIElement::render(target);
}

void ProgressBar::setMinValue(float minValue) {
    m_minValue = minValue;

    m_currentValue = std::max(m_minValue, std::min(m_currentValue, m_maxValue));

    m_text.setString(std::to_string(static_cast<int>(m_currentValue)));
}

float ProgressBar::getMinValue() const {
    return m_minValue;
}

void ProgressBar::setMaxValue(float maxValue) {
    m_maxValue = maxValue;

    m_currentValue = std::max(m_minValue, std::min(m_currentValue, m_maxValue));

    m_text.setString(std::to_string(static_cast<int>(m_currentValue)));
}

float ProgressBar::getMaxValue() const {
    return m_maxValue;
}

void ProgressBar::setCurrentValue(float currentValue) {
    m_currentValue = std::max(m_minValue, std::min(currentValue, m_maxValue));

    m_text.setString(std::to_string(static_cast<int>(m_currentValue)));
}

float ProgressBar::getCurrentValue() const {
    return m_currentValue;
}

void ProgressBar::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
    m_background.setFillColor(color);
}

sf::Color ProgressBar::getBackgroundColor() const {
    return m_backgroundColor;
}

void ProgressBar::setFillColor(const sf::Color& color) {
    m_fillColor = color;
    m_fillBar.setFillColor(color);
}

sf::Color ProgressBar::getFillColor() const {
    return m_fillColor;
}

void ProgressBar::setTextColor(const sf::Color& color) {
    m_textColor = color;
    m_text.setFillColor(color);
}

sf::Color ProgressBar::getTextColor() const {
    return m_textColor;
}

void ProgressBar::setShowText(bool show) {
    m_showText = show;
}

bool ProgressBar::getShowText() const {
    return m_showText;
}

void ProgressBar::setText(const std::string& text) {
    m_text.setString(text);
}

const std::string& ProgressBar::getText() const {
    return m_text.getString();
}

void ProgressBar::setFont(const sf::Font& font) {
    m_text.setFont(font);
}

// ----------------- Panel Implementation -----------------

Panel::Panel()
    : UIElement(),
    m_drawBackground(true),
    m_hasBorder(false),
    m_borderThickness(1.0f),
    m_borderColor(sf::Color::White)
{
    m_background.setFillColor(sf::Color(50, 50, 50, 200));
}

Panel::Panel(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size)
    : UIElement(id, position, size),
    m_drawBackground(true),
    m_hasBorder(false),
    m_borderThickness(1.0f),
    m_borderColor(sf::Color::White)
{
    m_background.setSize(size);
    m_background.setPosition(position);
    m_background.setFillColor(sf::Color(50, 50, 50, 200));
}

void Panel::update(float dt) {
    m_background.setPosition(getGlobalPosition());
    m_background.setSize(m_size);

    if (m_hasBorder) {
        m_background.setOutlineThickness(m_borderThickness);
        m_background.setOutlineColor(m_borderColor);
    }
    else {
        m_background.setOutlineThickness(0);
    }

    UIElement::update(dt);
}

void Panel::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    if (m_drawBackground) {
        target.draw(m_background);
    }

    UIElement::render(target);
}

bool Panel::handleEvent(const sf::Event& event) {
    if (UIElement::handleEvent(event)) {
        return true;
    }

    if (!isActive() || !isInteractive()) return false;

    if ((event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (containsPoint(mousePos)) {
            if (event.type == sf::Event::MouseButtonPressed) {
                click();
            }
            return true;
        }
    }

    return false;
}

void Panel::setDrawBackground(bool draw) {
    m_drawBackground = draw;
}

bool Panel::getDrawBackground() const {
    return m_drawBackground;
}

void Panel::setBackgroundColor(const sf::Color& color) {
    m_background.setFillColor(color);
}

sf::Color Panel::getBackgroundColor() const {
    return m_background.getFillColor();
}

void Panel::setBorder(bool hasBorder) {
    m_hasBorder = hasBorder;
}

bool Panel::hasBorder() const {
    return m_hasBorder;
}

void Panel::setBorderThickness(float thickness) {
    m_borderThickness = thickness;
}

float Panel::getBorderThickness() const {
    return m_borderThickness;
}

void Panel::setBorderColor(const sf::Color& color) {
    m_borderColor = color;
}

sf::Color Panel::getBorderColor() const {
    return m_borderColor;
}

// ----------------- TextBox Implementation -----------------

TextBox::TextBox()
    : UIElement(),
    m_content(""),
    m_placeholder("Enter text..."),
    m_isFocused(false),
    m_isPassword(false),
    m_maxLength(100),
    m_characterSize(16),
    m_textColor(sf::Color::White),
    m_placeholderColor(sf::Color(150, 150, 150)),
    m_backgroundColor(sf::Color(30, 30, 30)),
    m_borderColor(sf::Color(100, 100, 100)),
    m_borderThickness(1.0f),
    m_showCursor(false)
{
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(m_borderThickness);
    m_background.setOutlineColor(m_borderColor);

    m_text.setFillColor(m_textColor);
    m_text.setCharacterSize(m_characterSize);

    m_placeholderText.setFillColor(m_placeholderColor);
    m_placeholderText.setCharacterSize(m_characterSize);

    m_cursor.setFillColor(m_textColor);
    m_cursor.setSize(sf::Vector2f(2.0f, static_cast<float>(m_characterSize) + 4.0f));

    m_cursorBlinkClock.restart();
}

TextBox::TextBox(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size)
    : UIElement(id, position, size),
    m_content(""),
    m_placeholder("Enter text..."),
    m_isFocused(false),
    m_isPassword(false),
    m_maxLength(100),
    m_characterSize(16),
    m_textColor(sf::Color::White),
    m_placeholderColor(sf::Color(150, 150, 150)),
    m_backgroundColor(sf::Color(30, 30, 30)),
    m_borderColor(sf::Color(100, 100, 100)),
    m_borderThickness(1.0f),
    m_showCursor(false)
{
    m_background.setSize(size);
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(m_borderThickness);
    m_background.setOutlineColor(m_borderColor);

    m_text.setFillColor(m_textColor);
    m_text.setCharacterSize(m_characterSize);
    m_text.setPosition(position.x + 5.0f, position.y + (size.y - m_characterSize) / 2.0f);

    m_placeholderText.setString(m_placeholder);
    m_placeholderText.setFillColor(m_placeholderColor);
    m_placeholderText.setCharacterSize(m_characterSize);
    m_placeholderText.setPosition(position.x + 5.0f, position.y + (size.y - m_characterSize) / 2.0f);

    m_cursor.setFillColor(m_textColor);
    m_cursor.setSize(sf::Vector2f(2.0f, static_cast<float>(m_characterSize) + 4.0f));
    m_cursor.setPosition(position.x + 5.0f, position.y + (size.y - m_cursor.getSize().y) / 2.0f);

    m_cursorBlinkClock.restart();
}

void TextBox::update(float dt) {
    m_background.setPosition(getGlobalPosition());
    m_background.setSize(m_size);

    sf::Vector2f pos = getGlobalPosition();
    m_text.setPosition(pos.x + 5.0f, pos.y + (m_size.y - m_characterSize) / 2.0f);
    m_placeholderText.setPosition(pos.x + 5.0f, pos.y + (m_size.y - m_characterSize) / 2.0f);

    if (m_isFocused) {
        float textWidth = 0;
        if (!m_content.empty() && m_text.getFont()) {
            sf::Text measureText = m_text;
            measureText.setString(m_isPassword ? std::string(m_content.length(), '*') : m_content);
            textWidth = measureText.getLocalBounds().width;
        }

        m_cursor.setPosition(pos.x + 5.0f + textWidth, pos.y + (m_size.y - m_cursor.getSize().y) / 2.0f);

        if (m_cursorBlinkClock.getElapsedTime().asSeconds() > 0.5f) {
            m_showCursor = !m_showCursor;
            m_cursorBlinkClock.restart();
        }
    }
    else {
        m_showCursor = false;
    }

    UIElement::update(dt);
}

void TextBox::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    target.draw(m_background);

    if (m_content.empty() && !m_isFocused) {
        if (m_placeholderText.getFont()) {
            target.draw(m_placeholderText);
        }
    }
    else {
        if (m_text.getFont()) {
            sf::Text displayText = m_text;
            displayText.setString(m_isPassword ? std::string(m_content.length(), '*') : m_content);
            target.draw(displayText);
        }
    }

    if (m_isFocused && m_showCursor) {
        target.draw(m_cursor);
    }

    UIElement::render(target);
}

bool TextBox::handleEvent(const sf::Event& event) {
    if (UIElement::handleEvent(event)) {
        return true;
    }

    if (!isActive() || !isInteractive()) return false;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (containsPoint(mousePos)) {
            focus();
            return true;
        }
        else if (m_isFocused) {
            unfocus();
        }
    }

    if (m_isFocused) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode >= 32 && event.text.unicode < 127) {
                if (m_content.length() < m_maxLength) {
                    m_content += static_cast<char>(event.text.unicode);

                    if (m_onTextChanged) {
                        m_onTextChanged(m_content);
                    }

                    m_showCursor = true;
                    m_cursorBlinkClock.restart();
                }
                return true;
            }
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::BackSpace) {
                if (!m_content.empty()) {
                    m_content.pop_back();

                    if (m_onTextChanged) {
                        m_onTextChanged(m_content);
                    }

                    m_showCursor = true;
                    m_cursorBlinkClock.restart();
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Return) {
                if (m_onEnter) {
                    m_onEnter();
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Escape) {
                unfocus();
                return true;
            }
        }
    }

    return false;
}

void TextBox::setText(const std::string& text) {
    m_content = text;

    if (m_content.length() > m_maxLength) {
        m_content = m_content.substr(0, m_maxLength);
    }

    if (m_onTextChanged) {
        m_onTextChanged(m_content);
    }
}

const std::string& TextBox::getText() const {
    return m_content;
}

void TextBox::setPlaceholder(const std::string& text) {
    m_placeholder = text;
    m_placeholderText.setString(text);
}

const std::string& TextBox::getPlaceholder() const {
    return m_placeholder;
}

void TextBox::setFont(const sf::Font& font) {
    m_text.setFont(font);
    m_placeholderText.setFont(font);
}

void TextBox::setTextColor(const sf::Color& color) {
    m_textColor = color;
    m_text.setFillColor(color);
    m_cursor.setFillColor(color);
}

sf::Color TextBox::getTextColor() const {
    return m_textColor;
}

void TextBox::setPlaceholderColor(const sf::Color& color) {
    m_placeholderColor = color;
    m_placeholderText.setFillColor(color);
}

sf::Color TextBox::getPlaceholderColor() const {
    return m_placeholderColor;
}

void TextBox::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
    m_background.setFillColor(color);
}

sf::Color TextBox::getBackgroundColor() const {
    return m_backgroundColor;
}

void TextBox::setBorderColor(const sf::Color& color) {
    m_borderColor = color;
    m_background.setOutlineColor(color);
}

sf::Color TextBox::getBorderColor() const {
    return m_borderColor;
}

void TextBox::setBorderThickness(float thickness) {
    m_borderThickness = thickness;
    m_background.setOutlineThickness(thickness);
}

float TextBox::getBorderThickness() const {
    return m_borderThickness;
}

void TextBox::setCharacterSize(unsigned int size) {
    m_characterSize = size;
    m_text.setCharacterSize(size);
    m_placeholderText.setCharacterSize(size);
    m_cursor.setSize(sf::Vector2f(2.0f, static_cast<float>(size) + 4.0f));
}

unsigned int TextBox::getCharacterSize() const {
    return m_characterSize;
}

void TextBox::setMaxLength(unsigned int length) {
    m_maxLength = length;

    if (m_content.length() > m_maxLength) {
        m_content = m_content.substr(0, m_maxLength);

        if (m_onTextChanged) {
            m_onTextChanged(m_content);
        }
    }
}

unsigned int TextBox::getMaxLength() const {
    return m_maxLength;
}

void TextBox::setPassword(bool isPassword) {
    m_isPassword = isPassword;
}

bool TextBox::isPassword() const {
    return m_isPassword;
}

void TextBox::focus() {
    if (!m_isFocused) {
        m_isFocused = true;

        m_showCursor = true;
        m_cursorBlinkClock.restart();

        m_background.setOutlineColor(sf::Color(100, 150, 255));

        UIElement::focus();
    }
}

void TextBox::unfocus() {
    if (m_isFocused) {
        m_isFocused = false;
        m_showCursor = false;

        m_background.setOutlineColor(m_borderColor);

        UIElement::unfocus();
    }
}

void TextBox::setOnTextChangedCallback(const std::function<void(const std::string&)>& callback) {
    m_onTextChanged = callback;
}

void TextBox::setOnEnterCallback(const std::function<void()>& callback) {
    m_onEnter = callback;
}

// ----------------- CheckBox Implementation -----------------

CheckBox::CheckBox()
    : UIElement(),
    m_checked(false),
    m_boxColor(sf::Color(80, 80, 80)),
    m_checkMarkColor(sf::Color::White),
    m_labelColor(sf::Color::White)
{
    m_box.setSize(sf::Vector2f(16.0f, 16.0f));
    m_box.setFillColor(m_boxColor);
    m_box.setOutlineThickness(1.0f);
    m_box.setOutlineColor(sf::Color(150, 150, 150));

    m_checkMark.setSize(sf::Vector2f(10.0f, 10.0f));
    m_checkMark.setFillColor(m_checkMarkColor);

    m_label.setFillColor(m_labelColor);
    m_label.setCharacterSize(14);

    m_size = sf::Vector2f(120.0f, 20.0f);
}

CheckBox::CheckBox(const std::string& id, const std::string& label, const sf::Vector2f& position, bool checked)
    : UIElement(id, position, sf::Vector2f(120.0f, 20.0f)),
    m_checked(checked),
    m_boxColor(sf::Color(80, 80, 80)),
    m_checkMarkColor(sf::Color::White),
    m_labelColor(sf::Color::White)
{
    m_box.setSize(sf::Vector2f(16.0f, 16.0f));
    m_box.setPosition(position);
    m_box.setFillColor(m_boxColor);
    m_box.setOutlineThickness(1.0f);
    m_box.setOutlineColor(sf::Color(150, 150, 150));

    m_checkMark.setSize(sf::Vector2f(10.0f, 10.0f));
    m_checkMark.setPosition(position.x + 3.0f, position.y + 3.0f);
    m_checkMark.setFillColor(m_checkMarkColor);

    m_label.setString(label);
    m_label.setFillColor(m_labelColor);
    m_label.setCharacterSize(14);
    m_label.setPosition(position.x + 24.0f, position.y);
}

void CheckBox::update(float dt) {
    sf::Vector2f pos = getGlobalPosition();
    m_box.setPosition(pos);
    m_checkMark.setPosition(pos.x + 3.0f, pos.y + 3.0f);
    m_label.setPosition(pos.x + 24.0f, pos.y);

    if (m_label.getFont()) {
        sf::FloatRect bounds = m_label.getLocalBounds();
        m_size.x = 24.0f + bounds.width;
    }

    UIElement::update(dt);
}

void CheckBox::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    target.draw(m_box);

    if (m_checked) {
        target.draw(m_checkMark);
    }

    if (m_label.getFont()) {
        target.draw(m_label);
    }

    UIElement::render(target);
}

bool CheckBox::handleEvent(const sf::Event& event) {
    if (UIElement::handleEvent(event)) {
        return true;
    }

    if (!isActive() || !isInteractive()) return false;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (containsPoint(mousePos)) {
            toggle();
            click();
            return true;
        }
    }

    return false;
}

void CheckBox::setLabel(const std::string& label) {
    m_label.setString(label);
}

const std::string& CheckBox::getLabel() const {
    return m_label.getString();
}

void CheckBox::setFont(const sf::Font& font) {
    m_label.setFont(font);
}

void CheckBox::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;

        if (m_onCheckedChanged) {
            m_onCheckedChanged(m_checked);
        }
    }
}

bool CheckBox::isChecked() const {
    return m_checked;
}

void CheckBox::toggle() {
    setChecked(!m_checked);
}

void CheckBox::setBoxColor(const sf::Color& color) {
    m_boxColor = color;
    m_box.setFillColor(color);
}

sf::Color CheckBox::getBoxColor() const {
    return m_boxColor;
}

void CheckBox::setCheckMarkColor(const sf::Color& color) {
    m_checkMarkColor = color;
    m_checkMark.setFillColor(color);
}

sf::Color CheckBox::getCheckMarkColor() const {
    return m_checkMarkColor;
}

void CheckBox::setLabelColor(const sf::Color& color) {
    m_labelColor = color;
    m_label.setFillColor(color);
}

sf::Color CheckBox::getLabelColor() const {
    return m_labelColor;
}

void CheckBox::setOnCheckedChangedCallback(const std::function<void(bool)>& callback) {
    m_onCheckedChanged = callback;
}

// ----------------- Dropdown Implementation -----------------

Dropdown::Dropdown()
    : UIElement(),
    m_isOpen(false),
    m_selectedIndex(-1),
    m_backgroundColor(sf::Color(50, 50, 50)),
    m_textColor(sf::Color::White),
    m_highlightColor(sf::Color(100, 150, 255)),
    m_borderColor(sf::Color(100, 100, 100)),
    m_borderThickness(1.0f)
{
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineColor(m_borderColor);
    m_background.setOutlineThickness(m_borderThickness);

    m_selectedText.setFillColor(m_textColor);
    m_selectedText.setCharacterSize(14);
}

Dropdown::Dropdown(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size)
    : UIElement(id, position, size),
    m_isOpen(false),
    m_selectedIndex(-1),
    m_backgroundColor(sf::Color(50, 50, 50)),
    m_textColor(sf::Color::White),
    m_highlightColor(sf::Color(100, 150, 255)),
    m_borderColor(sf::Color(100, 100, 100)),
    m_borderThickness(1.0f)
{
    m_background.setSize(size);
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineColor(m_borderColor);
    m_background.setOutlineThickness(m_borderThickness);

    m_selectedText.setFillColor(m_textColor);
    m_selectedText.setCharacterSize(14);
    m_selectedText.setPosition(position.x + 5.0f, position.y + (size.y - 14.0f) / 2.0f);
}

void Dropdown::update(float dt) {
    sf::Vector2f pos = getGlobalPosition();
    m_background.setPosition(pos);
    m_background.setSize(m_size);

    m_selectedText.setPosition(pos.x + 5.0f, pos.y + (m_size.y - m_selectedText.getCharacterSize()) / 2.0f);

    UIElement::update(dt);
}

void Dropdown::render(sf::RenderTarget& target) {
    if (!isVisible()) return;

    target.draw(m_background);

    if (m_selectedText.getFont()) {
        target.draw(m_selectedText);
    }

    if (m_isOpen) {
        sf::Vector2f pos = getGlobalPosition();
        float itemHeight = 25.0f;
        float totalHeight = itemHeight * m_items.size();

        sf::RectangleShape itemsBackground;
        itemsBackground.setPosition(pos.x, pos.y + m_size.y);
        itemsBackground.setSize(sf::Vector2f(m_size.x, totalHeight));
        itemsBackground.setFillColor(m_backgroundColor);
        itemsBackground.setOutlineColor(m_borderColor);
        itemsBackground.setOutlineThickness(m_borderThickness);
        target.draw(itemsBackground);

        for (size_t i = 0; i < m_items.size(); ++i) {
            sf::Text itemText = m_selectedText;
            itemText.setString(m_items[i].text);
            itemText.setPosition(pos.x + 5.0f, pos.y + m_size.y + i * itemHeight + (itemHeight - itemText.getCharacterSize()) / 2.0f);

            if (static_cast<int>(i) == m_selectedIndex) {
                sf::RectangleShape highlight;
                highlight.setPosition(pos.x, pos.y + m_size.y + i * itemHeight);
                highlight.setSize(sf::Vector2f(m_size.x, itemHeight));
                highlight.setFillColor(m_highlightColor);
                target.draw(highlight);

                itemText.setFillColor(sf::Color::White);
            }
            else {
                itemText.setFillColor(m_textColor);
            }

            target.draw(itemText);
        }
    }

    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    arrow.setPoint(0, sf::Vector2f(0, 0));
    arrow.setPoint(1, sf::Vector2f(10, 0));
    arrow.setPoint(2, sf::Vector2f(5, 5));
    arrow.setFillColor(m_textColor);

    sf::Vector2f pos = getGlobalPosition();
    arrow.setPosition(pos.x + m_size.x - 15.0f, pos.y + (m_size.y - 5.0f) / 2.0f);

    if (m_isOpen) {
        arrow.setScale(1.0f, -1.0f);
        arrow.move(0.0f, 5.0f);
    }

    target.draw(arrow);

    UIElement::render(target);
}

bool Dropdown::handleEvent(const sf::Event& event) {
    if (UIElement::handleEvent(event)) {
        return true;
    }

    if (!isActive() || !isInteractive()) return false;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        if (containsPoint(mousePos)) {
            m_isOpen = !m_isOpen;

            click();
            return true;
        }

        if (m_isOpen) {
            sf::Vector2f pos = getGlobalPosition();
            float itemHeight = 25.0f;

            sf::FloatRect itemsArea(
                pos.x,
                pos.y + m_size.y,
                m_size.x,
                itemHeight * m_items.size()
            );

            if (itemsArea.contains(mousePos)) {
                int clickedIndex = static_cast<int>((mousePos.y - (pos.y + m_size.y)) / itemHeight);

                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(m_items.size())) {
                    setSelectedIndex(clickedIndex);

                    m_isOpen = false;
                    return true;
                }
            }
        }

        if (m_isOpen) {
            m_isOpen = false;
            return true;
        }
    }

    return false;
}

void Dropdown::addItem(const std::string& text, const std::string& value) {
    DropdownItem item;
    item.text = text;
    item.value = value.empty() ? text : value;
    m_items.push_back(item);

    if (m_items.size() == 1 && m_selectedIndex == -1) {
        setSelectedIndex(0);
    }
}

void Dropdown::removeItem(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_items.erase(m_items.begin() + index);

        if (m_selectedIndex == index) {
            if (m_items.empty()) {
                m_selectedIndex = -1;
                m_selectedText.setString("");
            }
            else if (m_selectedIndex >= static_cast<int>(m_items.size())) {
                setSelectedIndex(m_items.size() - 1);
            }
        }
        else if (m_selectedIndex > index) {
            m_selectedIndex--;
        }
    }
}

void Dropdown::clearItems() {
    m_items.clear();
    m_selectedIndex = -1;
    m_selectedText.setString("");
}

void Dropdown::setSelectedIndex(int index) {
    if (index >= -1 && index < static_cast<int>(m_items.size()) && index != m_selectedIndex) {
        m_selectedIndex = index;

        if (index >= 0) {
            m_selectedText.setString(m_items[index].text);

            if (m_onSelectionChanged) {
                m_onSelectionChanged(index, m_items[index].value);
            }
        }
        else {
            m_selectedText.setString("");
        }
    }
}

int Dropdown::getSelectedIndex() const {
    return m_selectedIndex;
}

void Dropdown::setSelectedValue(const std::string& value) {
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].value == value) {
            setSelectedIndex(static_cast<int>(i));
            return;
        }
    }

    setSelectedIndex(-1);
}

std::string Dropdown::getSelectedValue() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
        return m_items[m_selectedIndex].value;
    }
    return "";
}

void Dropdown::setFont(const sf::Font& font) {
    m_selectedText.setFont(font);
}

void Dropdown::setBackgroundColor(const sf::Color& color) {
    m_backgroundColor = color;
    m_background.setFillColor(color);
}

sf::Color Dropdown::getBackgroundColor() const {
    return m_backgroundColor;
}

void Dropdown::setTextColor(const sf::Color& color) {
    m_textColor = color;
    m_selectedText.setFillColor(color);
}

sf::Color Dropdown::getTextColor() const {
    return m_textColor;
}

void Dropdown::setHighlightColor(const sf::Color& color) {
    m_highlightColor = color;
}

sf::Color Dropdown::getHighlightColor() const {
    return m_highlightColor;
}

void Dropdown::setBorderColor(const sf::Color& color) {
    m_borderColor = color;
    m_background.setOutlineColor(color);
}

sf::Color Dropdown::getBorderColor() const {
    return m_borderColor;
}

void Dropdown::setBorderThickness(float thickness) {
    m_borderThickness = thickness;
    m_background.setOutlineThickness(thickness);
}

float Dropdown::getBorderThickness() const {
    return m_borderThickness;
}

void Dropdown::open() {
    m_isOpen = true;
}

void Dropdown::close() {
    m_isOpen = false;
}

bool Dropdown::isOpen() const {
    return m_isOpen;
}

void Dropdown::setOnSelectionChangedCallback(const std::function<void(int, const std::string&)>& callback) {
    m_onSelectionChanged = callback;
}

//
//// ----------------- Dialogue Implementation -----------------
//
//
//DialogueBox::DialogueBox(const sf::Font& font, const sf::Vector2f& position, const sf::Vector2f& size)
//    : m_currentLine(0) {
//
//    m_background = Panel("dialoguePanel", position, size);
//    m_background.setBackgroundColor(sf::Color(0, 0, 0, 180));
//    m_background.setBorder(true);
//
//    m_textLabel = Label("dialogueText", "", position + sf::Vector2f(10.f, 10.f));
//    m_textLabel.setFont(font);
//    m_textLabel.setTextColor(sf::Color::White);
//    m_textLabel.setCharacterSize(18);
//
//    m_nextButton = Button("nextBtn", "Suivant",
//        position + sf::Vector2f(size.x - 110.f, size.y - 40.f),
//        { 100.f, 30.f });
//    m_nextButton.setFont(font);
//    m_nextButton.setOnClickCallback([this]() 
//        {
//        this->nextLine();
//        });
//}
//
//void DialogueBox::setDialogueLines(const std::vector<std::string>& lines) {
//    m_lines = lines;
//    m_currentLine = 0;
//
//    if (!m_lines.empty()) {
//        m_fullText = m_lines[0];
//        m_displayedText.clear();
//        m_elapsedTime = 0.f;
//        m_textFullyDisplayed = false;
//        m_textLabel.setText("");
//    }
//}
//
//void DialogueBox::setOnDialogueEndCallback(const std::function<void()>& callback) {
//    m_onDialogueEnd = callback;
//}
//
//void DialogueBox::nextLine() {
//    if (!m_textFullyDisplayed) 
//    {
//        m_displayedText = m_fullText;
//        m_textLabel.setText(m_displayedText);
//        m_textFullyDisplayed = true;
//        return;
//    }
//
//    m_currentLine++;
//    if (m_currentLine < m_lines.size()) 
//    {
//        m_fullText = m_lines[m_currentLine];
//        m_displayedText.clear();
//        m_elapsedTime = 0.f;
//        m_textFullyDisplayed = false;
//        m_textLabel.setText("");
//    }
//    else
//    {
//        if (m_onDialogueEnd)
//            m_onDialogueEnd();
//    }
//}
//
//void DialogueBox::update(float dt)
//{
//    updateTypewriter(dt);
//    m_background.update(dt);
//    m_textLabel.update(dt);
//    m_nextButton.update(dt);
//}
//
//void DialogueBox::updateTypewriter(float dt)
//{
//    if (m_textFullyDisplayed || m_fullText.empty()) return;
//
//    m_elapsedTime += dt;
//
//    if (m_elapsedTime >= m_typewriterSpeed) 
//    {
//        if (m_displayedText.size() < m_fullText.size())
//        {
//            m_displayedText += m_fullText[m_displayedText.size()];
//            m_textLabel.setText(m_displayedText);
//        }
//        else 
//            m_textFullyDisplayed = true;
//        m_elapsedTime = 0.f;
//    }
//}
//
//void DialogueBox::render(sf::RenderTarget& target)
//{
//    m_background.render(target);
//    m_textLabel.render(target);
//    m_nextButton.render(target);
//}
//
//bool DialogueBox::handleEvent(const sf::Event& event)
//{
//    return m_nextButton.handleEvent(event);
//}