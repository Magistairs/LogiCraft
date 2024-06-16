#include "StatSystem.h"
#include "tools_imgui.h"

lc::StatSystem::StatSystem() : StatSystem(4, 1.f, 100.f, 500.f, 1)
{
    m_name = "StatSystem";
    m_typeName = "StatSystem";
    m_type = TYPE::STAT_SYSTEM;
}

lc::StatSystem::StatSystem(const unsigned int& health,const float& attack_speed,const float& attack_range,const float& speed,const unsigned int& jumpcount)
{
    m_name = "StatSystem";
    m_typeName = "StatSystem";
    m_type = TYPE::STAT_SYSTEM;
    
    m_base_health_i = health;

    m_base_dexterity_f = attack_speed;

    m_base_range_f = attack_range;

    m_base_speed_f = speed;
	
    m_base_jumpcount_i = jumpcount;
}

void lc::StatSystem::setHierarchieFunc()
{
    m_hierarchieInformation = [&]
    {
        int life = m_base_health_i;
        ImGui::InputInt("Life",&life);
        if(life <= 0)
            life = 1;
        m_base_health_i = life;

        ImGui::InputFloat("Dexterity",&m_base_dexterity_f);
        ImGui::InputFloat("Range",&m_base_range_f);
        ImGui::InputFloat("Speed",&m_base_speed_f);
    };
}

void lc::StatSystem::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << " " << static_cast<int>(m_type) <<
        " " << m_base_health_i <<
        " " << m_base_dexterity_f <<
        " " << m_base_range_f <<
        " " << m_base_speed_f;   
}

void lc::StatSystem::Load(std::ifstream& load)
{
    load >> m_base_health_i >> m_base_dexterity_f >> m_base_range_f >> m_base_speed_f;
}

void lc::StatSystem::UpdateEvent(sf::Event& _window)
{
}

void lc::StatSystem::Update(WindowManager& _window)
{
}

void lc::StatSystem::Draw(WindowManager& _window)
{
}

void lc::StatSystem::Draw(sf::RenderTexture& _window)
{
}

std::shared_ptr<lc::GameComponent> lc::StatSystem::Clone()
{
    return std::make_shared<StatSystem>(*this);
}

void lc::StatSystem::Export(std::ofstream& exportation)
{
    exportation << " " << static_cast<int>(m_type) <<
      " " << m_base_health_i <<
      " " << m_base_dexterity_f <<
      " " << m_base_range_f <<
      " " << m_base_speed_f;  
}
