
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

#include "tools_imgui.h"
#include "nfd.h"

ThreadManager::~ThreadManager()
{
	while(!m_thread_list_.empty())
		this->Update();
}

void ThreadManager::Update()
{
	for(auto it = m_thread_list_.begin();it != m_thread_list_.end();)
	{
		if(*it->second)
		{
			it->first.join();
			it = m_thread_list_.erase(it);
		}
	}
}


bool Tools::camera_grabbed = false;

Tools::Renderer::Renderer()
	: m_renderer_is_focus_(false), m_renderer_is_grabbed_(false), m_renderer_zoom_(1.f)
{
	m_renderer_view_ = sf::View(sf::Vector2f(960.f, 540.f), sf::Vector2f(1920.f, 1080.f));

	m_renderer_ = std::make_shared<sf::RenderTexture>();
	m_renderer_->create(1920u, 1080u);
	m_renderer_->setView(m_renderer_view_);
}

Tools::Renderer::~Renderer()
{
	m_renderer_.reset();
}

void Tools::Renderer::UpdateZoom(const sf::Event& _event)
{
	if (_event.type == sf::Event::MouseWheelScrolled && m_renderer_is_focus_)
	{
		if (_event.mouseWheelScroll.delta > 0.f && m_renderer_zoom_ > 0.2f)
			m_renderer_zoom_ *= (1 / 1.1f);
		else if (_event.mouseWheelScroll.delta < 0.f && m_renderer_zoom_ < 8.f)
			m_renderer_zoom_ *= 1.1f;
	}
}

void Tools::Renderer::Update()
{
	this->UpdateResize();

	this->UpdateMovement();

	ImGui::Image(*m_renderer_, m_renderer_size_);
}

void Tools::Renderer::Clear()
{
	m_renderer_->clear(sf::Color::Transparent);
	m_renderer_->setView(m_renderer_view_);
}

void Tools::Renderer::Draw(const sf::Drawable& drawable, const sf::RenderStates& states)
{ m_renderer_->draw(drawable, states); }

void Tools::Renderer::Draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
{ m_renderer_->draw(vertices, vertexCount, type, states); }

void Tools::Renderer::Draw(const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states)
{ m_renderer_->draw(vertexBuffer, states); }

void Tools::Renderer::Draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount, const sf::RenderStates& states)
{ m_renderer_->draw(vertexBuffer, firstVertex, vertexCount, states); }

void Tools::Renderer::Display()
{ m_renderer_->display(); }

std::shared_ptr<sf::RenderTexture>& Tools::Renderer::get_render_texture() { return m_renderer_; }

sf::Vector2f& Tools::Renderer::get_added_position() { return m_renderer_added_position_; }

sf::Vector2f& Tools::Renderer::get_last_added_position() { return m_renderer_last_added_position_; }

sf::Vector2f& Tools::Renderer::get_size() { return m_renderer_size_; }

sf::View& Tools::Renderer::get_view() { return m_renderer_view_; }

bool& Tools::Renderer::his_focus() { return m_renderer_is_focus_; }

bool& Tools::Renderer::his_grabbed() { return m_renderer_is_grabbed_; }

float& Tools::Renderer::get_zoom() { return m_renderer_zoom_; }

void Tools::Renderer::UpdateResize()
{
	m_renderer_is_focus_ = ImGui::IsWindowFocused();

	m_renderer_view_.setCenter((m_renderer_size_ / 2.f + m_renderer_added_position_) * m_renderer_zoom_);
	m_renderer_view_.setSize(m_renderer_size_ * m_renderer_zoom_);

	if (m_renderer_size_ != sf::Vector2f(ImGui::GetContentRegionAvail()))
	{
		m_renderer_size_ = ImGui::GetContentRegionAvail();
		m_renderer_->create(static_cast<unsigned>(m_renderer_size_.x), static_cast<unsigned>(m_renderer_size_.y));
	}
}

void Tools::Renderer::UpdateMovement()
{
	if (MOUSE(Right))
	{
		//If mouse right is press and its in the window bounds.
		if (Tools::Collisions::point_rect(ImGui::GetMousePos(), { ImGui::GetWindowPos(), ImGui::GetWindowSize() }) &&
			!m_renderer_is_grabbed_ && !Tools::camera_grabbed)
		{
			//And the mouse is not on another window the camera is grabbed.
			if (!Tools::IG::MouseIsOnAboveWindow())
			{
				Tools::camera_grabbed = true;
				m_renderer_is_grabbed_ = true;
				m_renderer_last_added_position_ = m_renderer_added_position_;
				ImGui::SetWindowFocus();
			}
		}
		else if (m_renderer_is_grabbed_)
		{
			m_renderer_added_position_ = -sf::Vector2f(ImGui::GetMouseDragDelta(ImGuiMouseButton_Right)) + m_renderer_last_added_position_;
		}
	}
	else
	{
		Tools::camera_grabbed = false;
		m_renderer_is_grabbed_ = false;
	}
}

void Tools::IG::LoadRessourcesFromFile(std::string& path, const char* filter)
{
	{
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(filter, nullptr, &outPath);
		path = "";
		if(outPath)
			path = std::string(outPath);
	}
}

void Tools::IG::SaveRessourcesFromFile(std::string& path, const char* filter)
{
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog(filter, nullptr, &outPath);
	if (outPath)
		path = std::string(outPath);
}

