
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

#include "Game.h"
#include "SFML_ENGINE/RessourcesManager.h"
#include "imgui.h"
#include "imgui-SFML.h"

Game::Game() : m_windowManager()
{
}

Game::Game(int width, int height, std::string title, bool fullscreen) : m_windowManager(width, height, title, fullscreen)
{
}

Game::~Game()
{
    m_state.clear();
}

void Game::update()
{
    Tools::restartClock();
    m_windowManager.StopEventUpdate(true);
    m_windowManager.EventUpdate();

    while (m_windowManager.pollEvent())
    {
        ImGui::SFML::ProcessEvent(m_windowManager.getEvent());
        if (m_windowManager.getEvent().type == sf::Event::Closed)
            m_windowManager.getWindow().close();

        if (!m_state.empty())
            m_state.front()->updateEvent(m_windowManager.getEvent());
    }

    ImGui::SFML::Update(m_windowManager.getWindow(),  sf::Time(sf::seconds(Tools::getDeltaTime())));
  
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if (KEY(F11) && m_windowManager.timer() > 0.25f && m_windowManager.getWindow().hasFocus())
    {
        m_windowManager.toggleFullscreen();
        m_windowManager.resetTimer();
    }

    if (!m_state.empty())
    {
        if (!m_state.front().get()->getIsReady() and GET_MANAGER->IsReady())
        {
            if (!m_state.front().get()->getIsStarted())
            {
                std::thread t([&] {m_state.front().get()->init(); });
                t.detach();
                m_state.front().get()->getIsStarted() = true;
            }
        }
        else if (m_state.front().get()->getIsReady())
#ifdef NDEBUG
            if (m_windowManager.getWindow().hasFocus())
#endif                
                m_state.front().get()->update();

        if (static_cast<int>(m_state.size()) > 1)
            m_state.erase(std::remove_if(m_state.begin(), m_state.end(), [](std::unique_ptr<State>& _state)
                {
                    return _state->getNeedToBeDeleted();
                }), m_state.end());
    }
}

void Game::render()
{
    m_windowManager.clear(sf::Color::Black);
    ImGui::SFML::Render(m_windowManager.getWindow());
    if (!m_state.empty() and GET_MANAGER->IsReady() and m_state.front().get()->getIsReady())
        m_state.front().get()->render();
    else if (!GET_MANAGER->IsReady() or !m_state.front().get()->getIsReady())
        GET_MANAGER->showLoadingScreen(m_windowManager.getWindow());
    m_windowManager.display();
}

//void Game::runGame()
//{
//    m_windowManager.getWindow().setMouseCursorVisible(false);
//    /*TODO : Modifier la texture de l'�cran de chargement*/
//    GET_MANAGER->getLoadingScreen() = Animation(GET_MANAGER->getTexture("loading"), sf::IntRect(0, 0, 140, 170), 0.1f, 7);
//    GET_MANAGER->getLoadingScreen().getSprite().setPosition({ (m_windowManager.getWindow().getSize().x - 140.f) / 2, (m_windowManager.getWindow().getSize().y - GET_MANAGER->getLoadingScreen().getSprite().getGlobalBounds().height) / 2 });
//
//    /*TODO : Modifier le state de d�part de l'application*/
//    m_state.push_back(State::Create<EditorState>(m_windowManager, &m_state));
//
//    ImGui::SFML::Init(m_windowManager.getWindow());
//
//    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
//
//    auto& style = ImGui::GetStyle();
//    style.WindowRounding = 8.0f;
//
//    while (!m_windowManager.isDone() and m_windowManager.getWindow().isOpen())
//    {
//        if (GET_MANAGER->IsReady())
//        {
//            GET_MANAGER->setVolumeForAllMusic(m_windowManager.getMusic_Volume());
//            GET_MANAGER->setVolumeForAllSound(m_windowManager.getSFX_Volume());
//        }
//
//        update();
//        render();
//    }
//    ImGui::SFML::Shutdown();
//}

void Game::runGame()
{
    m_windowManager.getWindow().setMouseCursorVisible(false);
    /*TODO : Modifier la texture de l'�cran de chargement*/
    GET_MANAGER->getLoadingScreen() = Animation(GET_MANAGER->getTexture("loading"), sf::IntRect(0, 0, 140, 170), 0.1f, 7);
    GET_MANAGER->getLoadingScreen().getSprite().setPosition({ (m_windowManager.getWindow().getSize().x - 140.f) / 2, (m_windowManager.getWindow().getSize().y - GET_MANAGER->getLoadingScreen().getSprite().getGlobalBounds().height) / 2 });

    /*TODO : Modifier le state de d�part de l'application*/
    m_state.push_back(State::Create<EditorState>(m_windowManager, &m_state));

    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGui::SFML::Init(m_windowManager.getWindow());

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    auto& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;

    while (!m_windowManager.isDone() and m_windowManager.getWindow().isOpen())
    {
        if (GET_MANAGER->IsReady())
        {
            GET_MANAGER->setVolumeForAllMusic(m_windowManager.getMusic_Volume());
            GET_MANAGER->setVolumeForAllSound(m_windowManager.getSFX_Volume());
        }

        update();
        render();
    }
    ImNodes::DestroyContext();
    ImGui::SFML::Shutdown();
}