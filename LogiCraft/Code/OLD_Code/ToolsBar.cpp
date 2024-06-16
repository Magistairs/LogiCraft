
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

#include <sstream>

#include "ToolsBar.h"

#include "PlayerSpawn.h"

Layers ToolsBar::s_layers = { {13, "BackGround 9"},{12, "BackGround 8"},{11, "BackGround 7"},{10,"BackGround 6"},{ 9, "BackGround 5"},{8, "BackGround 4"},{7, "BackGround 3"},{ 6, "BackGround 2"},{5, "BackGround 1"},{4, "Player Plan"},{3, "Front Plan 4"}, {2, "Front Plan 3"}, {1, "Front Plan 2"}, {0, "Front Plan 1"} };
Layer ToolsBar::s_actualLayer = *ToolsBar::s_layers.begin();

ToolsBar::ToolsBar()
{}

void ToolsBar::Update(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport)
{
	InputCheck(object, _window, _viewport);

	if (ImGui::BeginMainMenuBar())
	{
		sf::Vector2f subMenuSize = { 0.f,20.f };

		Tools::IG::SetSpacing({ 25.f,0.f }, [&]
			{
				if (ImGui::BeginMenu("File"))
				{
					Tools::IG::SetSpacing(subMenuSize, [&]
						{
							if (ImGui::MenuItem("New", "Ctrl + N") || (KEY(LControl) && KEY(N))) { New(object, _viewport); }
							if (ImGui::MenuItem("Load", "Ctrl + O") || (KEY(LControl) && KEY(O))) { m_isLoading = true; }
							if (ImGui::MenuItem("Save As", "Ctrl + S") || (KEY(LControl) && KEY(S))) { m_isSaving = true; }
							if (ImGui::MenuItem("Export as", "Ctrl + E") || (KEY(LControl) && KEY(E)))
							{
								m_isExporting = true;
								auto player = object->GetRoot(object)->getObject(PLAYER_NAME);
								auto spawn = object->GetRoot(object)->getComponent<lc::PlayerSpawn>("Player Spawn");
								player->getTransform().getPosition() = spawn->GetSpawnPosition();						}
							if (ImGui::MenuItem("Exit", "Ctrl + W") ||(KEY(LControl) && KEY(W))) { Exit(_window); }
							ImGui::EndMenu();
						});

				}

				if (ImGui::BeginMenu(ToolsBar::s_actualLayer.second.c_str()))
				{
					Tools::IG::SetSpacing(subMenuSize, [&] 
						{
							for (auto& layer : ToolsBar::s_layers)
								if (ImGui::MenuItem(layer.second.c_str())) 
									ChangeLayer(layer);

							ImGui::EndMenu(); 
						});
				}

				if (ImGui::BeginMenu("Viewport"))
				{
					Tools::IG::SetSpacing(subMenuSize, [&] 
						{
							if (ImGui::MenuItem("Add viewport")) { AddViewport(_viewport); }
							if (ImGui::MenuItem("Viewport Option")) { OpenViewportOption(_viewport); }
							ImGui::EndMenu(); 
						});
				}

				if(ImGui::Button("Any Help ?")) { m_showingHelp = true; }
				if(ImGui::Button("Change Parallax Speed")) { m_isChangingParallaxSpeed = true; }
				if(ImGui::Button("Change Background Color")) { m_isChangingBGColor = true; }
				if (m_showingHelp)
					ShowHelp();
				if (ImGui::BeginMenu(std::to_string(Tools::getDeltaTime()).c_str()))
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu(std::to_string(Tools::frameRate()).c_str()))
				{
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			});
	}

	if (m_isSaving)
	{
		Tools::IG::CreateWindow("Save", m_isSaving, ImGuiWindowFlags_AlwaysAutoResize, [&] {
			InputText("Save Name", m_path, 100);
			if (Button("Save") && m_path[0] != '\0')
			{
				std::filesystem::create_directory(std::string("../Ressources/" + std::string(m_path)));
				Save(object, _viewport, _window.getWindow());
				m_isSaving = false;
				m_path.clear();
			}
			});
	}
	if(m_isChangingParallaxSpeed)
		ChangeParallaxFactor();
	if(m_isChangingBGColor)
		ChangeBGColor();
	if (m_isExporting)
	{
		Tools::IG::CreateWindow("Export", m_isExporting, ImGuiWindowFlags_AlwaysAutoResize, [&] {
			InputText("Export Name", m_path, 100);
			if (Button("Export") && m_path[0] != '\0')
			{
				std::filesystem::create_directory(std::string("../Ressources/" + std::string(m_path)));
				Export(object, _viewport, _window.getWindow());
				m_isExporting = false;
				m_path.clear();
			}
			});
	}
	else if (m_isLoading)
	{
		Tools::IG::CreateWindow("Load", m_isLoading, ImGuiWindowFlags_AlwaysAutoResize, [&] {
			if (m_path[0] != '\0')
				Text(m_path.c_str());
			if (Button("Search .lcp file"))
			{
				Tools::IG::LoadRessourcesFromFile(m_path, "lcp");
			}
			if (Button("Load") && m_path[0] != '\0')
			{
				Load(object, m_path, _viewport);
				m_path.clear();
				m_isLoading = false;
			}
			});
	}
}

