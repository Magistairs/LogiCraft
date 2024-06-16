
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

#include "Viewport.h"
#include "Convex.h"
#include "ToolsBar.h"
Viewport::Viewport() :
	m_name(""), //STRING
	m_zoom(1.f), m_movementSpeed(300.f), //FLOAT
	m_isOpen(true), m_hasWindowFocus(false), m_isZoomLocked(false), m_isMovementLocked(false), m_isViewportLocked(false), m_isCameraGrabbed(false) //BOOL
{
}

Viewport::Viewport(sf::Vector2f _start_position, sf::Vector2f _start_size, sf::Vector2u _window_size, std::string _name) :
	m_position(_start_position), m_size(_start_size), //VECTOR2
	m_name(_name), //STRING
	m_zoom(1.f), m_movementSpeed(300.f), //FLOAT
	m_isOpen(true), m_hasWindowFocus(false), m_isZoomLocked(false), m_isMovementLocked(false), m_isViewportLocked(false), m_isCameraGrabbed(false) //BOOL
{
	m_view.setCenter(_start_size / 2.f);
	//m_view.setSize(_start_size);
	//m_view.setViewport(sf::FloatRect(m_position.x / static_cast<float>(_window_size.x), m_position.y / static_cast<float>(_window_size.y), 1.f, 1.f));

	m_renderer.create(static_cast<unsigned>(_start_size.x), static_cast<unsigned>(_start_size.y));

	m_backgroundShape.setFillColor(sf::Color(14u, 14u, 14u, 200u));
}

Viewport::~Viewport()
{
}

bool Viewport::isInWindow(sf::Vector2f _pos)
{
	return Tools::Collisions::point_rect(_pos, { m_position + sf::Vector2f(1.f, 20.f), m_size });
}

ScreenZone::ScreenZone()
	: m_isUsed(false)
{
}

ScreenZone::ScreenZone(sf::Vector2i _screenIndex)
	: m_isUsed(false), m_screenIndex(_screenIndex)
{
	m_screenShape.setSize(SCREEN_SIZE);
	m_screenShape.setTexture(&GET_MANAGER->getTexture("Unused_Screen_Zone"), true);
	m_screenShape.setPosition(sf::Vector2f(SCREEN_SIZE.x * _screenIndex.x, SCREEN_SIZE.y * _screenIndex.y));
}

ScreenZone::~ScreenZone()
{
}

void ScreenZone::setScreenToUsed()
{
	m_screenShape.setTexture(&GET_MANAGER->getTexture("Used_Screen_Zone"), true);
	m_isUsed = true;
}

void ScreenZone::setScreenToUnused()
{
	m_screenShape.setTexture(&GET_MANAGER->getTexture("Unused_Screen_Zone"), true);
	m_isUsed = false;
}

Viewports::Viewports()
	: m_optionIsOpen(false), m_isMovementsLocked(false), m_isViewportsLocked(false), m_isZoomsLocked(false), m_hasGrabbedAnObject(false), m_wantToPlaceAnObject(false), 
	m_screenZoneUnused(SCREEN_LIMIT), m_actualSelectedObjectNumber(0u), m_selectedTimer(0.f), m_selection_rect_()
{
	ScreenZone tmp_SZ(sf::Vector2i(0, 0));
	this->setScreenZoneToUse(tmp_SZ);
	m_screenZones.push_back(tmp_SZ);

	m_selectionRect.setFillColor(sf::Color::Transparent);
	m_selectionRect.setOutlineThickness(3.f);
	m_selectionRect.setOutlineColor(sf::Color::Red);

	m_moveRect.setFillColor(sf::Color(255u, 255u, 0u, 100u));
	m_moveRect.setSize(sf::Vector2f(25.f, 25.f));
	m_moveRect.setOrigin(m_moveRect.getSize() / 2.f);
	m_moveRect.setOutlineThickness(3.f);
	m_moveRect.setOutlineColor(sf::Color::Yellow);
}

Viewports::~Viewports()
{
	m_viewports.clear();
	m_screenZones.clear();
}

