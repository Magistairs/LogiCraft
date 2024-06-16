
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
#include "Ressource.h"

namespace lc
{
	class Font : public Ressource
	{
	public:
		Font();
		Font(std::string name, std::string path, std::string newRepo = "");
		Font(std::string name, std::string path, int character_size, int outline_thickness, std::string sentence, sf::Color insideColor, sf::Color outlineColor, sf::Vector2f _relative); // load constructor
		Font(const Font& font);
		virtual ~Font() override;
		sf::Font& getFont() { return m_font; }
		std::string getFontName() { return m_fontName; }
		std::string getFontPath() { return m_fontPath; }
		std::string& getSentence() { return m_sentence; }

		virtual void setHierarchieFunc();
		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		virtual void Export(std::ofstream& exportation) override;
		virtual void Load(std::ifstream& load) override;
	private:
		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;

		void UpdateText();

		std::string m_fontName;
		sf::Font m_font;
		int m_characterSize;
		int m_outlineThickness;

		std::string m_fontPath;
		std::string m_sentence;
		sf::Color m_insideColor;
		sf::Color m_outLineColor;

		sf::Texture m_rendererTexture;
	};
}


