
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
#include "Viewport.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Button.h"
#include "AI.h"
#include "Event.h"
#include "Font.h"
#include "tools_imgui.h"
#include "BrowserContent.h"

typedef std::map<unsigned char, std::string> Layers;
typedef std::pair<unsigned char, std::string> Layer;

class ToolsBar
{
public:
	ToolsBar();
	~ToolsBar()
	{
		std::ofstream file("../Ressources/parallax.txt");
		for(auto& it : s_parallax_speed_factor_map_)
			file << it.second << "\n" ;
	}

	void Update(std::shared_ptr <lc::GameObject> _game_object, WindowManager& _window, Viewports& _viewport);

	static Layers GetLayers();
	static Layer GetActualLayer();

	
	static void SetupParallaxSpeed();
	static float GetParallaxSpeedFactor(const int& index);
private:
	void InputCheck(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport);
	void AddViewport(Viewports& _viewport);
	void OpenViewportOption(Viewports& _viewport);
	void New(std::shared_ptr<lc::GameObject> _game_object, Viewports& _viewport);
	void Load(std::shared_ptr <lc::GameObject> _game_object, std::string path, Viewports& _viewport);
	void ShowHelp();

	void Save(std::shared_ptr <lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window);
	void Export(std::shared_ptr <lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window);
	
	void Exit(WindowManager& _window);
	void ChangeLayer(Layer _newLayer);

	void ChangeParallaxFactor();
	void ChangeBGColor();

	bool m_isExporting = false;
	bool m_isSaving = false;
	bool m_isLoading = false;
	bool m_showingHelp = false;
	bool m_isChangingParallaxSpeed = false;
	bool m_isChangingBGColor = false;
	
	inline static std::map<int,float> s_parallax_speed_factor_map_ = {};
	
	std::string m_path {'\0'};

	static Layer s_actualLayer;

	static Layers s_layers;

	
};

