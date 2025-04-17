#pragma once

#include "UIElements.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>

class Button : public UIElement {
private:
    sf::Text m_text;
    sf::RectangleShape m_shape;
    sf::Texture* m_texture;

    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_pressedColor;
    sf::Color m_disabledColor;

    bool m_isPressed;
    bool m_isHovered;

    std::function<void()> m_onClick;

public:
    Button();
    Button(const std::string& id, const std::string& text, const sf::Vector2f& position, const sf::Vector2f& size);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);

    void setTextColor(const sf::Color& color);
    sf::Color getTextColor() const;

    void setTexture(sf::Texture* texture);
    sf::Texture* getTexture() const;

    void setNormalColor(const sf::Color& color);
    void setHoverColor(const sf::Color& color);
    void setPressedColor(const sf::Color& color);
    void setDisabledColor(const sf::Color& color);

    void setOnClickCallback(const std::function<void()>& callback);

    bool isPressed() const;
    bool isHovered() const;
};

class Label : public UIElement {
private:
    sf::Text m_text;
    bool m_autoResize;

public:
    Label();
    Label(const std::string& id, const std::string& text, const sf::Vector2f& position);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);

    void setTextColor(const sf::Color& color);
    sf::Color getTextColor() const;

    void setCharacterSize(unsigned int size);
    unsigned int getCharacterSize() const;

    void setAutoResize(bool autoResize);
    bool getAutoResize() const;

    void setAlignment(sf::Text::Style alignment);
    sf::Text::Style getAlignment() const;
};

class Image : public UIElement {
private:
    sf::Sprite m_sprite;
    sf::Texture* m_texture;

public:
    Image();
    Image(const std::string& id, sf::Texture* texture, const sf::Vector2f& position);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    void setTexture(sf::Texture* texture);
    sf::Texture* getTexture() const;

    void setTextureRect(const sf::IntRect& rect);
    sf::IntRect getTextureRect() const;
};

class ProgressBar : public UIElement {
private:
    sf::RectangleShape m_background;
    sf::RectangleShape m_fillBar;
    sf::Text m_text;

    float m_minValue;
    float m_maxValue;
    float m_currentValue;

    sf::Color m_backgroundColor;
    sf::Color m_fillColor;
    sf::Color m_textColor;

    bool m_showText;

public:
    ProgressBar();
    ProgressBar(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size,
        float minValue = 0.0f, float maxValue = 100.0f, float currentValue = 0.0f);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    void setMinValue(float minValue);
    float getMinValue() const;

    void setMaxValue(float maxValue);
    float getMaxValue() const;

    void setCurrentValue(float currentValue);
    float getCurrentValue() const;

    void setBackgroundColor(const sf::Color& color);
    sf::Color getBackgroundColor() const;

    void setFillColor(const sf::Color& color);
    sf::Color getFillColor() const;

    void setTextColor(const sf::Color& color);
    sf::Color getTextColor() const;

    void setShowText(bool show);
    bool getShowText() const;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);
};

class Panel : public UIElement {
private:
    sf::RectangleShape m_background;
    bool m_drawBackground;
    bool m_hasBorder;
    float m_borderThickness;
    sf::Color m_borderColor;

public:
    Panel();
    Panel(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void setDrawBackground(bool draw);
    bool getDrawBackground() const;

    void setBackgroundColor(const sf::Color& color);
    sf::Color getBackgroundColor() const;

    void setBorder(bool hasBorder);
    bool hasBorder() const;

    void setBorderThickness(float thickness);
    float getBorderThickness() const;

    void setBorderColor(const sf::Color& color);
    sf::Color getBorderColor() const;
};

class TextBox : public UIElement {
private:
    sf::RectangleShape m_background;
    sf::Text m_text;
    sf::Text m_placeholderText;

    std::string m_content;
    std::string m_placeholder;

    bool m_isFocused;
    bool m_isPassword;

    unsigned int m_maxLength;
    unsigned int m_characterSize;