Layers ToolsBar::GetLayers()
{
	return s_layers;
}

Layer ToolsBar::GetActualLayer()
{
	return s_actualLayer;
}

void ToolsBar::SetupParallaxSpeed()
{
	/*{11, "BackGround 7"},{10,"BackGround 6"},{ 9, "BackGround 5"},{8, "BackGround 4"},{7, "BackGround 3"},{ 6, "BackGround 2"},{5, "BackGround 1"},
	 *{4, "Player Plan"},{3, "Front Plan 4"}, {2, "Front Plan 3"}, {1, "Front Plan 2"}, {0, "Front Plan 1"}*/
	std::ifstream file("../Ressources/parallax.txt");
	file >> s_parallax_speed_factor_map_[0];
	file >>s_parallax_speed_factor_map_[1];
	file >>s_parallax_speed_factor_map_[2];
	file >>s_parallax_speed_factor_map_[3];
	file >>s_parallax_speed_factor_map_[4];
	file >>s_parallax_speed_factor_map_[5];
	file >>s_parallax_speed_factor_map_[6];
	file >>s_parallax_speed_factor_map_[7];
	file >>s_parallax_speed_factor_map_[8];
	file >>s_parallax_speed_factor_map_[9];
	file >>s_parallax_speed_factor_map_[10];
	file >>s_parallax_speed_factor_map_[11];
	file >>s_parallax_speed_factor_map_[12];
	file >>s_parallax_speed_factor_map_[13];
	file.close();
}

float ToolsBar::GetParallaxSpeedFactor(const int& index)
{
	return s_parallax_speed_factor_map_[index];
}

void ToolsBar::InputCheck(std::shared_ptr <lc::GameObject> object, WindowManager& _window, Viewports& _viewport)
{
	if (KEY(LControl) && KEY(N)) { New(object, _viewport); }
	if (KEY(LControl) && KEY(O)) { m_isLoading = true; }
	if (KEY(LControl) && KEY(S)) { m_isSaving = true; }
	if (KEY(LControl) && KEY(W)) { Exit(_window); }
}

void ToolsBar::AddViewport(Viewports& _viewport)
{
	_viewport.addViewport(std::make_shared<Viewport>(sf::Vector2f(250.f, 20.f), sf::Vector2f(1460.f, 560.f), sf::Vector2u(1920u, 1080u), " "));
}

void ToolsBar::OpenViewportOption(Viewports& _viewport)
{
	_viewport.isOptionOpen() = true;
}

