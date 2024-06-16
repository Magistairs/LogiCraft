
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

namespace lc
{
	class Transform
	{
	public:
		Transform();
		Transform(sf::Vector2f _position, sf::Vector2f _size = sf::Vector2f(), sf::Vector2f _origin = sf::Vector2f(), sf::Vector2f _scale = sf::Vector2f(1.f, 1.f), float _rotation = 0.f);
		~Transform();

		sf::Vector2f& getPosition() { return m_position; }
		sf::Vector2f& getOrigin() { return m_origin; }
		sf::Vector2f& getScale() { return m_scale; }
		sf::Vector2f& getSize() { return m_size; }

		sf::Vector2f& getDistance() { return m_distance; }

		float& getRotation() { return m_rotation; }
	private:
		sf::Vector2f m_position;
		sf::Vector2f m_origin;
		sf::Vector2f m_scale;
		sf::Vector2f m_size;

		sf::Vector2f m_distance;

		float m_rotation;
	};
}