void Viewports::UpdateEvent(sf::Event& _event)
{
	for (auto& viewport : m_viewports)
	{
		if (viewport->hasWindowFocus() && !viewport->isZoomLocked() && viewport->isInWindow(ImGui::GetMousePos()))
		{
			if (_event.type == sf::Event::MouseWheelScrolled)
			{
				if (_event.mouseWheelScroll.delta > 0.f && viewport->getZoom() > 0.2f)
				{
					viewport->getZoom() *= (1 / 1.1f);
				}
				else if (_event.mouseWheelScroll.delta < 0.f && viewport->getZoom() < 8.f)
				{
					viewport->getZoom() *= 1.1f;
				}
			}
		}
	}
}

void Viewports::Update(ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window)
{
	this->UpdateViewports(_selectedObject, _scene, _window);

	this->UpdateViewportOption();

	this->ViewportDeleteOnCloseBehavior();
}

void Viewports::Draw(ObjWeakPtrList& _object, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window)
{
	for (auto& viewport : m_viewports)
	{
		m_acutualUpdatedViewport = viewport;
		//I rescale the viewport to a normal point so the rendertexture dont do wierd thing.
		viewport->getView().setViewport({ 0.f, 0.f, 1.f, 1.f });

		viewport->getRenderTexture().clear(s_background_color);
		viewport->getRenderTexture().setView(viewport->getView());
		viewport->getBackgroundShape().setFillColor(s_background_color);
		viewport->getRenderTexture().draw(viewport->getBackgroundShape());
		this->DisplayScreenZones(viewport->getRenderTexture());
		for (int depth = static_cast<int>(ToolsBar::GetLayers().size()) - 1; depth >= 0; depth--)
		{
			_scene->Draw(viewport->getRenderTexture(), depth);
		}
		for (auto& it : points_convex)
		{
			viewport->getRenderTexture().draw(it);
		}
		if (!_object.empty())
		{
			//Find if the only selected object is the world then we not display the rect.
			if (std::find_if(_object.begin(), _object.end(), [&_scene](std::weak_ptr<lc::GameObject> selectedObject) {
				if (!selectedObject.expired())
					if (_scene == selectedObject.lock())
						return true;

				return false;
				}) == _object.end())
			{
				viewport->getRenderTexture().draw(m_selectionRect);
				viewport->getRenderTexture().draw(m_moveRect);
			}
		}
		viewport->getRenderTexture().display();

		//And then i reset the viewport to his right position so all the position are good.
		viewport->getView().setViewport({
		(viewport->getPosition().x + 1.f) / static_cast<float>(_window.getSize().x), (viewport->getPosition().y + 20.f) / static_cast<float>(_window.getSize().y),
		(viewport->getSize().x + 10.f) / static_cast<float>(_window.getSize().x), (viewport->getSize().y + 10.f) / static_cast<float>(_window.getSize().y) });
	}
}

void Viewports::addViewport(std::shared_ptr<Viewport> _viewport)
{
	_viewport->getName() += std::to_string(m_viewports.size());
	_viewport->isMovementLocked() = m_isMovementsLocked;
	_viewport->isViewportLocked() = m_isViewportsLocked;
	_viewport->isZoomLocked() = m_isZoomsLocked;
	m_viewports.push_back(_viewport);
}

void Viewports::setScreenZoneToUse(ScreenZone& _screenZone)
{
	m_screenZoneUnused--;
	_screenZone.setScreenToUsed();

	auto screenNeighborVerify = [this, &_screenZone](sf::Vector2i _index) {
		for (auto& screen : m_screenZones)
		{
			if (screen.getScreenIndex() == _screenZone.getScreenIndex() + _index)
			{
				return;
			}
		}

		m_screenZones.push_back(ScreenZone(_screenZone.getScreenIndex() + _index));
		};

	screenNeighborVerify(sf::Vector2i(0, -1));
	screenNeighborVerify(sf::Vector2i(0, 1));
	screenNeighborVerify(sf::Vector2i(-1, 0));
	screenNeighborVerify(sf::Vector2i(1, 0));
}

void Viewports::clearViewport()
{
	m_viewports.clear();
}

