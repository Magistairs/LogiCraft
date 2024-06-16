#include "Convex.h"

lc::Convex::Convex()
{
	m_type = TYPE::CONVEX;
	m_convex.setFillColor(sf::Color::Black);
	m_typeName = "Convex";
	m_name = "Convex_" + std::to_string(m_convex_id++);
	m_position = { 0.f,0.f };
}

lc::Convex::Convex(std::vector<sf::Vector2f>& container)
{
	m_type = TYPE::CONVEX;
	m_name = "Convex_" + std::to_string(m_convex_id++);
	m_convex.setFillColor(sf::Color::Black);
	m_typeName = "Convex";
	m_convex.setPointCount(container.size());
	sf::Vector2f tmpPosition = {};
	for (size_t i = 0; i < container.size(); i++)
	{
		tmpPosition += container[i];
		m_convex.setPoint(i, container[i]);
	}
	m_position = tmpPosition / static_cast<float>(container.size());
	m_convex.setOrigin(m_position);
	container.clear();
}

lc::Convex::Convex(const Convex& _convex) : m_convex(_convex.m_convex)
{
	m_type = TYPE::CONVEX;
	m_typeName = "Convex";
	m_position = _convex.m_position;
	m_convex.setOrigin(m_position);
}

lc::Convex::~Convex()
{
}

void lc::Convex::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
	save << static_cast<int>(TYPE::CONVEX)
		<< " " << "Convex"
		<< " " << m_convex.getPointCount();
	for (size_t i = 0; i < m_convex.getPointCount(); i++)
	{
		save << " " << m_convex.getPoint(i).x << " " << m_convex.getPoint(i).y;
	}
	save << " " << m_position.x << " " << m_position.y << std::endl;
}

void lc::Convex::Load(std::ifstream& load)
{
	
	m_type = TYPE::CONVEX;
	load >> m_typeName;
	int nb_points(0);
	load >> nb_points;
	float x(0);
	float y(0);
	m_convex.setPointCount(nb_points);
	for (size_t i = 0; i < m_convex.getPointCount(); i++)
	{
		load >> x >> y;
		m_convex.setPoint(i, { x, y });
	}
	load >> m_position.x >> m_position.y;
	m_convex.setOrigin(m_position);

	if(getParent()->getName() == "PLAYER")
	{
		m_convex.setPoint(0,{0,0});
		m_convex.setPoint(1,{280,0});
		m_convex.setPoint(2,{280,270});
		m_convex.setPoint(3,{0,270});
		m_convex.setFillColor(sf::Color::Red);
	}
}

void lc::Convex::SaveRenderer(sf::RenderTexture& texture, int _depth)
{
	if (getParent())
		if (getParent()->getDepth() == _depth)
			texture.draw(m_convex);
}

void lc::Convex::UpdateEvent(sf::Event& _window)
{
}

void lc::Convex::Update(WindowManager& _window)
{
	m_convex.setPosition(getParent()->getTransform().getPosition());
	m_convex.setScale(getParent()->getTransform().getScale());
	m_convex.setOrigin(getParent()->getTransform().getOrigin());
	m_renderer.setSize(m_convex.getGlobalBounds().getSize());

	if(getParent()->getName() == PLAYER_NAME)
		m_convex.setFillColor(sf::Color::Red);
}

void lc::Convex::Draw(WindowManager& _window)
{
	_window.draw(m_convex);
}

void lc::Convex::Draw(sf::RenderTexture& _window)
{
	_window.draw(m_convex);
}

std::shared_ptr<lc::GameComponent> lc::Convex::Clone()
{
	return std::shared_ptr<lc::GameComponent>();
}

void lc::Convex::setHierarchieFunc()
{
	
}
