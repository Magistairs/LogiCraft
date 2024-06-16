
/*------------------------------------LICENSE------------------------------------
MIT License

Copyright (c) [2024] [GRALLAN Yann]
Copyright (c) [2024] [LESAGE Charles]
Copyright (c) [2024] [MENA-BOUR Samy]

This software utilizes code from the following GitHub repositories, which are also licensed under the MIT License:

- [SFML](https://github.com/SFML)
- [ImGUI](https://github.com/ocornut/imgui)
- [ImNodes](https://github.com/Nelarius/imnodes)
- [SFML-Manager](https://github.com/Xanhos/Cpp-Manager-for-SFML)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
---------------------------------------------------------------------------------*/

#include "Button.h"

lc::Button::Button()
{
    //m_renderer = sf::RectangleShape({ 100.f, 100.f });
    //m_renderer.setPosition(100.f,100.f);
   // m_renderer.setFillColor(sf::Color::White);
    m_type = TYPE::BUTTON;
    m_typeName = "Button";
}

lc::Button::Button(std::string _name, sf::Vector2f _position, sf::Vector2f _scale)
{
    m_typeName = "Button";
    m_renderer.setPosition(_position);
    m_renderer.setFillColor(sf::Color::White);
    m_renderer.setScale(_scale);
    m_name = _name;
    m_type = TYPE::BUTTON;
}

lc::Button::Button(std::string _name, sf::Vector2f _relative, short function)
{
    m_typeName = "Button";
    m_renderer.setFillColor(sf::Color::White);
    m_name = _name;
    m_type = TYPE::BUTTON;
    m_relativePosition = _relative;
    m_function = function;
}

lc::Button::~Button()
{
}

void lc::Button::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << static_cast<int>(m_type) 
        << " " << Tools::replaceSpace(m_typeName, true)
        << " " << m_function;
}

void lc::Button::Export(std::ofstream& exportation)
{
    exportation << static_cast<int>(m_type) << " " << m_function;
}

void lc::Button::Load(std::ifstream& load)
{
    load >> m_typeName >> m_function;
}

void lc::Button::UpdateEvent(sf::Event& _window)
{
}

void lc::Button::Update(WindowManager& _window)
{
    if (getParent())
        setPosition(getParent()->getTransform().getPosition());
}

void lc::Button::Draw(WindowManager& _window)
{
    //if (getParent())
    //{
    //    m_renderer.setPosition(getParent()->getTransform().getPosition());
    //    m_renderer.setSize(sf::Vector2f(100.f, 100.f));
    //}


    //_window.draw(m_renderer);

    //onDraw(_window, sf::Color::Magenta);
}

void lc::Button::Draw(sf::RenderTexture& _window)
{
    if (getParent())
    {
        m_renderer.setSize(getParent()->getTransform().getSize());
        m_renderer.setFillColor(sf::Color::Transparent);
        m_renderer.setOutlineColor(sf::Color::White);
        m_renderer.setOutlineThickness(-2.5f);
        m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
        m_renderer.setScale(getParent()->getTransform().getScale());
        m_renderer.setRotation(getParent()->getTransform().getRotation());
        m_renderer.setOrigin(getParent()->getTransform().getOrigin());
    }
    if (m_showCollider)
    {
        _window.draw(m_renderer);
    }
}

std::shared_ptr<lc::GameComponent> lc::Button::Clone()
{
    return std::make_shared<lc::Button>(*this);
}

void lc::Button::setHierarchieFunc()
{
    m_hierarchieInformation = [this] 
        {
            ImGui::Checkbox("Show Collider", &m_showCollider);
            ImGui::Checkbox("Set Function", &m_setFunction);
            if (m_setFunction)
            {
                const char* items[] = { "Go to Menu", "Go to Play", "Go to Options", "Quit Game", "Show Controls", "+ Music", "- Music", "+ Sound", "- Sound", "Toggle Fullscreen" };
                const char* current_item = "No Function Set Actually";
                if (ImGui::BeginCombo(std::string("Functions available " + std::to_string(getParent()->getID())).c_str(), m_function == -1 ? current_item : items[m_function], ImGuiComboFlags_WidthFitPreview))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                    {
                        bool is_selected = (current_item == items[n]);
                        if (ImGui::Selectable(items[n], is_selected))
                        {
                            m_function = n;
                            current_item = items[n];
                        }
                    }
                    ImGui::EndCombo();
                }
            }
        };
}