void Viewports::clearScreenZone()
{
	m_screenZones.clear();
	m_screenZoneUnused = SCREEN_LIMIT;
	ScreenZone tmp_SZ(sf::Vector2i(0, 0));
	this->setScreenZoneToUse(tmp_SZ);
	m_screenZones.push_back(tmp_SZ);

	m_selectionRect.setFillColor(sf::Color::Transparent);
	m_selectionRect.setOutlineThickness(3.f);
	m_selectionRect.setOutlineColor(sf::Color::Red);
}

bool& Viewports::isOptionOpen()
{
	return m_optionIsOpen;
}

void Viewports::UpdateViewports(ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window)
{
	for (auto& viewport : m_viewports)
	{
		m_acutualUpdatedViewport = viewport;
		ImGui::Begin(std::string("Viewport " + viewport->getName() + "##lc").c_str(), &viewport->isOpen(), viewport->isViewportLocked() ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None);
		{
			m_selectedTimer += Tools::getDeltaTime();

			viewport->hasWindowFocus() = ImGui::IsWindowFocused();
			sf::Vector2f mousePosition(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow())));
			sf::Vector2f mousePositionWithView(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), viewport->getView()));


			if (viewport->hasWindowFocus())
			{
				this->CreateConvex(_scene, mousePositionWithView);
				
				m_actualFocusViewport = viewport;

				this->UpdateScreenZones(viewport, _window);

				this->ObjectIsSelectedOrMovedBehavior(mousePosition, mousePositionWithView, _selectedObject, _scene, viewport, _window);
			}

			this->ResizeSelectionBoxBehavior(mousePositionWithView, _selectedObject, _scene, viewport, _window);

			this->ViewportMovement(mousePosition, viewport);

			viewport->getPosition() = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y};
			viewport->getSize() = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y};
			
			if (viewport->getLastSize() != viewport->getSize())
			{
				viewport->getRenderTexture().create(static_cast<unsigned>(ImGui::GetContentRegionAvail().x), static_cast<unsigned>(ImGui::GetContentRegionAvail().y));
				viewport->getLastSize() = viewport->getSize();
			}

			viewport->getView().setCenter(((viewport->getSize() / 2.f) + viewport->getAddedPosition()) * viewport->getZoom());
			viewport->getView().setSize(viewport->getSize() * viewport->getZoom());

			viewport->getBackgroundShape().setSize(viewport->getView().getSize());
			viewport->getBackgroundShape().setOrigin(viewport->getView().getSize() / 2.f);
			viewport->getBackgroundShape().setPosition(viewport->getView().getCenter());

			if (viewport->getSize().x < 0.f)
				viewport->getSize().x = 1.f;

			if (viewport->getSize().y < 0.f)
				viewport->getSize().y = 1.f;
		
			ImGui::Image(viewport->getRenderTexture(), ImGui::GetContentRegionAvail());
		}
		ImGui::End();
	}
}

void Viewports::UpdateViewportOption()
{
	if (m_optionIsOpen)
	{
		ImGui::Begin("Viewports Option##lc", &m_optionIsOpen, ImGuiWindowFlags_HorizontalScrollbar);
		{
			auto function = [this](std::function<void(std::shared_ptr<Viewport>&)> _f) { for (auto& viewport : m_viewports) { _f(viewport); } };

			ImGui::Text("All Viewport");

			//Check Box Only
			if (ImGui::Checkbox("Lock All Camera Movement", &m_isMovementsLocked))
				function([this](std::shared_ptr<Viewport>& _viewport) { _viewport->isMovementLocked() = m_isMovementsLocked; });

			if (ImGui::Checkbox("Lock All Camera Zoom", &m_isZoomsLocked))
				function([this](std::shared_ptr<Viewport>& _viewport) { _viewport->isZoomLocked() = m_isZoomsLocked; });

			if (ImGui::Checkbox("Lock All Viewport Window", &m_isViewportsLocked))
				function([this](std::shared_ptr<Viewport>& _viewport) { _viewport->isViewportLocked() = m_isViewportsLocked; });

			if (ImGui::Button("Close All Viewports"))
				function([this](std::shared_ptr<Viewport>& _viewport) { _viewport->isOpen() = false; });

			if (!m_actualFocusViewport.expired())
			{
				auto tmp_viewport = m_actualFocusViewport.lock();
				ImGui::Text(std::string("Selected Viewport" + tmp_viewport->getName()).c_str());

				ImGui::Checkbox("Lock Camera Movement", &tmp_viewport->isMovementLocked());

				ImGui::Checkbox("Lock Camera Zoom", &tmp_viewport->isZoomLocked());

				ImGui::Checkbox("Lock Viewport Window", &tmp_viewport->isViewportLocked());

				//Input Only
				ImGui::PushItemWidth(100.f);
				ImGui::InputFloat("Camera Movement Speed", &tmp_viewport->getMovementSpeed());

				ImGui::PushItemWidth(100.f);
				ImGui::InputFloat("Camera Position X", &tmp_viewport->getAddedPosition().x);

				ImGui::SameLine();
				ImGui::PushItemWidth(100.f);
				ImGui::InputFloat("Camera Position Y", &tmp_viewport->getAddedPosition().y);

				if (ImGui::Button("Unselect Viewport"))
					m_actualFocusViewport.reset();
			}
			else
			{
				ImGui::Text("No Viewport Selected");
			}
		}
		ImGui::End();
	}
}