void ToolsBar::New(std::shared_ptr<lc::GameObject> _game_object, Viewports& _viewport)
{
	lc::GameObject::getGeneralID() = 0u;

	_viewport.clearScreenZone();
	*_game_object = *lc::GameObject::CreateGameObject("WORLD");
	_viewport.clearViewport();

	AddViewport(_viewport);
	ChangeLayer(*ToolsBar::s_layers.begin());
}

void ToolsBar::Load(std::shared_ptr <lc::GameObject> _game_object, std::string path, Viewports& _viewport)
{
	New(_game_object, _viewport);
	std::ifstream load(path);
	int number_of_screen(0);
	load >> number_of_screen;
	for(int i = 0; i < number_of_screen; i++)
	{
		sf::Vector2i screen_index;
		load >> screen_index;
		for(auto& screen : _viewport.getAllScreenzone())
		{
			if(screen.getScreenIndex() == screen_index)
				_viewport.setScreenZoneToUse(screen);
		}
	}	
	_game_object->Load(load);
}

void ToolsBar::ShowHelp()
{
	ImGui::Begin("Here we are !", &m_showingHelp, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Here is the list of possible inputs :");
	ImGui::BulletText("(Z | Arrow UP) | (S | Arrow DOWN) | (Q | Arrow LEFT) | (D | Arrow RIGHT) | (Mouse Right + Drag) = > Scene's view deplacement.");
	ImGui::BulletText("F11 => Toggle fulscreen.");
	ImGui::BulletText("Delete => Delete all selected objects.");
	ImGui::BulletText("LControl + N => Clear and create a new scene.");
	ImGui::BulletText("LControl + O => Open the load menu.");
	ImGui::BulletText("LControl + S => Open the save menu.");
	ImGui::BulletText("LControl + E => Open the export menu.");
	ImGui::BulletText("LControl + W => Close the program.");
	ImGui::BulletText("LControl + Release Left Click => Add component to a hovered object.");
	ImGui::BulletText("LControl + Left Click on unused screen zone => Allow the screen zone.");
	ImGui::BulletText("LControl + Right Click on used screen zone => Free the screen zone.");
	ImGui::BulletText("LControl + Left Click on multiple objects => Multiple Selection.");
	ImGui::BulletText("LControl + C => Copy all selected objects.");
	ImGui::BulletText("LControl + V => Paste all selected objects.");
	ImGui::BulletText("LShift + Left Click => Select object and all his child in hierarchy.");

	ImGui::End();
}


void ToolsBar::Save(std::shared_ptr <lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window)
{
	Tools::s_filePool.clear();
	std::ofstream save("../Ressources/" + std::string(m_path) + "/save.lcp");
	FileWriter exportation("../Ressources/" + std::string(m_path) + "/export.lcg");
	sf::RenderTexture render_texture;
	save << std::count_if(_viewports.getAllScreenzone().begin(),_viewports.getAllScreenzone().end(), [](ScreenZone& screen){return screen.isUsed();}) << " ";
	for (auto& screen : _viewports.getAllScreenzone())
	{
		if (screen.isUsed())
			save << screen.getScreenIndex() << " ";
	}
	save << "\n";
	_game_object->Save(save, exportation, render_texture, s_actualLayer.first);
	save.close();
	exportation.close();
	fs::remove(fs::path("../Ressources/" + std::string(m_path) + "/export.lcg"));	
}
void ToolsBar::Export(std::shared_ptr<lc::GameObject> _game_object, Viewports& _viewports, sf::RenderWindow& _window)
{
	Tools::s_filePool.clear();
	std::ofstream save("../Ressources/" + std::string(m_path) + "/save.lcp");
	FileWriter exportation("../Ressources/" + std::string(m_path) + "/export.lcg");
	std::ostringstream oss;
	
	oss << std::count_if(_viewports.getAllScreenzone().begin(),_viewports.getAllScreenzone().end(), [](ScreenZone& screen){return screen.isUsed();}) << " ";
	for (auto& screen : _viewports.getAllScreenzone())
	{
		if (screen.isUsed())
			oss << screen.getScreenIndex() << " ";
	}
	oss << "\n";
	save << oss.str();
	exportation << oss.str();
	sf::RenderTexture render_texture;
	_game_object->NeedToBeExported({"AI", "RigidBody", "Particles", "Animation", "Event", "Button", "Player Spawn", "DisplayCollider","Heat_Shader", "Light_Shader", "Water_Shader"});
	_game_object->Save(save, exportation, render_texture, s_actualLayer.first);

	ThreadManager thread_manager(std::count_if(_viewports.getAllScreenzone().begin(),_viewports.getAllScreenzone().end(), [](ScreenZone& screen){return screen.isUsed();}));

	unsigned int number_of_thread(0u);
	constexpr unsigned short thread_limit(20u);
	
	for (auto& screen : _viewports.getAllScreenzone())
	{
		if (screen.isUsed())
		{
			while (number_of_thread >= thread_limit) {}
			
			if (number_of_thread <= thread_limit)
			{
				number_of_thread++;
				std::cout << number_of_thread << '\n';
				thread_manager.AddNewThread([&] 
				{
					sf::RenderTexture render_texture_thread;				
					render_texture_thread.create(static_cast<unsigned int>(SCREEN_SIZE.x), static_cast<unsigned int>(SCREEN_SIZE.y));
					render_texture_thread.setView(sf::View(sf::Vector2f(SCREEN_SIZE.x / 2.f + (SCREEN_SIZE.x * screen.getScreenIndex().x), SCREEN_SIZE.y / 2.f + (SCREEN_SIZE.y * screen.getScreenIndex().y)), sf::Vector2f(SCREEN_SIZE.x, SCREEN_SIZE.y)));
					for (int depth = 0; depth <= static_cast<int>(s_layers.size()); depth++)
					{
						render_texture_thread.clear(sf::Color::Transparent);
						bool need_to_export_png = false;
						_game_object->SaveRenderer(render_texture_thread, depth,need_to_export_png, sf::FloatRect({SCREEN_SIZE.x * screen.getScreenIndex().x,SCREEN_SIZE.y * screen.getScreenIndex().y},SCREEN_SIZE));
						if(need_to_export_png)
						{
							sf::Image image;
						   image = render_texture_thread.getTexture().copyToImage();
						   image.flipVertically();
						   image.saveToFile("../Ressources/" + std::string(m_path) + "/" + std::string(std::to_string(screen.getScreenIndex().x) + "_" + std::to_string(screen.getScreenIndex().y)) + "_" + std::string(m_path) + "_layer_" + std::to_string(depth) + ".png");
						}
					}

					number_of_thread--;
				});
			}
			
		}
	}

	while(thread_manager.GetThreadSize())
		thread_manager.Update();
	
	_game_object->ResetExport();
	save.close();
	exportation.close();
}

void ToolsBar::Exit( WindowManager& _window)
{
	_window.getWindow().close();
}

void ToolsBar::ChangeLayer(Layer _newLayer)
{
	s_actualLayer = _newLayer;
}

void ToolsBar::ChangeParallaxFactor()
{
	ImGui::Begin("Parallax Factor",&m_isChangingParallaxSpeed,ImGuiWindowFlags_AlwaysAutoResize);
	for(auto& it : s_parallax_speed_factor_map_)
	{
		if(s_layers[it.first] != "Player Plan")
			ImGui::SliderFloat(s_layers[it.first].c_str(),&it.second,-1.f,1.f,"%.3f");		
	}
	ImGui::End();
}

void ToolsBar::ChangeBGColor()
{
	ImGui::Begin("Background Color##lc", &m_isChangingBGColor,  ImGuiWindowFlags_AlwaysAutoResize);
	ColorEdit4("Color", Viewports::getBackGroundColor(),ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB);
	End();
}