bool Tools::IG::MouseIsOnAboveWindow(ImGuiWindow* _window)
{
	for (auto& window : ImGui::GetCurrentContext()->Windows)
		if (window->ID != _window->ID)
			if (Tools::Collisions::point_rect(ImGui::GetMousePos(), { window->Pos, window->Size }) &&
				ImGui::IsWindowAbove(window, _window) &&
				window->WasActive &&
				std::string(window->Name).find("lc") != std::string::npos)
			{
				return true;
			}
	 
	return false;
}

bool Tools::Collisions::lineRect(sf::FloatRect line, sf::FloatRect rect)
{
	bool left = lineLine(line, { rect.left, rect.top, rect.left, rect.top + rect.height });
	bool right = lineLine(line, { rect.left + rect.width,  rect.top, rect.left + rect.width, rect.top + rect.height });
	bool top = lineLine(line, { rect.left,  rect.top, rect.left + rect.width,  rect.top });
	bool bottom = lineLine(line, { rect.left, rect.top + rect.height, rect.left + rect.width, rect.top + rect.height });

	bool lineInRect = Tools::Collisions::point_rect(line.getPosition(), rect) or Tools::Collisions::point_rect(line.getSize(), rect);
	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom || lineInRect) {
		return true;
	}
	return false;
}

bool Tools::Collisions::lineLine(sf::FloatRect line_one, sf::FloatRect line_two)
{
	float uA = ((line_two.width - line_two.left) * (line_one.top - line_two.top) - (line_two.height - line_two.top) * (line_one.left - line_two.left)) / ((line_two.height - line_two.top) * (line_one.width - line_one.left) - (line_two.width - line_two.left) * (line_one.height - line_one.top));
	float uB = ((line_one.width - line_one.left) * (line_one.top - line_two.top) - (line_one.height - line_one.top) * (line_one.left - line_two.left)) / ((line_two.height - line_two.top) * (line_one.width - line_one.left) - (line_two.width - line_two.left) * (line_one.height - line_one.top));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

		return true;
	}
	return false;
}

bool Tools::sameFile(const std::string& sourceFile, const std::string& destFile)
{
	fs::path source(sourceFile), dest(destFile);

	if (source == dest)
		return true;

	for (auto& i : Tools::s_filePool)
	{
		if (i == source.filename())
			return true;
	}

	return false;
}

void Tools::copyFile(const fs::path& sourceFile, const fs::path& destFile)
{
	if (sameFile(sourceFile.string(), destFile.string()))
		return;

	if(!fs::exists(destFile.parent_path()))
		fs::create_directories(destFile.parent_path());

	std::ifstream sourceStream(sourceFile, std::ios::binary);
	if (!sourceStream.is_open()) {
		std::cerr << "Erreur : Impossible d'ouvrir le fichier source." << std::endl;
		return;
	}

	std::ofstream destStream(destFile, std::ios::binary);
	if (!destStream.is_open()) {
		std::cerr << "Erreur : Impossible d'ouvrir le fichier de destination." << std::endl;
		sourceStream.close();
		return;
	}

	destStream << sourceStream.rdbuf();

	Tools::s_filePool.push_back(sourceFile.filename().string());

	sourceStream.close();
	destStream.close();
}


void Tools::ReplaceCharacter(std::string& _sentence, unsigned char _characterToReplace, unsigned char _replaceChar)
{
	for (auto& character : _sentence)
		if (character == _characterToReplace)
			character = _replaceChar;
}

std::string Tools::replaceSpace(std::string string, bool spaceOrUnderscore)
{
	if (!spaceOrUnderscore)
	{
		for (size_t i = 0; i < string.length(); ++i)
		{
			if (string[i] == '_')
			{
				string[i] = ' ';
			}
		}
	}
	else
	{
		for (size_t i = 0; i < string.length(); ++i)
		{
			if (string[i] == ' ')
			{
				string[i] = '_';
			}
		}
	}
	return string;
}

FileWriter::FileWriter(fs::path path) : std::ofstream(path), m_path(path)
{
}

fs::path FileWriter::getPath() const
{
	return m_path;
}

void Tools::remove_extention(std::string& string, const std::string& extention_name)
{
	const auto tmp_off((string.find_last_of("." + extention_name)));
	if (tmp_off != std::string::npos)
		if (string.begin() + static_cast<int>(tmp_off) + 1 == string.end())
			string = string.substr(0, tmp_off - 4);
}

std::string Tools::split_path_and_file_name(std::string& path_with_file_name)
{
	std::string tmp_file_name(path_with_file_name);

	const int tmp_off(static_cast<int>(path_with_file_name.find_last_of('\\')) + 1);

	path_with_file_name = path_with_file_name.substr(0, tmp_off);
	return tmp_file_name = tmp_file_name.substr(tmp_off);
}

std::ostream& operator<<(std::ostream& os, const sf::Color& color)
{
	return os << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " "
			  << static_cast<int>(color.b) << " " << static_cast<int>(color.a);
}

std::istream& operator>>(std::istream& is, sf::Color& color)
{
	int tmp_tab[4]{ 0, 0, 0, 0 };
	is >> tmp_tab[0] >> tmp_tab[1] >> tmp_tab[2] >> tmp_tab[3];
	color = sf::Color(static_cast<sf::Uint8>(tmp_tab[0]), static_cast<sf::Uint8>(tmp_tab[1]),
		static_cast<sf::Uint8>(tmp_tab[2]), static_cast<sf::Uint8>(tmp_tab[3]));
	return is;
}