void Viewports::UpdateScreenZones(std::shared_ptr<Viewport>& _viewport, WindowManager& _window)
{
	for (auto& screen : m_screenZones)
	{
		if (KEY(LControl) && MOUSE(Left) && !screen.isUsed() && m_screenZoneUnused >= 0 && !m_wantToPlaceAnObject)
		{
			if (_viewport->isInWindow(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()))))
				if (Tools::Collisions::point_rect(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), _viewport->getView()), sf::FloatRect(screen.getScreenShape().getPosition() + sf::Vector2f(1386.f, 548.f), sf::Vector2f(1080.f, 1080.f))))
					this->setScreenZoneToUse(screen);
		}
		else if (KEY(LControl) && MOUSE(Right) && screen.isUsed() && screen.getScreenIndex() != sf::Vector2i() && !m_wantToPlaceAnObject)
		{
			if (_viewport->isInWindow(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()))))
				if (Tools::Collisions::point_rect(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), _viewport->getView()), sf::FloatRect(screen.getScreenShape().getPosition() + sf::Vector2f(1386.f, 548.f), sf::Vector2f(1080.f, 1080.f))))
				{
					this->DeleteScreen(screen);
					break;
				}
		}
	}
}

void Viewports::DisplayScreenZones(sf::RenderTexture& _renderer)
{
	for (auto& screen : m_screenZones)
		if (screen.isUsed() || (!screen.isUsed() && m_screenZoneUnused >= 0))
			_renderer.draw(screen.getScreenShape());
}

void Viewports::DeleteScreen(ScreenZone& _screenZone)
{
	//Function to delete the useless unused screen zone that as been created by the old used screen.
	auto function = [this, &_screenZone](sf::Vector2i _index, sf::Vector2i _sideIndexOne, sf::Vector2i _sideIndexTwo) {
		for (auto screen = m_screenZones.begin(); screen != m_screenZones.end();)
		{
			//Here we will find the screen from the given index.
			if (screen->getScreenIndex() == _screenZone.getScreenIndex() + _index)
			{
				auto tmp_screen = std::find_if(m_screenZones.begin(), m_screenZones.end(), [&screen, &_index, &_sideIndexOne, &_sideIndexTwo](ScreenZone& _screen)
					{
						//This will find if the unused screen that need to be deleted is used by another used screen zone to not delete it.
						if ((_screen.getScreenIndex() == screen->getScreenIndex() + _sideIndexOne || _screen.getScreenIndex() == screen->getScreenIndex() + _sideIndexTwo || _screen.getScreenIndex() == screen->getScreenIndex() + _index))
							if (_screen.isUsed())
								return true;

						//Else it will be delete.
						return false;
					});

				//If the screen that as been return is not used by another used screen zone
				//And is not an used screen.
				if (tmp_screen == m_screenZones.end() && !screen->isUsed())
					screen = m_screenZones.erase(screen);
				else
					screen++;
			}
			else
			{
				screen++;
			}
		}
		};

	//And then the function is call to see all the side to the screen zone that need to be deleted.
	function(sf::Vector2i(0, -1), sf::Vector2i(-1, 0), sf::Vector2i(1, 0));
	function(sf::Vector2i(0, 1), sf::Vector2i(-1, 0), sf::Vector2i(1, 0));
	function(sf::Vector2i(-1, 0), sf::Vector2i(0, -1), sf::Vector2i(0, 1));
	function(sf::Vector2i(1, 0), sf::Vector2i(0, -1), sf::Vector2i(0, 1));

	bool hasNoUsedNeightbor(true);

	m_screenZoneUnused++;
	_screenZone.setScreenToUnused();

	//And if at the end the used screen that as been set to unused and the screen around him are unused too he will be deleted.
	for (auto& screen : m_screenZones)
	{
		if (screen.getScreenIndex() == _screenZone.getScreenIndex() + sf::Vector2i(0, -1) ||
			screen.getScreenIndex() == _screenZone.getScreenIndex() + sf::Vector2i(0, 1) ||
			screen.getScreenIndex() == _screenZone.getScreenIndex() + sf::Vector2i(-1, 0) ||
			screen.getScreenIndex() == _screenZone.getScreenIndex() + sf::Vector2i(1, 0))
			if (screen.isUsed())
			{
				hasNoUsedNeightbor = false;
				break;
			}
	}

	if (hasNoUsedNeightbor)
	{
		m_screenZones.remove_if([&_screenZone](ScreenZone& _screen) {
			return _screenZone.getScreenIndex() == _screen.getScreenIndex();
			});
	}
}


