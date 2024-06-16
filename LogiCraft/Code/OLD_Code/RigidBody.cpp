
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

#include "RigidBody.h"
#include "GameObject.h"

const float g = 400.f;

lc::RigidBody::RigidBody()
{
	m_name = "RigidBody";
	m_type = TYPE::RIGIDBODY;
	m_typeName = "RigidBody";
}

lc::RigidBody::RigidBody(sf::FloatRect _collider, sf::Vector2f _velocity, sf::Vector2f _relative, sf::Vector2f posBeforeSimulate, bool showCollide, bool kinetic)
{
	m_name = "RigidBody";
	m_typeName = "RigidBody";
	m_collider = _collider;
	m_velocity = _velocity;
	m_relativePosition = _relative;
	m_posBeforeSimulate = posBeforeSimulate;
	m_showCollider = showCollide;
	m_isKinetic = kinetic;
	m_type = TYPE::RIGIDBODY;
}

lc::RigidBody::~RigidBody()
{
}

void lc::RigidBody::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
	// texture.draw(m_renderer);
	if (!getParent()->getIsSaved())
	{
		save << (int)m_type
			<< " " << Tools::replaceSpace(m_typeName, true)
			<< " " << m_collider.left
			<< " " << m_collider.top
			<< " " << m_collider.width
			<< " " << m_collider.height
			<< " " << m_velocity.x
			<< " " << m_velocity.y
			<< " " << m_relativePosition.x
			<< " " << m_relativePosition.y
			<< " " << m_isKinetic
			<< " " << m_is_flying_
			<< " " << Tools::replaceSpace(m_name, true) << std::endl;
	}
}

void lc::RigidBody::Export(std::ofstream& exportation)
{
	exportation << (int)m_type
		<< " " << m_collider.left
		<< " " << m_collider.top
		<< " " << m_collider.width
		<< " " << m_collider.height
		<< " " << m_relativePosition.x
		<< " " << m_relativePosition.y
		<< " " << m_isKinetic
	    << " " << m_is_flying_
		<< std::endl;
}

void lc::RigidBody::Load(std::ifstream& load)
{
	load >> m_typeName
		>> m_collider.left
		>> m_collider.top
		>> m_collider.width
		>> m_collider.height
		>> m_velocity.x
		>> m_velocity.y
		>> m_relativePosition.x
		>> m_relativePosition.y
		>> m_isKinetic
		>> m_is_flying_
		>> m_name;
	Tools::replaceSpace(m_typeName, false);
	Tools::replaceSpace(m_name, false);
}

void lc::RigidBody::UpdateEvent(sf::Event& _window)
{
}

void lc::RigidBody::Update(WindowManager& _window)
{
	auto scene = getParent();
	while (scene->getParent())
	{
		scene = scene->getParent();
	}

	if(!m_collider.width && !m_collider.height)
	{
		m_collider.width = getParent()->getTransform().getSize().x;
		m_collider.height = getParent()->getTransform().getSize().y;
	}

	for (auto& i : m_inputFunc)
	{
		if(i)
			i(this);
	}

	if(m_isKinetic and m_simulate)
	{
		if(!m_is_flying_)
			m_velocity.y += (g)*Tools::getDeltaTime();
		CheckAllObject(scene);
	}
	else m_velocity = { 0.f,0.f };

	if (m_isKinetic)
		if (getParent())
		{
			getParent()->getTransform().getPosition() += m_velocity * Tools::getDeltaTime();
		}
	if (getParent())
	{
		m_collider.left = getParent()->getTransform().getPosition().x + m_relativePosition.x;
		m_collider.top = getParent()->getTransform().getPosition().y + m_relativePosition.y;
	}
}

void lc::RigidBody::Draw(WindowManager& _window)
{
	if (m_showCollider)
	{
		m_shape.setSize(m_collider.getSize());
		m_shape.setPosition(getParent()->getTransform().getPosition() + m_collider.getPosition());
		m_shape.setFillColor(sf::Color::Transparent);
		m_shape.setOutlineColor(sf::Color::Blue);
		m_shape.setOutlineThickness(5.f);
		_window.draw(m_shape);
	}
}

void lc::RigidBody::Draw(sf::RenderTexture& _window)
{
	if (m_showCollider)
	{
		m_shape.setSize(m_collider.getSize());
		m_shape.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_shape.setFillColor(sf::Color::Transparent);
		m_shape.setOutlineColor(sf::Color::Blue);
		m_shape.setOutlineThickness(- 3.f);
		_window.draw(m_shape);
	}
}

std::shared_ptr<lc::GameComponent> lc::RigidBody::Clone()
{
	return std::make_shared<lc::RigidBody>(*this);
}

