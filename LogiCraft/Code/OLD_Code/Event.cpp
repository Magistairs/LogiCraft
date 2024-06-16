
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

#include "Event.h"

lc::Event::Event()
{
	m_name = "Event";
}

lc::Event::Event(std::string name, short function)
{
	m_displayThumbnail = false;
	m_name = "Event";
	m_typeName = "Event";
	m_type = TYPE::EVENT;

	sf::RenderTexture rt;
	sf::Text text(m_name, GET_MANAGER->getFont("arial"), 30);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);
	auto t = sf::Vector2f(text.getGlobalBounds().getSize().x, text.getGlobalBounds().getSize().y + text.getFont()->getLineSpacing(20));
	rt.create(static_cast<unsigned>(t.x), static_cast<unsigned>(t.y));

	rt.clear(sf::Color(0, 0, 0, 0));
	rt.draw(text);
	rt.display();
	m_rendererTexture = rt.getTexture();
	m_renderer.setTexture(&m_rendererTexture, true);
	m_renderer.setSize(text.getGlobalBounds().getSize() + sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing()));
	m_filterColor = sf::Color(0x0FF0D9FF);
}



lc::Event::~Event()
{
}

void lc::Event::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{

	save << static_cast<int>(m_type)
		<< " " << Tools::replaceSpace(m_typeName, true)
		<< " " << m_condition
		<< " " << static_cast<int>(m_isReverse)
		<< " " << Tools::replaceSpace(m_objectA.lock()->getName(), true)
		<< " " << Tools::replaceSpace(m_objectB.lock()->getName(), true)
		<< std::endl;
	
}

void lc::Event::Export(std::ofstream& exportation)
{
	exportation << static_cast<int>(m_type)
		<< " " << m_condition << " " << static_cast<int>(m_isReverse)
		<< " " << Tools::replaceSpace(m_objectA.lock()->getName(), true)
		<< " " << Tools::replaceSpace(m_objectB.lock()->getName(), true);

}

void lc::Event::Load(std::ifstream& load)
{
	load >> m_typeName >> m_condition >> m_isReverse >> m_objectA_name >> m_objectB_name;
	m_objectA_name = Tools::replaceSpace(m_objectA_name);
	m_objectB_name = Tools::replaceSpace(m_objectB_name);

	m_displayThumbnail = false;
	m_type = TYPE::EVENT;
}

void lc::Event::UpdateEvent(sf::Event& _window)
{
}

void lc::Event::Update(WindowManager& _window)
{
	if (m_displayThumbnail)
	{
		sf::RenderTexture rt;
		sf::Text text(m_name, GET_MANAGER->getFont(""), 30);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1.f);
		auto t = sf::Vector2f(text.getGlobalBounds().getSize().x + text.getOutlineThickness(), text.getGlobalBounds().getSize().y + text.getFont()->getLineSpacing(20) + text.getOutlineThickness());
		rt.create(static_cast<unsigned>(t.x), static_cast<unsigned>(t.y));

		rt.clear(sf::Color(0, 0, 0, 0));
		rt.draw(text);
		rt.display();
		m_rendererTexture = rt.getTexture();
		m_renderer.setTexture(&m_rendererTexture, true);
		m_renderer.setSize(text.getGlobalBounds().getSize() + sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing()));
	}
	if (m_objectA.expired() and lc::GameObject::GetRoot(getParent())->hasObject(m_objectA_name))
	{
		m_objectA = lc::GameObject::GetRoot(getParent())->getObject(m_objectA_name);
	}
	if (m_objectB.expired() and lc::GameObject::GetRoot(getParent())->hasObject(m_objectB_name))
	{
		m_objectB = lc::GameObject::GetRoot(getParent())->getObject(m_objectA_name);
	}
}

void lc::Event::Draw(WindowManager& _window)
{
	_window.draw(m_renderer);
}

void lc::Event::Draw(sf::RenderTexture& _window)
{
	//_window.draw(m_renderer);

}

std::shared_ptr<lc::GameComponent> lc::Event::Clone()
{
	return std::make_shared<lc::Event>(*this);
}

