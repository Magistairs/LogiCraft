#pragma once
#include "Ressource.h"

namespace lc
{
    class PlayerSpawn : public lc::Ressource
    {
    public:
        PlayerSpawn();
        void Export(std::ofstream& exportation) override;
        void setHierarchieFunc() override;
        void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
        void Load(std::ifstream& load) override;
        void UpdateEvent(sf::Event& _window) override;
        void Update(WindowManager& _window) override;
        void Draw(WindowManager& _window) override;
        void Draw(sf::RenderTexture& _window) override;
        std::shared_ptr<lc::GameComponent> Clone() override;

        const sf::Vector2f& GetSpawnPosition() const {return m_relativePosition;}
    private:
        sf::RectangleShape m_player_screen_zone_;
    };
}
