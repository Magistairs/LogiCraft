
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

#include "BrowserContent.h"

#include "Event.h"
#include "Font.h"
#include "TaskManager.h"
#include "Texture.h"
#include "ToolsBar.h"

#define BUTTON_SIZE 100.f

BrowserContent::BrowserContent() : m_mainFolder("../ASSETS") {  };

void BrowserContent::Update(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport)
{
	bool bReloadRequested = false; // to replace by a proper event queue

	ImGui::Begin("Browser Content##lc", 0, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/);
	ImGui::SetWindowSize(ImVec2(1920.f, 500.f), ImGuiCond_FirstUseEver);
	ImGui::SetWindowPos(ImVec2(0.f, 580.f), ImGuiCond_FirstUseEver);
	if (m_isLoading)
	{
		ImGui::Text("Loading");
		ImGui::End();
		return;
	}
	ImVec2 vec = ImVec2(75.f, 17.5f);
	if (!m_selectedFolder.empty())
	{
		if (ImGui::Button("New", ImVec2(40, 20)) and ImGui::IsWindowFocused())
			m_creatingNewRessources = true;

		SameLine(0, 10.f);
		if (ImGui::Button(m_sortString.c_str(), ImVec2(70, 20)))
		{
			m_selectingFilter = true;
		}
		SameLine(0, 10.f);

		bReloadRequested = ImGui::Button("Reload Content Browser");
		ImGui::SameLine();
		ImVec2 buttonsize(70, 20);
		auto pos = sf::Vector2f(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x - buttonsize.x, ImGui::GetCursorPos().y + ImGui::GetWindowPos().y);
		ImGui::NewLine();
		if (m_selectingFilter)
		{
			Tools::IG::CreateWindow("Filter", m_selectingFilter, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse, [&] {
				Tools::IG::SetSpacing(ImVec2(0, 0), [&]
					{
						ImGui::SetWindowPos(pos);
						if (ImGui::Button("ALL", buttonsize)) { m_sortString = "ALL"; m_sortedRessources = lc::Ressource::TYPE::NONE; m_selectingFilter = false; }
						if (ImGui::Button("TEXTURE", buttonsize)) { m_sortString = "TEXTURE"; m_sortedRessources = lc::Ressource::TYPE::TEXTURE; m_selectingFilter = false; }
						if (ImGui::Button("IA", buttonsize)) { m_sortString = "IA"; m_sortedRessources = lc::Ressource::TYPE::IA; m_selectingFilter = false; }
						if (ImGui::Button("ANIMATION", buttonsize)) { m_sortString = "ANIMATION"; m_sortedRessources = lc::Ressource::TYPE::ANIMATION; m_selectingFilter = false; }
						if (ImGui::Button("SOUND", buttonsize)) { m_sortString = "SOUND"; m_sortedRessources = lc::Ressource::TYPE::SOUND;  m_selectingFilter = false; }
						if (ImGui::Button("MUSIC", buttonsize)) { m_sortString = "MUSIC"; m_sortedRessources = lc::Ressource::TYPE::MUSIC; m_selectingFilter = false; }
						if (ImGui::Button("BUTTON", buttonsize)) { m_sortString = "BUTTON"; m_sortedRessources = lc::Ressource::TYPE::BUTTON; m_selectingFilter = false; }
						if (ImGui::Button("FONT", buttonsize)) { m_sortString = "FONT"; m_sortedRessources = lc::Ressource::TYPE::FONT; m_selectingFilter = false; }
						if (ImGui::Button("EVENT", buttonsize)) { m_sortString = "EVENT"; m_sortedRessources = lc::Ressource::TYPE::EVENT; m_selectingFilter = false; }

					});
				});

		}

	}
	ImGui::BeginChild("Folder window", ImVec2(150, 0), 1, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	ImGui::BeginGroup();
	Tools::IG::SetSpacing({ 0.f,5.f }, [&vec, this] {
		m_mainFolder.update(m_selectedFolder);
		});
	ImGui::EndGroup();

	ImGui::EndChild();
	{
		Tools::IG::SetSpacing({ 0.f,7.5f }, [&] {
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::SameLine();
			ImGui::BeginChild("Content window", ImVec2(0, 0), 1);
			ImGui::BeginGroup();
			if (!m_selectedFolder.empty())
			{
				const std::filesystem::path path{ m_selectedFolder };
				ImVec2 button_sz(BUTTON_SIZE, BUTTON_SIZE);
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
				//m_display.windowVisibleX = window_visible_x2;
				//m_display.initialPosition = { ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,  ImGui::GetWindowPos().y + ImGui::GetCursorPos().y };
				//m_display.size = button_sz;
				//m_display.spacing = { 7.5f, style.ItemSpacing.y };

				for (auto const& dir_entry : m_mainFolder.getFolder(m_selectedFolder).getFolderRessources())
				{
					if (m_sortedRessources == dir_entry->m_type or m_sortedRessources == lc::Ressource::TYPE::NONE)
					{

						float last_button_x2 = ImGui::GetItemRectMax().x;
						float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line

						if (next_button_x2 < window_visible_x2)
							ImGui::SameLine(0.f, 7.5f);
						sf::Sprite tmp(*dir_entry->getShape().getTexture());
						tmp.setTextureRect(dir_entry->getShape().getTextureRect());
						//m_display.position = ImGui::GetItemRectMin();
						ImGui::PushStyleColor(ImGuiCol_BorderShadow, dir_entry->getFilterColor());
						ImGui::PushStyleColor(ImGuiCol_Border, dir_entry->getFilterColor());
						ImGui::GetStyle().FrameBorderSize = 2.f;
						if (ImGui::ImageButton(dir_entry->getName().c_str(), tmp, button_sz))
						{
						}
						ImGui::GetStyle().FrameBorderSize = 0.f;
						ImGui::PopStyleColor(2);
						if (ImGui::IsWindowFocused())
						{
							if (!m_slidingRessources and MOUSE(Left))
							{
								//auto initPos = m_display.position;
								if (Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), { ImGui::GetItemRectMin(),ImGui::GetItemRectSize() }))
								{
									m_slidingRessources = &*dir_entry;
									m_newObjectName = dir_entry->getName();
									auto t = dir_entry.get();
									m_isSlidingRessources = true;
									_viewport.wantToPlaceAnObject() = true;
								}
								//m_display.position = initPos;
							}
						}
					}
				}
				PutRessources(_object, _window, _viewport);
			}
			ImGui::EndGroup();
			ImGui::EndChild();
			});
	}
	ImGui::End();

	if (m_creatingNewRessources)
	{
		CreateRessources();
	}

	if(bReloadRequested)
		Reload();
}