void lc::RigidBody::setHierarchieFunc()
{
	m_hierarchieInformation = [this] {
		float tmp[4]{ m_relativePosition.x, m_relativePosition.y, m_collider.width, m_collider.height };
		ImGui::DragFloat4("Collider", tmp);
		m_relativePosition.x = tmp[0], m_relativePosition.y = tmp[1], m_collider.width = tmp[2], m_collider.height = tmp[3];
		ImGui::Checkbox("Kinetic", &m_isKinetic);
		if (m_isKinetic)
		{
			ImGui::Checkbox("Flying", &m_is_flying_);
			
			if (ImGui::Checkbox("Simulate Gravity", &m_simulate))
			{
				if (m_simulate)
					m_posBeforeSimulate = getParent()->getTransform().getPosition();
				else
					getParent()->getTransform().getPosition() = m_posBeforeSimulate;
			}
		}
		else m_simulate = false;

		if(m_collider.width <= 0.f)
			m_collider.width = 1.f;
		if(m_collider.height <= 0.f)
			m_collider.height = 1.f;
		ImGui::Checkbox("Show Collider", &m_showCollider);

		if (ImGui::Button("Show All Colliders"))
		{
			const std::function<void(const std::shared_ptr<lc::GameObject>&)> show_all_colliders = [&](const std::shared_ptr<lc::GameObject>& game_object)
			{
				for (const auto& component : game_object->getComponents())
					if (component->getTypeName() == "RigidBody")
					{
						const auto rigidBody = std::dynamic_pointer_cast<lc::RigidBody>(component);
						rigidBody->m_showCollider = true;
					}
				
				for (const auto& tmp_game_object : game_object->getObjects())
					show_all_colliders(tmp_game_object);
			};

			show_all_colliders(lc::GameObject::GetRoot(getParent()));
		}
		
		if (ImGui::Button("UnShow All Colliders"))
		{
			const std::function<void(const std::shared_ptr<lc::GameObject>&)> unShow_all_colliders = [&](const std::shared_ptr<lc::GameObject>& game_object)
			{
				for (const auto& component : game_object->getComponents())
					if (component->getTypeName() == "RigidBody")
					{
						const auto rigidBody = std::dynamic_pointer_cast<lc::RigidBody>(component);
						rigidBody->m_showCollider = false;
					}
				
				for (const auto& tmp_game_object : game_object->getObjects())
					unShow_all_colliders(tmp_game_object);
			};

			unShow_all_colliders(lc::GameObject::GetRoot(getParent()));
		}
		};
}

void lc::RigidBody::AddInputFunction(std::function<void(lc::RigidBody*)> func)
{
	m_inputFunc.push_back(func);
}

bool lc::RigidBody::CheckAllObject(std::shared_ptr<lc::GameObject> _object)
{
	for (auto& i : _object->getComponents())
	{
		if(&*i != this)
			if (auto rb = dynamic_cast<lc::RigidBody*>(&*i))
			{
				if(m_velocity.y > 0.f)
				{
					if (Tools::Collisions::lineRect({ m_relativePosition + m_collider.getPosition() + sf::Vector2f{0.f,m_collider.getSize().y} + sf::Vector2f{0.f,m_velocity.y} *Tools::getDeltaTime(), {{m_relativePosition + m_collider.getPosition() + m_collider.getSize() + sf::Vector2f{0.f,m_velocity.y}*Tools::getDeltaTime() }} }, { rb->getParent()->getTransform().getPosition() + rb->m_relativePosition, rb->m_collider.getSize()}))
						m_velocity.y = 0.f;
				}
				else if (m_velocity.y < 0.f)
				{
					if (Tools::Collisions::lineRect({ m_relativePosition + m_collider.getPosition() + sf::Vector2f{0.f,m_velocity.y} *Tools::getDeltaTime(), {{m_relativePosition + m_collider.getPosition() + sf::Vector2f{m_collider.getSize().x,0.f} + sf::Vector2f{0.f,m_velocity.y}*Tools::getDeltaTime()}} }, { rb->getParent()->getTransform().getPosition() + rb->m_relativePosition, rb->m_collider.getSize() }))
						m_velocity.y = 0.f;
				}
				if (m_velocity.x < 0.f)
				{
					if (Tools::Collisions::lineRect({ m_relativePosition + m_collider.getPosition() + sf::Vector2f{m_velocity.x,0.f} *Tools::getDeltaTime(), {{m_relativePosition + m_collider.getPosition() + sf::Vector2f{0.f,m_collider.getSize().y} + sf::Vector2f{m_velocity.x,0.f}*Tools::getDeltaTime()}} }, { rb->getParent()->getTransform().getPosition() + rb->m_relativePosition, rb->m_collider.getSize() }))
						m_velocity.x = 0.f;
				}
				else if (m_velocity.x > 0.f)
				{
					if (Tools::Collisions::lineRect({ m_relativePosition + m_collider.getPosition() + sf::Vector2f{m_collider.getSize().x,0.f} + sf::Vector2f{m_velocity.x,0.f} *Tools::getDeltaTime(), {{m_relativePosition + m_collider.getPosition() + m_collider.getSize() + sf::Vector2f{m_velocity.x,0.f}*Tools::getDeltaTime() }} }, { rb->getParent()->getTransform().getPosition() + rb->m_relativePosition, rb->m_collider.getSize()}))
						m_velocity.x = 0.f;
				}
			}
	}

	for (auto& i : _object->getObjects())
		CheckAllObject(i);

	return true;
}