void Viewports::ViewportMovement(sf::Vector2f _mousePosition, std::shared_ptr<Viewport> _viewport)
{
	if (!_viewport->isMovementLocked())
	{
		if (_viewport->hasWindowFocus())
		{
			if (KEY(Z) || KEY(Up))
				_viewport->getAddedPosition().y -= _viewport->getMovementSpeed() * Tools::getDeltaTime();
			if (KEY(S) || KEY(Down))
				_viewport->getAddedPosition().y += _viewport->getMovementSpeed() * Tools::getDeltaTime();
			if (KEY(Q) || KEY(Left))
				_viewport->getAddedPosition().x -= _viewport->getMovementSpeed() * Tools::getDeltaTime();
			if (KEY(D) || KEY(Right))
				_viewport->getAddedPosition().x += _viewport->getMovementSpeed() * Tools::getDeltaTime();
		}

		if (MOUSE(Right))
		{
			if (Tools::Collisions::point_rect(_mousePosition, sf::FloatRect(_viewport->getPosition(), _viewport->getSize())) && !_viewport->isCameraGrabbed() && !Tools::camera_grabbed)
			{
				if (!Tools::IG::MouseIsOnAboveWindow())
				{
					Tools::camera_grabbed = true;
					_viewport->isCameraGrabbed() = true;
					_viewport->getLastAddedPosition() = _viewport->getAddedPosition();
					ImGui::SetWindowFocus();
				}
			}
			else if (_viewport->isCameraGrabbed())
			{
				_viewport->getAddedPosition() = -sf::Vector2f(ImGui::GetMouseDragDelta(ImGuiMouseButton_Right)) + _viewport->getLastAddedPosition();
			}
		}
		else
		{
			Tools::camera_grabbed = false;
			_viewport->isCameraGrabbed() = false;
		}
	}
}

void Viewports::ViewportDeleteOnCloseBehavior()
{
	m_viewports.remove_if([](auto& _viewport)
		{
			return !_viewport->isOpen();
		});
}

