#include "DisplayCollider.h"


lc::DisplayCollider::DisplayCollider()
{
    m_typeName = "DisplayCollider";
    m_type = TYPE::DISPLAY_COLLIDER;
    m_name = "DisplayCollider";

}

lc::DisplayCollider::DisplayCollider(const sf::FloatRect& collider, const std::string& trigger,
                                     const std::string& triggered)
    : m_collider_(collider), m_trigger_name_(trigger), m_triggered_name_(triggered)
{
    m_typeName = "DisplayCollider";
    m_type = TYPE::DISPLAY_COLLIDER;
    m_name = "DisplayCollider";
}

void lc::DisplayCollider::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << static_cast<int>(m_type)
            << " " << Tools::replaceSpace(m_typeName, true)
            << " " << m_collider_.getSize()
            << " " << m_relativePosition.x
            << " " << m_relativePosition.y
            << " " << m_trigger_name_
            << " " << m_triggered_name_ << std::endl;
}

void lc::DisplayCollider::SaveRenderer(sf::RenderTexture& texture, int _depth)
{
    Ressource::SaveRenderer(texture, _depth);
}

void lc::DisplayCollider::Load(std::ifstream& load)
{
    sf::Vector2f size;
    load >> m_typeName
    >> size
    >> m_relativePosition
    >> m_trigger_name_
    >> m_triggered_name_;
    Tools::replaceSpace(m_typeName,false);
}

void lc::DisplayCollider::UpdateEvent(sf::Event& _window)
{
}

void lc::DisplayCollider::Update(WindowManager& _window)
{
    m_collider_ = sf::FloatRect(getParent()->getTransform().getPosition() + m_relativePosition, m_collider_.getSize());
}

void lc::DisplayCollider::Draw(WindowManager& _window)
{
    if (m_show_collider_)
    {
        m_renderer.setSize(m_collider_.getSize());
        m_renderer.setPosition(m_collider_.getPosition());
        m_renderer.setFillColor(sf::Color::Transparent);
        m_renderer.setOutlineColor(sf::Color::White);
        m_renderer.setOutlineThickness(2.f);
        _window.draw(m_renderer);
    }
}

void lc::DisplayCollider::Draw(sf::RenderTexture& _window)
{
    if (m_show_collider_)
    {
        m_renderer.setSize(m_collider_.getSize());
        m_renderer.setPosition(m_collider_.getPosition());
        m_renderer.setFillColor(sf::Color::Transparent);
        m_renderer.setOutlineColor(sf::Color::White);
        m_renderer.setOutlineThickness(2.f);
        _window.draw(m_renderer);
    }
}

std::shared_ptr<lc::GameComponent> lc::DisplayCollider::Clone()
{
    return std::make_shared<DisplayCollider>(*this);
}


void lc::DisplayCollider::Export(std::ofstream& exportation)
{
    exportation << static_cast<int>(m_type)
                << " " << m_relativePosition.x
                << " " << m_relativePosition.y
                << " " << m_trigger_name_
                << " " << m_triggered_name_ << std::endl;
}

void lc::DisplayCollider::setHierarchieFunc()
{
    m_hierarchieInformation = [this]()
    {
        ImGui::Checkbox("Show collider",&m_show_collider_);
        NewLine();
        DragFloat2("Relative position", m_relativePosition);
        auto colliderSize = m_collider_.getSize();
        DragFloat2("Collider size", colliderSize);
        m_collider_ = {getParent()->getTransform().getPosition() +m_relativePosition, colliderSize};
        NewLine();
        InputText("Game object trigger", m_trigger_name_, 150);
        InputText("Game object triggered", m_triggered_name_, 150);
    };
}
