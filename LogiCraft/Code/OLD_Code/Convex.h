#pragma once
#include "Ressource.h"
#include "GameObject.h"

namespace lc
{
	class Convex : public lc::Ressource
	{
	public:
		Convex();
		Convex(std::vector<sf::Vector2f>& container);
		Convex(const Convex& _convex);
		~Convex();
		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void Load(std::ifstream& load) override; 
		virtual void SaveRenderer(sf::RenderTexture& texture, int _depth) override;

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;
		sf::Vector2f& getPosition() { return m_position; }
		sf::ConvexShape& getConvex() { return m_convex; }
	private:
		sf::ConvexShape m_convex;
		inline static int m_convex_id = 0;
		sf::Vector2f m_position;
	};
}