    sf::Color m_textColor;
    sf::Color m_placeholderColor;
    sf::Color m_backgroundColor;
    sf::Color m_borderColor;

    float m_borderThickness;

    sf::Clock m_cursorBlinkClock;
    bool m_showCursor;
    sf::RectangleShape m_cursor;

    std::function<void(const std::string&)> m_onTextChanged;
    std::function<void()> m_onEnter;

public:
    TextBox();
    TextBox(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setPlaceholder(const std::string& text);
    const std::string& getPlaceholder() const;

    void setFont(const sf::Font& font);

    void setTextColor(const sf::Color& color);
    sf::Color getTextColor() const;

    void setPlaceholderColor(const sf::Color& color);
    sf::Color getPlaceholderColor() const;

    void setBackgroundColor(const sf::Color& color);
    sf::Color getBackgroundColor() const;

    void setBorderColor(const sf::Color& color);
    sf::Color getBorderColor() const;

    void setBorderThickness(float thickness);
    float getBorderThickness() const;

    void setCharacterSize(unsigned int size);
    unsigned int getCharacterSize() const;

    void setMaxLength(unsigned int length);
    unsigned int getMaxLength() const;

    void setPassword(bool isPassword);
    bool isPassword() const;

    void focus() override;
    void unfocus() override;

    void setOnTextChangedCallback(const std::function<void(const std::string&)>& callback);
    void setOnEnterCallback(const std::function<void()>& callback);
};

class CheckBox : public UIElement {
private:
    sf::RectangleShape m_box;
    sf::RectangleShape m_checkMark;
    sf::Text m_label;

    bool m_checked;

    sf::Color m_boxColor;
    sf::Color m_checkMarkColor;
    sf::Color m_labelColor;

    std::function<void(bool)> m_onCheckedChanged;

public:
    CheckBox();
    CheckBox(const std::string& id, const std::string& label, const sf::Vector2f& position, bool checked = false);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void setLabel(const std::string& label);
    const std::string& getLabel() const;

    void setFont(const sf::Font& font);

    void setChecked(bool checked);
    bool isChecked() const;

    void toggle();

    void setBoxColor(const sf::Color& color);
    sf::Color getBoxColor() const;

    void setCheckMarkColor(const sf::Color& color);
    sf::Color getCheckMarkColor() const;

    void setLabelColor(const sf::Color& color);
    sf::Color getLabelColor() const;

    void setOnCheckedChangedCallback(const std::function<void(bool)>& callback);
};

class Dropdown : public UIElement {
private:
    struct DropdownItem {
        std::string text;
        std::string value;
    };

    sf::RectangleShape m_background;
    sf::Text m_selectedText;
    std::vector<DropdownItem> m_items;

    bool m_isOpen;
    int m_selectedIndex;

    sf::Color m_backgroundColor;
    sf::Color m_textColor;
    sf::Color m_highlightColor;
    sf::Color m_borderColor;

    float m_borderThickness;

    std::function<void(int, const std::string&)> m_onSelectionChanged;

public:
    Dropdown();
    Dropdown(const std::string& id, const sf::Vector2f& position, const sf::Vector2f& size);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    bool handleEvent(const sf::Event& event) override;

    void addItem(const std::string& text, const std::string& value = "");
    void removeItem(int index);
    void clearItems();

    void setSelectedIndex(int index);
    int getSelectedIndex() const;

    void setSelectedValue(const std::string& value);
    std::string getSelectedValue() const;

    void setFont(const sf::Font& font);

    void setBackgroundColor(const sf::Color& color);
    sf::Color getBackgroundColor() const;

    void setTextColor(const sf::Color& color);
    sf::Color getTextColor() const;

    void setHighlightColor(const sf::Color& color);
    sf::Color getHighlightColor() const;

    void setBorderColor(const sf::Color& color);
    sf::Color getBorderColor() const;

    void setBorderThickness(float thickness);
    float getBorderThickness() const;

    void open();
    void close();
    bool isOpen() const;

    void setOnSelectionChangedCallback(const std::function<void(int, const std::string&)>& callback);
};

