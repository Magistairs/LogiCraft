
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

#pragma once
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/RessourcesManager.h"
#include "Ressource.h"

namespace lc
{
	class Texture : public Ressource
	{
	public:
		Texture();
		Texture(std::string name, sf::IntRect rect, std::string path, sf::Vector2f _position, sf::Vector2f _scale, sf::Vector2f _relative); // load constructor
		Texture(std::string name, sf::IntRect rect, std::string path, std::string newRepo = "");
		Texture(const Texture& _texture);
		virtual ~Texture();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void Load(std::ifstream& load) override;
		virtual void Export(std::ofstream& exportation) override;

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;

		sf::Vector2f getTextureSize() { return (sf::Vector2f)m_textureRect.getSize(); }
		std::string getTextureName() { return m_textureName; }
		std::string getTexturePath() { return m_texturePath; }
		sf::Texture& getTexture() { return m_texture; }
		sf::IntRect& getTextureRect() { return m_textureRect; }
	private:
		sf::Texture m_texture;
		std::string m_textureName;
		std::string m_texturePath;
		sf::IntRect m_textureRect;
	};
}

