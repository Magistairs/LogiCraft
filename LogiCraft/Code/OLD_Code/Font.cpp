
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

#include "Font.h"
#include <filesystem>
#include "GameObject.h"
#include "Transform.h"


lc::Font::Font()
	: m_characterSize(10), m_outlineThickness(0)
{
	m_type = TYPE::FONT;
	m_typeName = "Text";
}

lc::Font::Font(std::string name, std::string path, std::string newRepo)
{
	m_typeName = "Text";
	fs::path p = path;
	m_fontName = p.filename().string();
	m_font.loadFromFile(path);
	sf::RenderTexture rt;
	m_thumbnailSize = { 100.f,100.f };
	m_renderer.setSize(m_thumbnailSize);
	rt.create(static_cast<unsigned>(m_thumbnailSize.x), static_cast<unsigned>(m_thumbnailSize.y));
	m_characterSize = 40u;
	sf::Text text("Aa1", m_font, m_characterSize);
	text.setOrigin(text.getGlobalBounds().getSize() / 2.f + sf::Vector2f(text.getLineSpacing(),text.getLineSpacing()) /2.f);
	text.setPosition(50, 50);
	rt.clear(sf::Color(0,0,0,0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture);
	m_displayThumbnail = true;
	m_name = name;
	m_type = TYPE::FONT;
	if (newRepo != "")
		m_fontPath = newRepo + "/" + m_fontName;
	else m_fontPath = path;
	m_sentence = "Lorem Ipsum";
	m_insideColor = sf::Color::White;
	m_outlineThickness = 0;
	m_outLineColor = sf::Color::Black;
	m_filterColor = sf::Color(0xF0D90FFF);
}

lc::Font::Font(std::string name, std::string path, int character_size, int outline_thickness, std::string sentence, sf::Color insideColor, sf::Color outlineColor, sf::Vector2f _relative)
{
	m_typeName = "Text";
	m_fontName = name;
	m_name = name;
	m_fontPath = path;
	m_font.loadFromFile(m_fontPath);
	m_characterSize = character_size;
	m_outlineThickness = outline_thickness;
	m_sentence = sentence;
	m_insideColor = insideColor;
	m_outLineColor = outlineColor;
	m_type = TYPE::FONT;
	sf::RenderTexture rt;
	sf::Text text(m_sentence, m_font, m_characterSize);
	rt.create(static_cast<unsigned>(text.getGlobalBounds().width), static_cast<unsigned>(text.getGlobalBounds().height));
	text.setOrigin(text.getGlobalBounds().getSize() / 2.f + sf::Vector2f(text.getLineSpacing(), text.getLineSpacing()) / 2.f);
	text.setPosition(rt.getSize().x / 2.f, rt.getSize().y / 2.f);
	rt.clear(sf::Color(0, 0, 0, 0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture);
	m_filterColor = sf::Color(0xF0D90FFF);
	m_relativePosition = _relative;
}

lc::Font::Font(const Font& font)
{
	m_typeName = "Text";
	m_font = font.m_font;
	m_fontName = font.m_fontName;
	m_fontPath = font.m_fontPath;
	m_characterSize = font.m_characterSize;
	m_sentence = font.m_sentence;
	m_insideColor = font.m_insideColor;
	m_outlineThickness = font.m_outlineThickness;
	m_outLineColor = font.m_outLineColor;
	m_relativePosition = font.m_relativePosition;
	m_rendererTexture = font.m_rendererTexture;
	m_renderer.setTexture(&m_rendererTexture);
	m_renderer.setSize(font.m_renderer.getSize());
	m_renderer.setPosition(font.m_renderer.getPosition());
	m_renderer.setRotation(font.m_renderer.getRotation());
	m_renderer.setScale(font.m_renderer.getScale());
	m_renderer.setOrigin(font.m_renderer.getOrigin());
	m_type = font.m_type;
	m_typeName = font.m_typeName;
	m_displayThumbnail = font.m_displayThumbnail;
	m_thumbnailSize = font.m_thumbnailSize;
	m_filterColor = sf::Color(0xF0D90FFF);
	m_name = font.m_name;
}

lc::Font::~Font()
{
}

void lc::Font::setHierarchieFunc()
{
	m_hierarchieInformation = [&] {
		if (ImGui::InputTextMultiline("Texte", m_sentence, 300))
		{
			UpdateText();
		}
		if(ImGui::InputInt("Character size", &m_characterSize))
		{
			if (m_characterSize <= 0)
				m_characterSize = 1;
			UpdateText();
		}
		if(ImGui::ColorEdit4("Text Color", m_insideColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB))
			UpdateText();

		if(ImGui::InputInt("Outline Thickness", &m_outlineThickness))
			UpdateText();
		if(ImGui::ColorEdit4("Outline Color", m_outLineColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB))
			UpdateText();
		if(ImGui::DragFloat2("Relative position", m_relativePosition, 1.0f, 0.0f, 0.0f, "%.2f"))
			UpdateText();

		};
}


void lc::Font::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)

{
	if (getParent()->getDepth() == _depth)
		texture.draw(m_renderer);
	{
		save << static_cast<int>(m_type)
			<< " " << Tools::replaceSpace(m_typeName, true)
			<< " " << Tools::replaceSpace(m_fontPath, true)
			<< " " << m_characterSize
			<< " " << m_outlineThickness
			<< " " << Tools::replaceSpace(m_sentence, true)
			<< " " << static_cast<int>(m_insideColor.r)
			<< " " << static_cast<int>(m_insideColor.g)
			<< " " << static_cast<int>(m_insideColor.b)
			<< " " << static_cast<int>(m_insideColor.a)
			<< " " << static_cast<int>(m_outLineColor.r)
			<< " " << static_cast<int>(m_outLineColor.g)
			<< " " << static_cast<int>(m_outLineColor.b)
			<< " " << static_cast<int>(m_outLineColor.a)
			<< " " << m_relativePosition.x
			<< " " << m_relativePosition.y << std::endl;
	}
}

void lc::Font::Export(std::ofstream& exportation)
{
	exportation << static_cast<int>(m_type)
		<< " " << Tools::replaceSpace(m_fontName, true)
		<< " " << m_characterSize
		<< " " << m_outlineThickness
		<< " " << Tools::replaceSpace(m_sentence, true)
		<< " " << static_cast<int>(m_insideColor.r)
		<< " " << static_cast<int>(m_insideColor.g)
		<< " " << static_cast<int>(m_insideColor.b)
		<< " " << static_cast<int>(m_insideColor.a)
		<< " " << static_cast<int>(m_outLineColor.r)
		<< " " << static_cast<int>(m_outLineColor.g)
		<< " " << static_cast<int>(m_outLineColor.b)
		<< " " << static_cast<int>(m_outLineColor.a)
		<< " " << m_relativePosition.x
		<< " " << m_relativePosition.y << std::endl;
	
	if (const auto tmp = dynamic_cast<FileWriter*>(&exportation))
		Tools::copyFile(m_fontPath, tmp->getPath().parent_path().string() + "/Ressources/" + Tools::replaceSpace(m_fontName, true));
}

void lc::Font::Load(std::ifstream& load)
{
	int tmp[8];
	load >> m_typeName >> m_fontPath >> m_characterSize >> m_outlineThickness >> m_sentence >>
		tmp[0] >> tmp[1] >> tmp[2] >> tmp[3] >>
		tmp[4] >> tmp[5] >> tmp[6] >> tmp[7] >>
		m_relativePosition.x >> m_relativePosition.y;

	m_insideColor = sf::Color(tmp[0], tmp[1], tmp[2], tmp[3]);
	m_outLineColor = sf::Color(tmp[4], tmp[5], tmp[6], tmp[7]);

	std::filesystem::path p = m_fontPath;
	m_fontName = p.filename().string();
	m_name = p.filename().stem().string();
	Tools::replaceSpace(m_sentence);

	m_font.loadFromFile(m_fontPath);
	UpdateText();

}


void lc::Font::UpdateEvent(sf::Event& _window)
{
}

void lc::Font::Update(WindowManager& _window)
{
}

void lc::Font::Draw(WindowManager& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	_window.draw(m_renderer);
}

void lc::Font::Draw(sf::RenderTexture& _window)
{
	if (getParent())
	{
		m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
		m_renderer.setScale(getParent()->getTransform().getScale());
		m_renderer.setRotation(getParent()->getTransform().getRotation());
		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
	}

	_window.draw(m_renderer);
}

std::shared_ptr<lc::GameComponent> lc::Font::Clone()
{
	return std::make_shared<lc::Font>(*this);
}


void lc::Font::UpdateText()
{
	sf::RenderTexture rt;
	sf::Text text(m_sentence, m_font, static_cast<unsigned>(m_characterSize));
	text.setFillColor(m_insideColor);
	text.setOutlineColor(m_outLineColor);
	text.setOutlineThickness(static_cast<float>(m_outlineThickness));
	text.setOrigin(text.getGlobalBounds().getSize() / 2.f);
	const auto render_texture_size = sf::Vector2f(text.getGlobalBounds().getSize().x, text.getGlobalBounds().getSize().y + text.getFont()->getLineSpacing(m_characterSize));
	rt.create(static_cast<unsigned>(render_texture_size.x), static_cast<unsigned>(render_texture_size.y));
	text.setPosition(render_texture_size / 2.f);
	rt.clear(sf::Color(0, 0, 0, 0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture, true);
	m_renderer.setSize(text.getGlobalBounds().getSize() + sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing()));
	m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
}
