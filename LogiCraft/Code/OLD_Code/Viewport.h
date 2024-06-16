
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
#include <functional>

#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

#include "tools_imgui.h"
#include "GameObject.h"

const sf::Vector2f SCREEN_SIZE(3840.f, 2160.f);
constexpr unsigned short SCREEN_LIMIT = 65535;

class Viewport
{
public:
	Viewport();
	Viewport(sf::Vector2f _start_position, sf::Vector2f _start_size, sf::Vector2u _window_size, std::string _name);
	~Viewport();

	sf::View& getView() { return m_view; }

	sf::RectangleShape& getBackgroundShape() { return m_backgroundShape; }

	sf::RenderTexture& getRenderTexture() { return m_renderer; }

	std::string& getName() { return m_name; }

	sf::Vector2f& getPosition() { return m_position; }
	sf::Vector2f& getAddedPosition() { return m_addedPosition; }
	sf::Vector2f& getLastAddedPosition() { return m_lastAddedPosition; }
	sf::Vector2f& getSize() { return m_size; }
	sf::Vector2f& getLastSize() { return m_lastSize; }

	bool& isOpen() { return m_isOpen; }
	bool& hasWindowFocus() { return m_hasWindowFocus; }
	bool& isMovementLocked() { return m_isMovementLocked; }
	bool& isZoomLocked() { return m_isZoomLocked; }
	bool& isViewportLocked() { return m_isViewportLocked; }
	bool& isCameraGrabbed() { return m_isCameraGrabbed; }
	float& getZoom() { return m_zoom; }
	float& getMovementSpeed() { return m_movementSpeed; }

	bool isInWindow(sf::Vector2f _pos);
private:
	sf::View m_view;

	sf::RectangleShape m_backgroundShape;

	sf::RenderTexture m_renderer;

	std::string m_name;

	sf::Vector2f m_position;
	sf::Vector2f m_addedPosition;
	sf::Vector2f m_lastAddedPosition;
	sf::Vector2f m_size;
	sf::Vector2f m_lastSize;

	bool m_isOpen;
	bool m_hasWindowFocus;
	bool m_isMovementLocked;
	bool m_isZoomLocked;
	bool m_isViewportLocked;
	bool m_isCameraGrabbed;

	float m_zoom;
	float m_movementSpeed;
};

class ScreenZone
{
public:
	ScreenZone();
	ScreenZone(sf::Vector2i _screenIndex);
	~ScreenZone();

	sf::Vector2i& getScreenIndex() { return m_screenIndex; }
	sf::RectangleShape& getScreenShape() { return m_screenShape; }
	
	bool& isUsed() { return m_isUsed; }

	void setScreenToUsed();
	void setScreenToUnused();
private:
	sf::RectangleShape m_screenShape;

	sf::Vector2i m_screenIndex;

	bool m_isUsed;
};

class Viewports
{
public:
	Viewports();
	~Viewports();

#pragma region UPDATE/DISPLAY

	/*
	* @brief All the event that as been launch will be here.
	* 
	* @param _event : the event that as been received.
	*/
	void UpdateEvent(sf::Event& _event);

	/*
	* @brief Just an update.
	*/
	void Update(ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window);

	/*
	* @brief A function to display every viewport.
	* 
	* @param _display : You will to put a lambda that contain everything you need to display on the viewport.
	*/
	void Draw(ObjWeakPtrList& _object, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window);
#pragma endregion

#pragma region GETTER/SETTER
	auto& getAllViewport() { return m_viewports; }
	auto& getAllScreenzone() { return m_screenZones; }
	auto& getActualSelectedObjectNumber() { return m_actualSelectedObjectNumber; }
	bool& wantToPlaceAnObject() { return m_wantToPlaceAnObject; }
	static sf::Color& getBackGroundColor() {return  s_background_color;}
	
	auto getActualFocusViewport() { return m_actualFocusViewport; }
	auto getActualUpdatedViewport() 
	{ 
		return m_acutualUpdatedViewport; 
	}
#pragma endregion

#pragma region OTHER_FUNCTION
	/*
	* @brief Add a new viewport.
	*
	* @param _viewport : The viewport that will be added to the container.
	*/
	void addViewport(std::shared_ptr<Viewport> _viewport);

