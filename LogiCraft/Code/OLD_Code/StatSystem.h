#pragma once
#include "Ressource.h"

namespace lc
{
    class StatSystem : public lc::Ressource
    {
        unsigned int m_base_health_i;

        float m_base_dexterity_f;

        float m_base_range_f;

        float m_base_speed_f;

        int m_base_jumpcount_i;
	
    public:
        StatSystem();
	
        StatSystem(const unsigned int& health,
            const float& attack_speed,
            const float& attack_range,
            const float& speed,
            const unsigned int& jumpcount);
        void setHierarchieFunc() override;
        void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
        void Load(std::ifstream& load) override;
        void UpdateEvent(sf::Event& _window) override;
        void Update(WindowManager& _window) override;
        void Draw(WindowManager& _window) override;
        void Draw(sf::RenderTexture& _window) override;
        std::shared_ptr<lc::GameComponent> Clone() override;
        void Export(std::ofstream& exportation) override;
    };

}