void lc::Event::setHierarchieFunc()
{
	m_hierarchieInformation = [this]
		{
			ImGui::Checkbox("Set Event", &m_setFunction);
			if (m_setFunction)
			{
				auto scene = getParent();
				while (scene->getParent())
					scene = std::make_shared<lc::GameObject>(*scene->getParent());

				if(ImGui::Begin(std::string(getName() + "'s Window##lc" + std::to_string(getID())).c_str(), &m_setFunction))
				{
					static int linkid = 0;
					ImNodes::BeginNodeEditor();

					static std::vector<std::pair<int, std::pair<int, int>>> links;
					static const float text_width = ImGui::CalcTextSize("=> Receive information").x;

					for (int i = 0; i < links.size(); ++i)
					{
						ImNodes::Link(links[i].first, links[i].second.first, links[i].second.second);
						if (ImNodes::IsLinkSelected(links[i].first) && KEY(Backspace))
						{
							if (!m_objectA.expired())
							{
								if (links[i].second.first == m_objectA.lock()->getID() || links[i].second.first == m_objectA.lock()->getID() + 100000)
								{
									m_objectA.reset();
								}
							}
							if (!m_objectB.expired())
							{
								if (links[i].second.second == m_objectB.lock()->getID() || links[i].second.second == m_objectB.lock()->getID() + 100000)
								{
									m_objectB.reset();
								}
							}
							links.erase(links.begin() + i);
						}
					}

					for (auto& objects : scene->getObjects())
					{
						while (objects->getObjects().size() != 0)
						{
							for (auto& it : objects->getObjects())
							{
								ImNodes::BeginNode(it->getID());
								ImNodes::BeginNodeTitleBar();
								ImGui::TextUnformatted(std::string("Name: " + it->getName()).c_str());
								ImNodes::EndNodeTitleBar();
								ImGui::Text(std::string("position x: " + std::to_string(it->getTransform().getPosition().x)).c_str());
								ImGui::Text(std::string("position y: " + std::to_string(it->getTransform().getPosition().y)).c_str());
								if (it->getName() == "EVENT")
								{
									ImNodes::BeginInputAttribute(it->getID());
									ImGui::Text("=> Receive information");
									ImNodes::EndInputAttribute();
									m_objectA.lock() ? ImGui::Text(m_objectA.lock()->getName().c_str()) : ImGui::Text("Object A");
									ImNodes::BeginOutputAttribute(it->getID() + 100000);
									ImGui::Indent(text_width);
									ImGui::Text("Send information =>");
									ImNodes::EndOutputAttribute();
									m_objectB.lock() ? ImGui::Text(m_objectB.lock()->getName().c_str()) : ImGui::Text("Object B");
									setCondition();
									ImGui::Checkbox("Is interaction reversed if resolved", &m_isReverse);
								}
								else
								{
									ImNodes::BeginInputAttribute(it->getID());
									ImGui::Text("=> Receiver");
									ImNodes::EndInputAttribute();
									ImNodes::BeginOutputAttribute(it->getID() + 100000);
									ImGui::Indent(text_width);
									ImGui::Text("Sender =>");
									ImNodes::EndOutputAttribute();
								}
								ImGui::Dummy(sf::Vector2f(it->getTransform().getSize() / 10.f));
								if (!m_isOpen)
									ImNodes::SetNodeGridSpacePos(it->getID(), it->getTransform().getPosition());
								ImNodes::EndNode();
							}
						}
						ImNodes::BeginNode(objects->getID());
						ImNodes::BeginNodeTitleBar();
						ImGui::TextUnformatted(std::string(objects->getName()).c_str());
						ImNodes::EndNodeTitleBar();
						ImGui::Text(std::string("position x: " + std::to_string(objects->getTransform().getPosition().x)).c_str());
						ImGui::Text(std::string("position y: " + std::to_string(objects->getTransform().getPosition().y)).c_str());
						if (objects->getName() == "Event")
						{
							ImNodes::BeginInputAttribute(objects->getID());
							ImGui::Text("=> Receive information");
							ImNodes::EndInputAttribute();
							m_objectA.lock() ? ImGui::Text(m_objectA.lock()->getName().c_str()) : ImGui::Text("Object A");
							ImNodes::BeginOutputAttribute(objects->getID() + 100000);
							ImGui::Indent(text_width);
							ImGui::Text("Send information =>");
							ImNodes::EndOutputAttribute();
							m_objectB.lock() ? ImGui::Text(m_objectB.lock()->getName().c_str()) : ImGui::Text("Object B");
							setCondition();
							ImGui::Checkbox("Is interaction reversed if resolved", &m_isReverse);
						}
						else
						{
							ImNodes::BeginInputAttribute(objects->getID());
							ImGui::Text("=> Receiver");
							ImNodes::EndInputAttribute();
							ImNodes::BeginOutputAttribute(objects->getID() + 100000);
							ImGui::Indent(text_width);
							ImGui::Text("Sender =>");
							ImNodes::EndOutputAttribute();
						}
						ImGui::Dummy(sf::Vector2f(objects->getTransform().getSize() / 10.f));
						if (!m_isOpen)
							ImNodes::SetNodeGridSpacePos(objects->getID(), objects->getTransform().getPosition());
						ImNodes::EndNode();
					}
					ImNodes::MiniMap(0.25f, ImNodesMiniMapLocation_BottomRight);
					ImNodes::EndNodeEditor();

					int start(0), end(0);
					if (ImNodes::IsLinkCreated(&start, &end))
					{
						links.push_back({ linkid, {start,end} });
						linkid++;
						for (auto& objects : scene->getObjects())
						{
							while (objects->getObjects().size() != 0)
							{
								for (auto& it : objects->getObjects())
								{
									if (it != this->getParent())
									{
										if ((it->getID() == start || it->getID() + 100000 == start))
											m_objectA = it;
										if ((it->getID() == end || it->getID() + 100000 == end))
											m_objectB = it;
									}
								}
							}
							if (objects != this->getParent())
							{
								if ((objects->getID() == start || objects->getID() + 100000 == start))
									m_objectA = objects;
								if ((objects->getID() == end || objects->getID() + 100000 == end))
									m_objectB = objects;
							}
						}
					}
				}
				m_isOpen = true;
				ImGui::End();
			}
			else
			{
				m_isOpen = false;
			}
		};
}

void lc::Event::setCondition()
{
	const char* current_condition = "No condition";
	const char* values[] = { "Player collision", "Player interaction", "Player destruction", "No enemies alived" };
	if (ImGui::BeginCombo(std::string("Triger's condition##" + std::to_string(this->getID())).c_str(), m_condition == -1 ? current_condition : values[m_condition], ImGuiComboFlags_WidthFitPreview))
	{
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
		{
			bool is_selected_A = (current_condition == values[n]);
			if (ImGui::Selectable(values[n], is_selected_A))
			{
				m_condition = n;
				current_condition = values[n];
			}
		}
		ImGui::EndCombo();
	}
}