void BrowserContent::Draw(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport)
{
	if (!m_selectedFolder.empty())
	{
		if (m_slidingRessources)
		{
			DrawSlidingRessources(_object, _window, _viewport);
		}
	}
}

void BrowserContent::CreateRessources()
{
	if (m_selectedRessourceType == lc::Ressource::TYPE::NONE)
	{
		Tools::IG::CreateWindow("New Ressources", m_creatingNewRessources, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize, [&]() {
			if (ImGui::Button("Repository"))
			{
				m_mainFolder.getFolder(m_selectedFolder).getNewFolder(true);
				m_creatingNewRessources = false;
			}
			if (ImGui::Button("Texture"))
			{
				m_selectedRessourceType = lc::Ressource::TYPE::TEXTURE;
			}
			if (ImGui::Button("Font"))
			{
				m_selectedRessourceType = lc::Ressource::TYPE::FONT;
			}
			});
	}
	else
	{
		switch (m_selectedRessourceType)
		{
		case lc::Ressource::TYPE::TEXTURE:
			CreateTexture();
			break;
		case lc::Ressource::TYPE::FONT:
			CreateFont();
			break;
		}
	}

	if (!m_creatingNewRessources)
	{
		m_selectedRessourceType = lc::Ressource::TYPE::NONE;
	}
}

void BrowserContent::CreateTexture()
{
	Tools::IG::CreateWindow("Texture", m_creatingNewRessources, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize, [&]() {
		if (!m_selectedRessources.empty())
			ImGui::Text(m_selectedRessources.c_str());
		if (Button("Load Texture"))
		{
			Tools::IG::LoadRessourcesFromFile(m_selectedRessources, "png,jpg");
			auto t = sf::Texture();
			t.loadFromFile(m_selectedRessources);
			m_newTextureRect = sf::IntRect(0, 0, t.getSize().x, t.getSize().y);
			m_RessourceName = std::filesystem::path(m_selectedRessources).filename().stem().string();
		}
		ImGui::InputText("Texture Name", m_RessourceName, 100);
		ImGui::InputInt4("Texture Rect", m_newTextureRect);
		if (ImGui::Button("Create") and m_RessourceName[0] != '\0' and m_newTextureRect != sf::IntRect() and m_selectedRessources[0] != '\0')
		{
			m_mainFolder.getFolder(m_selectedFolder).AddRessources(std::make_unique<lc::Texture>(m_RessourceName, m_newTextureRect, m_selectedRessources, m_mainFolder.getFolder(m_selectedFolder).getPath().string()));
			m_creatingNewRessources = false;
			m_RessourceName.clear();
			m_selectedRessources.clear();
			m_newTextureRect = sf::IntRect();
			m_selectedRessourceType = lc::Ressource::TYPE::NONE;
		}
		});
}

