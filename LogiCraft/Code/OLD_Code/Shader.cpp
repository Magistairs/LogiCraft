#include "Shader.h"

lc::Shader::Shader::Shader()
    : m_time_(0.f)
{
    m_shader_ = std::make_shared<sf::Shader>();

    m_shader_states_.shader = m_shader_.get();
    m_shader_states_.transform = sf::Transform::Identity;
    m_shader_states_.blendMode = sf::BlendAlpha;
    m_shader_states_.texture = nullptr;
}

lc::Shader::Shader::~Shader()
{
    m_shader_.reset();
}

bool lc::Shader::Shader::is_totally_in(const std::shared_ptr<lc::GameObject>& game_object)
{
    if (game_object->getTransform().getPosition().x >= getParent()->getTransform().getPosition().x &&
       game_object->getTransform().getPosition().y >= getParent()->getTransform().getPosition().y &&
       (game_object->getTransform().getPosition().x) + (game_object->getTransform().getSize().x * std::abs(game_object->getTransform().getScale().x)) <=
       (getParent()->getTransform().getPosition().x) + (getParent()->getTransform().getSize().x * std::abs(getParent()->getTransform().getScale().x)) &&
       (game_object->getTransform().getPosition().y) + (game_object->getTransform().getSize().y * std::abs(game_object->getTransform().getScale().y)) <=
       (getParent()->getTransform().getPosition().y) + (getParent()->getTransform().getSize().y * std::abs(getParent()->getTransform().getScale().y)))
    {
        return true;
    }
    
    return false;
}