	void setScreenZoneToUse(ScreenZone& _screenZone);

	/*
	* @brief Clear all the viewport.
	*/
	void clearViewport();

	void clearScreenZone();

	/*
	* @brief A function to open or close the viewport option window.
	*/
	bool& isOptionOpen();
#pragma endregion
private:
#pragma region PRIVATE_FUNCTION
	/*
	* @brief Update of all the viewport.
	*/
	void UpdateViewports(ObjWeakPtrList& _selectedObject, std::shared_ptr<lc::GameObject> _scene, WindowManager& _window);

	/*
	* @brief Update of the option that can be apply to All/Selected viewport.
	*/
	void UpdateViewportOption();

	/*
	* @brief Update of the ScreenZones (The white lines).
	*/
	void UpdateScreenZones(std::shared_ptr<Viewport>& _viewport, WindowManager& _window);
	/*
	* @brief Display of the ScreenZones (The white lines).
	*/
	void DisplayScreenZones(sf::RenderTexture& _renderer);
	/*
	* @brief Set the screen to unused that as been set in the parameter.
	* @brief This will also delete the unused screen on his side.
	*/
	void DeleteScreen(ScreenZone& _screenZone);

	/*
	* @brief This function is here to move with Z/Q/S/D or the mouse.
	*/
	void ViewportMovement(sf::Vector2f _mousePosition, std::shared_ptr<Viewport> _viewport);

	/*
	* @brief This function is here to delete the screen that as been closed.
	*/
	void ViewportDeleteOnCloseBehavior();
	/*
	* @brief This function is made to resize the selection rect with the selected object.
	*/
	void ResizeSelectionBoxBehavior(
		sf::Vector2f _mousePositionWithView,
		ObjWeakPtrList& _selectedObject,
		std::shared_ptr<lc::GameObject> _scene,
		std::shared_ptr<Viewport> _viewport,
		WindowManager& _window);
	/*
	* @brief This is made to selected object in the viewport or move the selected object.
	*/
	void ObjectIsSelectedOrMovedBehavior(
		sf::Vector2f _mousePosition,
		sf::Vector2f _mousePositionWithView,
		ObjWeakPtrList& _selectedObject,
		std::shared_ptr<lc::GameObject> _scene,
		std::shared_ptr<Viewport> _viewport,
		WindowManager& _window);

	/*
	* @brief This recurcive algorithm is made to check the selection of the object and his child and the child of his child...
	*/
	bool CheckSelection(
		ObjWeakPtrList& _object,
		std::shared_ptr<lc::GameObject> _scene,
		sf::Vector2f _mousePosition,
		std::shared_ptr<Viewport>& _viewport,
		WindowManager& _window);
	/*
	* @brief This function resize the selection rect.
	*/
	void ResizeRectSelection(ObjWeakPtrList& _object, std::shared_ptr<lc::GameObject> _scene);

	void CreateConvex(std::shared_ptr<lc::GameObject> _scene, sf::Vector2f mousePosition);
#pragma endregion
private:
	std::list<std::shared_ptr<Viewport>> m_viewports;
	std::list<ScreenZone> m_screenZones;

	std::weak_ptr<Viewport> m_actualFocusViewport;
	std::weak_ptr<Viewport> m_acutualUpdatedViewport;

	sf::RectangleShape m_selectionRect;
	sf::RectangleShape m_moveRect;

	sf::Vector2f m_maxSelectedObjectPosition;
	sf::Vector2f m_maxSelectedObjectSize;

	std::vector<sf::Vector2f> container;
	std::vector<sf::RectangleShape> points_convex;

	sf::FloatRect m_selection_rect_;
	
	unsigned int m_actualSelectedObjectNumber;

	float m_selectedTimer;
	float timer = 0.f;

	unsigned short m_screenZoneUnused;

	bool m_optionIsOpen;

	bool m_wantToPlaceAnObject;
	bool m_hasGrabbedAnObject;

	bool m_isMovementsLocked;
	bool m_isZoomsLocked;
	bool m_isViewportsLocked;

	inline static sf::Color s_background_color = sf::Color(0.f,0.f,0.f,255.f);
};