void BrowserContent::CreateFont()
{
	Tools::IG::CreateWindow("Font", m_creatingNewRessources, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize, [&]() {
		if (!m_selectedRessources.empty())
			ImGui::Text(m_selectedRessources.c_str());
		if (Button("Load Font"))
		{
			Tools::IG::LoadRessourcesFromFile(m_selectedRessources, "ttf,otf");
			m_RessourceName = std::filesystem::path(m_selectedRessources).filename().stem().string();
		}
		ImGui::InputText("Font Name", m_RessourceName, 100);
		if (ImGui::Button("Create") and m_RessourceName[0] != '\0' and m_selectedRessources[0] != '\0')
		{
			m_mainFolder.getFolder(m_selectedFolder).AddRessources(std::make_unique<lc::Font>(m_RessourceName, m_selectedRessources));
			m_creatingNewRessources = false;
			m_RessourceName.clear();
			m_selectedRessources.clear();
			m_newTextureRect = sf::IntRect();
			m_selectedRessourceType = lc::Ressource::TYPE::NONE;
		}
		});
}

void BrowserContent::CreateEvent()
{
	Tools::IG::CreateWindow("Event", m_creatingNewRessources, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize, [&]() {
		ImGui::InputText("Event Name", m_RessourceName, 100);
		if (ImGui::Button("Create") and m_RessourceName[0] != '\0')
		{
			m_mainFolder.getFolder(m_selectedFolder).AddRessources(std::make_unique<lc::Event>(m_RessourceName, -1));
			m_creatingNewRessources = false;
			m_RessourceName.clear();
			m_selectedRessources.clear();
			m_newTextureRect = sf::IntRect();
			m_selectedRessourceType = lc::Ressource::TYPE::NONE;
		}
		});

}

void BrowserContent::PutRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport)
{
	if (m_slidingRessources)
	{
		if (!MOUSE(Left) && !KEY(LControl))
			m_choosingName = true;
		else if (!MOUSE(Left) && KEY(LControl))
			m_nameIsReady = true;
		else
		{
			for (auto& viewport : _viewport.getAllViewport())
			{
				if (Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), { viewport->getPosition(), viewport->getSize() }) and m_slidingRessources && !m_choosingName)
				{
					ImGui::Begin(std::string("Viewport " + viewport->getName() + "##lc").c_str(), &viewport->isOpen(), viewport->isViewportLocked() ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None);

					bool isOnAnotherWindow(false);
					auto tmp_currentWindow = ImGui::GetCurrentContext()->CurrentWindow;

					for (auto& window : ImGui::GetCurrentContext()->Windows)
						if (window->ID != tmp_currentWindow->ID)
							if (Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), { window->Pos, window->Size }) &&
								ImGui::IsWindowAbove(window, tmp_currentWindow) &&
								std::string(window->Name).find("##lc") != std::string::npos)
							{
								isOnAnotherWindow = true;
								break;
							}

					if (!isOnAnotherWindow)
						SetWindowFocus();

					ImGui::End();
				}
			}
			_viewport.wantToPlaceAnObject() = true;
		}
	}
	if (m_choosingName)
	{
		if (m_slidingRessources->getTypeName() != "Event")
			this->MakeRessourcesName(_object);
		else
			m_nameIsReady = true;
	}
	if (m_nameIsReady)
	{
		for (auto& viewport : _viewport.getAllViewport())
		{
			if (Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), { viewport->getPosition(), viewport->getSize() }) and m_slidingRessources)
			{
				auto selectedObject = _object;

				if (KEY(LControl))
					for (auto& i : _object->getObjects())
					{
						if (Tools::Collisions::point_rect(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), viewport->getView()), { i->getTransform().getPosition(), i->getTransform().getSize() }))
						{
							selectedObject = i;
							break;
						}
					}

				if (selectedObject == _object)
				{
					auto object = selectedObject->addObject(m_newObjectName, ToolsBar::GetActualLayer().first);
					AddComponent(object, _window, *viewport);
					if (ToolsBar::GetActualLayer().second == "Player Plan")
					{
						m_putPosition.x = static_cast<float>((int)m_putPosition.x / 120);
						m_putPosition.y = static_cast<float>((int)m_putPosition.y / 120);
						object->getTransform().getPosition() = (sf::Vector2f(m_putPosition.x * 120, m_putPosition.y * 120));
					}
					else
						object->getTransform().getPosition() = m_putPosition - object->getTransform().getSize() / 2.f;
					_viewport.wantToPlaceAnObject() = false;
				}
				else
				{
					AddComponent(selectedObject, _window, *viewport);
					_viewport.wantToPlaceAnObject() = false;
				}
			}
		}
		m_slidingRessources = nullptr;
		m_isSlidingRessources = false;
		m_choosingName = false;
		m_nameIsReady = false;
	}
}

