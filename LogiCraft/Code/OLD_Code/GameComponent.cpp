
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

#include "GameComponent.h"

#include "GameObject.h"

lc::GameComponent::GameComponent()
	: m_needToBeDeleted(false), m_isVisible(true), m_isUpdated(true), m_ID(0u)
{
}

lc::GameComponent::GameComponent(std::string _name)
	: m_name(_name), m_needToBeDeleted(false), m_isVisible(true), m_isUpdated(true), m_ID(0u)
{
}

lc::GameComponent::~GameComponent()
{
}

void lc::GameComponent::Hierarchie_Draw(unsigned int _parentID, std::list<std::shared_ptr<lc::GameComponent>>::iterator& _actComp, std::list<std::shared_ptr<lc::GameComponent>>& _compList)
{
	if (m_typeName == "Texture")
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.f, 0.6f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.8f, 0.f, 0.8f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.f, 0.f, 1.f, 1.f));
	}
	else if (m_typeName == "Text")
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.8f, 0.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.f, 0.f, 0.f, 1.f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1215f, 0.22352f, 0.3450f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.21960f, 0.4823f, 0.7960f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25882f, 0.5882f, 1.f, 1.f));
	}
	if (ImGui::TreeNodeEx(std::string(m_typeName + std::to_string(_parentID) + std::to_string(m_ID)).c_str(),
		ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth,
		std::string(m_typeName + ':' + m_name).c_str()))
	{
		if (_actComp != std::prev(_compList.end()))
		{
			ImGui::SameLine();
			if (ImGui::ArrowButtonEx(std::string("Down" + std::to_string((*_actComp)->getID())).c_str(), ImGuiDir_Down, sf::Vector2f(15.f, 13.f)))
			{
				auto tmp_Object = (*_actComp);
				(*_actComp) = *std::next(_actComp);
				*std::next(_actComp) = tmp_Object;
			}
		}
		if (_actComp != _compList.begin())
		{
			ImGui::SameLine();
			if (ImGui::ArrowButtonEx(std::string("Up" + std::to_string((*_actComp)->getID())).c_str(), ImGuiDir_Up, sf::Vector2f(15.f, 13.f)))
			{
				auto tmp_Object = (*_actComp);
				(*_actComp) = *std::prev(_actComp);
				*std::prev(_actComp) = tmp_Object;
			}
		}

		if(getParent()->getParent())
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
			if (ImGui::ImageButton(std::string("Delete component" + std::to_string(_parentID) + std::to_string(m_ID)).c_str(), GET_MANAGER->getTexture("poubelle"), sf::Vector2f(15.f, 15.f)))
				m_needToBeDeleted = true;

			ImGui::SameLine();
			ImGui::Text("Delete component");

			ImGui::Checkbox(std::string("is Visible##" + std::to_string(m_ID)).c_str(), &m_isVisible);
		}

		if (m_hierarchieInformation)
			m_hierarchieInformation();
		else
			ImGui::Text("No option provided.");

		ImGui::TreePop();
	}
	else
	{
		if (_actComp != std::prev(_compList.end()))
		{
			ImGui::SameLine();
			if (ImGui::ArrowButtonEx(std::string("Down" + std::to_string((*_actComp)->getID())).c_str(), ImGuiDir_Down, sf::Vector2f(15.f, 13.f)))
			{
				auto tmp_Object = (*_actComp);
				(*_actComp) = *std::next(_actComp);
				*std::next(_actComp) = tmp_Object;
			}
		}
		if (_actComp != _compList.begin())
		{
			ImGui::SameLine();
			if (ImGui::ArrowButtonEx(std::string("Up" + std::to_string((*_actComp)->getID())).c_str(), ImGuiDir_Up, sf::Vector2f(15.f, 13.f)))
			{
				auto tmp_Object = (*_actComp);
				(*_actComp) = *std::prev(_actComp);
				*std::prev(_actComp) = tmp_Object;
			}
		}
	}
	ImGui::PopStyleColor(3);
}
