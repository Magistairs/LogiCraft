
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

#include "GameObject.h"
#include "Texture.h"

lc::GameObject::GameObject()
	: m_name(""), m_depth(0), m_ID(m_generalID++), m_needToBeRemove(false)
{}

lc::GameObject::GameObject(std::string _name)
	: m_name(""), m_depth(0), m_ID(m_generalID++), m_needToBeRemove(false) 
{}

lc::GameObject::GameObject(std::string _name, unsigned char _depth)
	: m_name(_name), m_depth(_depth), m_ID(m_generalID++), m_needToBeRemove(false)
{}

lc::GameObject::~GameObject()
{
	m_components.clear();
}

void lc::GameObject::Load(std::ifstream& load)
{
	std::string Garbage;
	auto check = [&](char c) {
		auto t = load.tellg();
		load >> Garbage;
		if (Garbage.find(c) == std::string::npos)
			load.seekg(t);
		};

	int tmp;
	load >> m_name;
	load >> m_transform.getPosition().x >> m_transform.getPosition().y;
	load >> m_transform.getScale().x >> m_transform.getScale().y;
	load >> m_transform.getSize().x >> m_transform.getSize().y;
	load >> m_transform.getRotation();
	load >> m_transform.getOrigin().x >> m_transform.getOrigin().y;
	load >> tmp;
	m_depth = (unsigned char)tmp;
	while (Garbage.find("{") == std::string::npos)
	{
		load >> Garbage;
	}
	check('}');


	while (Garbage.find("}") == std::string::npos)
	{
		int type;
		load >> type;
		if ((Ressource::TYPE)type == Ressource::TYPE::NONE)
		{

		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::TEXTURE)
		{
			auto texture = std::make_shared<Texture>();
			texture->Load(load);
			addComponent(texture);
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::FONT)
		{
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::IA)
		{
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::RIGIDBODY)
		{
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::EVENT)
		{
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::BUTTON)
		{
		}
		else if ((Ressource::TYPE)type == Ressource::TYPE::PARTICULES_SYSTEM)
		{
		}
		check('}');
	}

	load >> Garbage;
	while (Garbage.find("{") == std::string::npos)
	{
		load >> Garbage;
	}

	check('}');
	while (Garbage.find("}") == std::string::npos)
	{
		this->addObject(CreateGameObject("OBJECT"))->Load(load);
		check('}');
	}


}


std::shared_ptr<lc::GameObject> lc::GameObject::CreateGameObject(std::string _name, unsigned char _depth)
{
	return std::make_shared<GameObject>(_name, _depth);
}

void lc::GameObject::CheckMaxSize()
{
	lc::Ressource* SizeX{ 0 },* SizeY{ 0 };
	for (auto& component : getComponents())
	{

		if (auto ressources = dynamic_cast<lc::Ressource*>(&*component))
		{
			if (!SizeX or SizeX->getMaximumSize().x < ressources->getMaximumSize().x)
				SizeX = ressources;

			if (!SizeY or SizeY->getMaximumSize().y < ressources->getMaximumSize().y)
				SizeY = ressources;
		}
	}

	if(SizeX)
		m_transform.getSize().x = SizeX->getMaximumSize().x;
	if(SizeY)
		m_transform.getSize().y = SizeY->getMaximumSize().y;
}

void lc::GameObject::UpdateEvent(sf::Event& _event)
{
	for (auto& object : m_objects)
			object->UpdateEvent(_event);

	for (auto& component : m_components)
		component->UpdateEvent(_event);
}

void lc::GameObject::Update(WindowManager& _window)
{
	for (auto object = m_objects.begin(); object != m_objects.end();)
	{
		(*object)->Update(_window);
		
		if ((*object)->hasToBeRemoved())
			object = m_objects.erase(object);
		else
			object++;
	}

	for (auto component = m_components.begin(); component != m_components.end();)
	{
		(*component)->Update(_window);
		
		if ((*component)->needToBeDeleted())
			component = m_components.erase(component);
		else
			component++;
	}

	CheckMaxSize();	
}

void lc::GameObject::Draw(WindowManager& _window)
{
	for (auto& object : m_objects)
			object->Draw(_window);

	for (auto& component : m_components)
		component->Draw(_window);
}

void lc::GameObject::Draw(sf::RenderTexture& _renderer)
{
	for (auto& object : m_objects)
			object->Draw(_renderer);

	for (auto& component : m_components)
		component->Draw(_renderer);
}

void lc::GameObject::Draw(sf::RenderTexture& _renderer, unsigned char _depth)
{
	for (auto& object : m_objects)
			object->Draw(_renderer, _depth);

	for (auto& component : m_components)
		if (m_depth == _depth)
			component->Draw(_renderer);
}