void BrowserContent::MakeRessourcesName(std::shared_ptr<lc::GameObject> _scene)
{
	ImGui::Begin("Choose a name for the object", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::InputText("Change Name", m_newObjectName, 150, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		Tools::ReplaceCharacter(m_newObjectName, ' ', '_');
		if (!this->CheckObjectName(_scene, m_newObjectName))
			m_nameIsReady = true;
		else
			m_showError = true;
	}
	if (m_showError)
		ImGui::Text("Name already existing, change it !");
	ImGui::End();
}

bool BrowserContent::CheckObjectName(std::shared_ptr<lc::GameObject> _obj, std::string _name)
{
	if (_obj->getName() == _name)
		return true;

	for (auto& object : _obj->getObjects())
	{
		if (this->CheckObjectName(object, _name))
			return true;
	}

	return false;
}

void BrowserContent::AddComponent(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewport& _viewport)
{
	if (m_slidingRessources->m_type == lc::Ressource::TYPE::TEXTURE)
	{
		auto ressources = _object->addComponent<lc::Texture>(*dynamic_cast<lc::Texture*>(m_slidingRessources));
		if (ressources->getThumbnail())
			ressources->ToggleThumbnail();
		_window.getWindow().setView(_viewport.getView());

		_window.getWindow().setView(_window.getWindow().getDefaultView());
	}
	else if (m_slidingRessources->m_type == lc::Ressource::TYPE::FONT)
	{
		auto ressources = _object->addComponent<lc::Font>(*dynamic_cast<lc::Font*>(m_slidingRessources));
		if (ressources->getThumbnail())
			ressources->ToggleThumbnail();
		_window.getWindow().setView(_viewport.getView());
		ressources->setPosition(_window.getMousePos<sf::Vector2f>() - ressources->getShape().getSize() / 2.f);
		_window.getWindow().setView(_window.getWindow().getDefaultView());
	}
	else if (m_slidingRessources->m_type == lc::Ressource::TYPE::EVENT)
	{
		auto ressources = _object->addComponent<lc::Event>(*dynamic_cast<lc::Event*>(m_slidingRessources));
		if (ressources->getThumbnail())
			ressources->ToggleThumbnail();
		_window.getWindow().setView(_viewport.getView());
		ressources->setPosition(_window.getMousePos<sf::Vector2f>() - ressources->getShape().getSize() / 2.f);
		_window.getWindow().setView(_window.getWindow().getDefaultView());
	}
	_object->Update(_window);
}

void BrowserContent::DrawSlidingRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport)
{
	bool alreadyDraw(false);
	sf::Vector2f pos;
	for (auto it = _viewport.getAllViewport().begin(); it != _viewport.getAllViewport().end();)
	{
		if (!alreadyDraw)
		{
			if (Tools::Collisions::point_rect(_window.getMousePos<sf::Vector2f>(), { (*it)->getPosition(),(*it)->getSize() }))
			{
				alreadyDraw = true;
				_window.getWindow().setView((*it)->getView());
				if (!m_choosingName)
				{
					m_putPosition = _window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), (*it)->getView());
					if (m_slidingRessources->m_type == lc::Ressource::TYPE::TEXTURE)
						m_slidingRessources->setPosition(m_putPosition - dynamic_cast<lc::Texture*>(m_slidingRessources)->getTextureSize() / 2.f);
					else if (m_slidingRessources->m_type == lc::Ressource::TYPE::FONT)
						m_slidingRessources->setPosition(m_putPosition - dynamic_cast<lc::Font*>(m_slidingRessources)->getShape().getSize() / 2.f);
				}
				_window.getWindow().setView(_window.getWindow().getDefaultView());
				it = _viewport.getAllViewport().begin();
			}
			else
			{
				it++;
			}
		}
		else
		{
			_window.getWindow().setView((*it)->getView());
			if (m_slidingRessources->getThumbnail())
				m_slidingRessources->ToggleThumbnail();
			if (!Tools::IG::MouseIsOnAboveWindow(ImGui::FindWindowByName(std::string("Viewport " + (*it)->getName() + "##lc").c_str())))
				m_slidingRessources->Draw((*it)->getRenderTexture());
			if (!m_slidingRessources->getThumbnail())
				m_slidingRessources->ToggleThumbnail();
			_window.getWindow().setView(_window.getWindow().getDefaultView());
			it++;
		}
	}
}

void BrowserContent::Reload()
{
	TaskManager::Get().AddTask([this]
		{
			m_isLoading = true;
			m_mainFolder.reload();
			m_isLoading = false;
		});
}
