#pragma once
#include "Ressource.h"

namespace lc
{
    class DisplayCollider : public Ressource
    {
        sf::FloatRect m_collider_;
        std::string m_trigger_name_, m_triggered_name_;
        bool m_show_collider_ = false;
    public:
        DisplayCollider();
        DisplayCollider(const sf::FloatRect& collider, const std::string& trigger, const std::string& triggered);

        void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
        void SaveRenderer(sf::RenderTexture& texture, int _depth) override;
        void Load(std::ifstream& load) override;

        void UpdateEvent(sf::Event& _window) override;
        void Update(WindowManager& _window) override;
        void Draw(WindowManager& _window) override;
        void Draw(sf::RenderTexture& _window) override;
        std::shared_ptr<GameComponent> Clone() override;
        void Export(std::ofstream& exportation) override;
        void setHierarchieFunc() override;
    };
}
