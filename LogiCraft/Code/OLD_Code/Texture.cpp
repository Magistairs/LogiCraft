
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

#include "Texture.h"
#include "GameObject.h"


lc::Texture::Texture()
{
	m_renderer.setTexture(&m_texture);
}

lc::Texture::Texture(std::string name, sf::IntRect rect, std::string path, std::string newRepo)
{
	fs::path p = path;
	m_textureName = p.filename().string();
	m_texture.loadFromFile(path);
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = name;
	m_typeName = "Texture";
	m_type = TYPE::TEXTURE;
	if (newRepo != "")
		m_texturePath = newRepo + "/" + m_textureName;
	else m_texturePath = path;

	m_filterColor = sf::Color(0xF90FF0FF);
}

lc::Texture::Texture(const Texture& _texture)
{
	m_texture = _texture.m_texture;
	m_textureName = _texture.m_textureName;
	m_texturePath = _texture.m_texturePath;
	m_textureRect = _texture.m_textureRect;
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = _texture.m_name;
	m_textureName = _texture.m_textureName;
	m_typeName = "Texture";
	m_filterColor = sf::Color(0xF90FF0FF);
	m_type = TYPE::TEXTURE;
}

lc::Texture::Texture(std::string name, sf::IntRect rect, std::string path, sf::Vector2f _position, sf::Vector2f _scale, sf::Vector2f _relative)
{
	m_typeName = "Texture";
	m_texturePath = path;
	fs::path p = path;
	m_textureName = p.filename().string();
	m_texture.loadFromFile(path);
	m_textureRect = rect;
	if (rect == sf::IntRect())
	{
		m_textureRect.width = m_texture.getSize().x;
		m_textureRect.height = m_texture.getSize().y;
	}
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_name = name;
	m_type = TYPE::TEXTURE;
	m_filterColor = sf::Color(0xF90FF0FF);
	m_relativePosition = _relative;
}

lc::Texture::~Texture()
{
}

void lc::Texture::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
	{
		save << static_cast<int>(m_type)
			<< " " << Tools::replaceSpace(m_typeName, true)
			<< " " << m_textureRect.left
			<< " " << m_textureRect.top
			<< " " << m_textureRect.width
			<< " " << m_textureRect.height
			<< " " << Tools::replaceSpace(m_texturePath, true)
			<< " " << m_relativePosition.x
			<< " " << m_relativePosition.y << std::endl;
	}
}

void lc::Texture::Load(std::ifstream& load)
{
	load >> m_typeName >>
		m_textureRect.left >>
		m_textureRect.top >>
		m_textureRect.width >>
		m_textureRect.height >>
		m_texturePath >>
		m_relativePosition.x >>
		m_relativePosition.y;

	fs::path p = m_texturePath;
	m_textureName = p.filename().string();
	m_name = p.filename().stem().string();
	m_texture.loadFromFile(m_texturePath);
	m_renderer.setTexture(&m_texture);
	m_renderer.setTextureRect(m_textureRect);
	m_renderer.setSize(sf::Vector2f(static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)));
	m_displayThumbnail = false;
	m_type = TYPE::TEXTURE;
}

void lc::Texture::Export(std::ofstream& exportation)
{
	exportation << static_cast<int>(m_type)
		<< " " << m_textureRect.left
		<< " " << m_textureRect.top
		<< " " << m_textureRect.width
		<< " " << m_textureRect.height
		<< " " << Tools::replaceSpace(fs::path(m_texturePath).filename().string(), true)
		<< " " << m_relativePosition.x
		<< " " << m_relativePosition.y << std::endl;

	if (const auto tmp = dynamic_cast<FileWriter*>(&exportation))
		Tools::copyFile(m_texturePath, tmp->getPath().parent_path().string() + "/Ressources/" + Tools::replaceSpace(m_textureName, true));
}

void lc::Texture::UpdateEvent(sf::Event& _window)
{
}

void lc::Texture::Update(WindowManager& _window)
{
	if (getParent())
		setPosition(getParent()->getTransform().getPosition());
}

void lc::Texture::Draw(WindowManager& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition());
	}

	m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());
	if (m_displayThumbnail)
		m_renderer.setSize(m_thumbnailSize);

	if (!getParent())
		_window.draw(m_renderer);
	else if (getParent()->is_in_window_view(_window))
		_window.draw(m_renderer);
}

void lc::Texture::Draw(sf::RenderTexture& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	m_renderer.setSize((sf::Vector2f)m_textureRect.getSize());

	if (!getParent())
		_window.draw(m_renderer);
	else if (getParent()->is_in_window_view(_window))
		_window.draw(m_renderer);
}

std::shared_ptr<lc::GameComponent> lc::Texture::Clone()
{
	return std::make_shared<lc::Texture>(*this);
}

void lc::Texture::setHierarchieFunc()
{
	m_hierarchieInformation = [this]()
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

			ImGui::Text("Texture Name : %s", m_textureName.c_str());

			if (m_isVisible)
				ImGui::DragFloat2("Relative Position", m_relativePosition, 1.0f, 0.0f, 0.0f, "%.2f");
	};
}
