
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
#include "imnodes.h"
#include "imnodes_internal.h"

namespace lc
{
	class Event : public lc::Ressource
	{
		std::weak_ptr<lc::GameObject> m_objectA;
		std::string m_objectA_name;
		std::weak_ptr<lc::GameObject> m_objectB;
		std::string m_objectB_name;
		sf::Texture m_rendererTexture;
		short m_condition = -1;
		bool m_setFunction = false;
		bool m_isReverse = false;
		bool m_isOpen = false;
	public:
		Event();
		Event(std::string name, short function);
		virtual ~Event();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth);
		void Export(std::ofstream& exportation) override;
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load);

		virtual void UpdateEvent(sf::Event& _window);
		virtual void Update(WindowManager& _window);
		virtual void Draw(WindowManager& _window);
		virtual void Draw(sf::RenderTexture& _window);
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		/*
		* @brief Open a window to set event's interaction.
		*/
		void setHierarchieFunc();
		void setCondition();

		std::pair<std::weak_ptr<lc::GameObject>, std::weak_ptr<lc::GameObject>> GetObjectAffiliated() { return { m_objectA, m_objectB }; }
	};
}

