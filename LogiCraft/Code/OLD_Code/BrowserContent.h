
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
#include "SFML_ENGINE/WindowManager.h"

#include "tools_imgui.h"
#include "Folder.h"
#include "Viewport.h"
#include "GameObject.h"

#undef CreateFont
#undef CreateEvent

class BrowserContent
{
public:
	BrowserContent();
	~BrowserContent() { m_mainFolder.Save(); };
	/**
	 * @brief Update the BrowserContent
	 * 
	 * \param _object GameObejct to update (generally its the scene)
	 * \param _window SFML window
	 * \param _viewport Viewport to update
	 */
	void Update(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);
	/**
	* @brief Draw the BrowserContent
	* 
	* \param _object GameObejct to draw (generally its the scene)
	* \param _window SFML window
	* \param _viewport Viewport to draw
	*/

	void Draw(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);
private:
	/**
	 * @brief Create a imgui window to select the type and create a new ressource
	 * 
	 */
	void CreateRessources();

	/**
	 * @brief Create a imgui window to create a new Texture
	 *
	 */
	void CreateTexture();

	/**
	 * @brief Create a imgui window to create a new Font
	 * 
	 */
	void CreateFont();

	/**
	 * @brief Create a imgui window to create a new Event
	 * 
	 */
	void CreateEvent();

	/**
	 * @brief Put a new ressource into the scene
	 * 
	 * \param _object the gameobject to which we add the resource(generally its the scene)
	 * \param _window SFML window Manager
	 * \param _viewport All the viewports of the software
	 */
	void PutRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);

	/*
	* @brief This function is use to make the naming object window appear.
	*/
	void MakeRessourcesName(std::shared_ptr<lc::GameObject> _scene);

	/*
	* @brief This function is used to check if any object in the scene have the same name as the slidingRessources.
	*/
	bool CheckObjectName(std::shared_ptr<lc::GameObject> _obj, std::string _name);

	/**
	 * @brief Add a component to the gameobject
	 * 
	 * \param _object the gameobject to which we add the resource(generally its the scene)
	 * \param _window SFML window Manager 
	 * \param _viewport All the viewports of the software
	 */
	void AddComponent(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewport& _viewport);

	/**
	* @brief Draw the sliding ressources
	* 
	* \param _object the gameobject to which we add the resource(generally its the scene)
	* \param _window SFML window Manager
	* \param _viewport All the viewports of the software
	*/
	void DrawSlidingRessources(std::shared_ptr<lc::GameObject> _object, WindowManager& _window, Viewports& _viewport);

	void Reload();

private:
	lc::Ressource::TYPE m_selectedRessourceType = lc::Ressource::TYPE::NONE;
	lc::Ressource::TYPE m_sortedRessources = lc::Ressource::TYPE::NONE;

	std::string m_sortString = {"ALL"};
	std::string m_selectedFolder = {};
	std::string m_selectedRessources = {};
	std::string m_RessourceName;
	std::string m_newFolderName;
	std::string m_newObjectName;

	sf::IntRect m_newTextureRect;

	std::atomic_bool m_isLoading = false;

	bool m_creatingNewRessources = false;
	bool m_isSlidingRessources = false;
	bool m_selectingFilter = false;
	// put ressource
	bool m_choosingName = false;
	bool m_nameIsReady = false;
	bool m_showError = false;
	sf::Vector2f m_putPosition;

	Folder m_mainFolder;
	lc::Ressource* m_slidingRessources = nullptr;
};