void Viewports::ResizeSelectionBoxBehavior(sf::Vector2f _mousePositionWithView, ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, std::shared_ptr<Viewport> _viewport, WindowManager& _window)
{
	if (!MOUSE(Left) && m_hasGrabbedAnObject)
		m_hasGrabbedAnObject = false;

	if (!_selectedObject.empty())
	{
		if ((m_actualSelectedObjectNumber != _selectedObject.size() || _selectedObject.size() == 1) || m_hasGrabbedAnObject)
		{
			if (m_hasGrabbedAnObject)
			{
				for (auto& selectedObject : _selectedObject)
					if (!selectedObject.expired())
						if (!selectedObject.lock()->isLock())
						{
							if (ToolsBar::GetActualLayer().second == "Player Plan" && selectedObject.lock()->getDepth() == 4 && selectedObject.lock()->getName() != PLAYER_NAME)
							{
								selectedObject.lock()->getTransform().getPosition() = 
									sf::Vector2f((int)(_mousePositionWithView.x + selectedObject.lock()->getTransform().getDistance().x) / 120 * 120,
										(int)(_mousePositionWithView.y + selectedObject.lock()->getTransform().getDistance().y) / 120 * 120);
							}
							else
							{
								selectedObject.lock()->getTransform().getPosition() = _mousePositionWithView + selectedObject.lock()->getTransform().getDistance();
							}
						}
			}

			m_maxSelectedObjectPosition = sf::Vector2f(INFINITY, INFINITY);
			m_maxSelectedObjectSize = sf::Vector2f(-INFINITY, -INFINITY);
			
			this->ResizeRectSelection(_selectedObject, _scene);

			m_actualSelectedObjectNumber = static_cast<unsigned>(_selectedObject.size());
		}

		if (_selectedObject.size() == 1)
		{
			if (!_selectedObject.front().expired())
				m_moveRect.setPosition(m_maxSelectedObjectPosition + _selectedObject.front().lock()->getTransform().getOrigin());
		}
		else
			m_moveRect.setPosition(m_maxSelectedObjectPosition);

		m_moveRect.setScale(sf::Vector2f(_viewport->getZoom(), _viewport->getZoom()));
		m_selectionRect.setPosition(m_maxSelectedObjectPosition);
		m_selectionRect.setSize(m_maxSelectedObjectSize);
	}
}

void Viewports::ObjectIsSelectedOrMovedBehavior(sf::Vector2f _mousePosition, sf::Vector2f _mousePositionWithView, ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, std::shared_ptr<Viewport> _viewport, WindowManager& _window)
{
	if (MOUSE(Left) && !m_wantToPlaceAnObject && !m_hasGrabbedAnObject)
	{
		if (Tools::Collisions::point_rect(_mousePositionWithView, m_moveRect.getGlobalBounds()) && !m_wantToPlaceAnObject)
		{
			if (!_selectedObject.empty())
			{
				for (auto& selectedObject : _selectedObject)
					if (!selectedObject.expired())
						selectedObject.lock()->getTransform().getDistance() = sf::Vector2f(selectedObject.lock()->getTransform().getPosition() - _mousePositionWithView);

				m_hasGrabbedAnObject = true;
			}
		}
		else if (!m_hasGrabbedAnObject && m_selectedTimer > 0.1f)
		{
			if (Tools::Collisions::point_rect(_mousePosition, { _viewport->getPosition(), _viewport->getSize() }) && !m_wantToPlaceAnObject)
				if (this->CheckSelection(_selectedObject, _scene, _mousePosition, _viewport, _window))
					m_selectedTimer = 0.f;
		}
	}
}

bool Viewports::CheckSelection(ObjWeakPtrList& _object, std::shared_ptr<lc::GameObject> _scene, sf::Vector2f _mousePosition, std::shared_ptr<Viewport>& _viewport, WindowManager& _window)
{
	if (std::find_if(_scene->getObjects().begin(), _scene->getObjects().end(), [this, &_window, &_object, &_viewport, _mousePosition, &_scene](std::shared_ptr<lc::GameObject> _sceneObject)
		{
			if (Tools::Collisions::point_rect(_window.getWindow().mapPixelToCoords(sf::Mouse::getPosition(_window.getWindow()), _viewport->getView()),
				{ _sceneObject->getTransform().getPosition() - _sceneObject->getTransform().getOrigin() + lc::GameObject::GetOffset(lc::GameObject::GetRoot(_sceneObject),_sceneObject->getDepth()), _sceneObject->getTransform().getSize()})
				&& _sceneObject->getDepth() == ToolsBar::GetActualLayer().first)
			{
				if (!KEY(LControl))
					_object.clear();

				if (std::find_if(_object.begin(), _object.end(), [&_sceneObject](std::weak_ptr<lc::GameObject> _wptrObject)
					{
						if (!_wptrObject.expired())
							if (_sceneObject == _wptrObject.lock())
								return true;

						return false;
					}) == _object.end())
					_object.push_back(_sceneObject);

					m_maxSelectedObjectPosition = sf::Vector2f(INFINITY, INFINITY);
					m_maxSelectedObjectSize = sf::Vector2f(-INFINITY, -INFINITY);

					return true;
			}
			else
			{
				if (!_sceneObject->getObjects().empty())
					return this->CheckSelection(_object, _sceneObject, _mousePosition, _viewport, _window);
				else
					return false;
			}
		}) == _scene->getObjects().end())
	{
		_object.clear();
		return false;
	}

	return true;
}

