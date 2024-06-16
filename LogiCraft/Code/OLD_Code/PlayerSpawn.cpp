#include "PlayerSpawn.h"

#include <memory>

lc::PlayerSpawn::PlayerSpawn()
{
    m_type = TYPE::SPAWN_POSITION;
    m_typeName = "Player Spawn";
    m_name = "Player Spawn";

    m_player_screen_zone_.setSize({3640.f, 2160.f});
    m_player_screen_zone_.setFillColor(sf::Color::Transparent);
    m_player_screen_zone_.setOutlineColor(sf::Color::Red);
    m_player_screen_zone_.setOutlineThickness(-2.f);
}

void lc::PlayerSpawn::Export(std::ofstream& exportation)
{
    exportation << static_cast<int>(m_type) << " " << m_relativePosition;
}

void lc::PlayerSpawn::setHierarchieFunc()
{
    m_hierarchieInformation = [&]
    {
        ImGui::DragFloat2("Spawn Position",m_relativePosition);

        auto& tmp_size(const_cast<sf::Vector2f&>(m_player_screen_zone_.getSize()));
        ImGui::DragFloat2("Player Screen Zone", tmp_size);
    };
}

void lc::PlayerSpawn::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << static_cast<int>(m_type)
            << " " << Tools::replaceSpace(m_typeName, true)
            << " " << m_relativePosition << std::endl;
}

void lc::PlayerSpawn::Load(std::ifstream& load)
{
    load >> m_typeName
    >> m_relativePosition;
    Tools::replaceSpace(m_typeName,false);
}

void lc::PlayerSpawn::UpdateEvent(sf::Event& _window)
{
}

void lc::PlayerSpawn::Update(WindowManager& _window)
{
    m_player_screen_zone_.setOrigin(m_player_screen_zone_.getSize() / 2.f);
    m_player_screen_zone_.setPosition(getParent()->getObject("PLAYER")->getTransform().getPosition() + (getParent()->getObject("PLAYER")->getTransform().getSize() / 2.f));
}

void lc::PlayerSpawn::Draw(WindowManager& _window)
{
}

void lc::PlayerSpawn::Draw(sf::RenderTexture& _window)
{
    m_renderer.setPosition(m_relativePosition);
    m_renderer.setSize({280,270});
    _window.draw(m_renderer);
    _window.draw(m_player_screen_zone_);
}

std::shared_ptr<lc::GameComponent> lc::PlayerSpawn::Clone()
{
    return std::make_shared<PlayerSpawn>(*this);
}