void Viewports::ResizeRectSelection(ObjWeakPtrList& _object, std::shared_ptr<lc::GameObject> _scene)
{
	for (auto& selectedObject : _object)
	{
		if (!selectedObject.expired())
		{
			if (_scene != selectedObject.lock())
			{
				if (m_maxSelectedObjectPosition.x > selectedObject.lock()->getTransform().getPosition().x - selectedObject.lock()->getTransform().getOrigin().x)
					m_maxSelectedObjectPosition.x = selectedObject.lock()->getTransform().getPosition().x - selectedObject.lock()->getTransform().getOrigin().x + lc::GameObject::GetOffset(lc::GameObject::GetRoot(selectedObject.lock()),selectedObject.lock()->getDepth()).x;

				if (m_maxSelectedObjectPosition.y > selectedObject.lock()->getTransform().getPosition().y - selectedObject.lock()->getTransform().getOrigin().y)
					m_maxSelectedObjectPosition.y = selectedObject.lock()->getTransform().getPosition().y - selectedObject.lock()->getTransform().getOrigin().y + lc::GameObject::GetOffset(lc::GameObject::GetRoot(selectedObject.lock()),selectedObject.lock()->getDepth()).y;
			}
		}
	}

	for (auto& selectedObject : _object)
	{
		if (!selectedObject.expired())
		{
			if (_scene != selectedObject.lock())
			{
				if (m_maxSelectedObjectPosition.x + m_maxSelectedObjectSize.x < (selectedObject.lock()->getTransform().getPosition().x - selectedObject.lock()->getTransform().getOrigin().x) + selectedObject.lock()->getTransform().getSize().x)
					m_maxSelectedObjectSize.x = ((selectedObject.lock()->getTransform().getPosition().x - selectedObject.lock()->getTransform().getOrigin().x) + selectedObject.lock()->getTransform().getSize().x) +lc::GameObject::GetOffset(lc::GameObject::GetRoot(selectedObject.lock()),selectedObject.lock()->getDepth()).x - m_maxSelectedObjectPosition.x;

				if (m_maxSelectedObjectPosition.y + m_maxSelectedObjectSize.y < (selectedObject.lock()->getTransform().getPosition().y - selectedObject.lock()->getTransform().getOrigin().y) + selectedObject.lock()->getTransform().getSize().y)
					m_maxSelectedObjectSize.y = ((selectedObject.lock()->getTransform().getPosition().y - selectedObject.lock()->getTransform().getOrigin().y) + selectedObject.lock()->getTransform().getSize().y) +lc::GameObject::GetOffset(lc::GameObject::GetRoot(selectedObject.lock()),selectedObject.lock()->getDepth()).y  - m_maxSelectedObjectPosition.y;
			}
		}
	}
}

void Viewports::CreateConvex(std::shared_ptr<lc::GameObject> _scene, sf::Vector2f mousePosition)
{
	timer += Tools::getDeltaTime();
	if (KEY(Tab) && MOUSE(Left) && timer > 0.25f)
	{
		container.push_back(mousePosition);
		sf::RectangleShape tmp({ 12.5f, 12.5f });
		tmp.setFillColor(sf::Color::Red);
		tmp.setOrigin(6.25f, 6.25f);
		tmp.setPosition(mousePosition);
		points_convex.push_back(tmp);
		timer = 0.f;
	}
	if (KEY(Space) && timer > 0.5f and !points_convex.empty())
	{
		points_convex.clear();
		auto convex = lc::Convex(container);
		auto object = _scene->addObject(convex.getName(), ToolsBar::GetActualLayer().first);
		object->getTransform().getPosition() = convex.getPosition();
		object->addComponent<lc::Convex>(convex);
		timer = 0.f;
	}
}