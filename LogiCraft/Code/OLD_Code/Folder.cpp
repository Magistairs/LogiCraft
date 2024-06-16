
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

#include "Folder.h"
#include "tools_imgui.h"
#include "Texture.h"
#include "Font.h"
#include "Event.h"
#include <algorithm>
#include <fstream>


Folder& Folder::getFolder(std::string path)
{
	if (m_path.string() == path)
		return *this;
	Folder* folder = nullptr;

	for (auto& i : m_childList)
	{
		folder = &i.getFolder(path);
		if (folder)
			break;
	}

	return *folder;
}


void Folder::Save()
{
	for (auto& ressource : m_ressources)
	{
		if (ressource->m_type == lc::Ressource::TYPE::TEXTURE)
		{
			auto& texture = dynamic_cast<lc::Texture&>(*ressource);
			if (!Tools::sameFile(texture.getTexturePath(), m_path.string() + "/" + texture.getName() + ".png"))
			{
				sf::Image image = texture.getTexture().copyToImage();

				int textureSizeX((int)texture.getTextureSize().x), textureSizeY((int)texture.getTextureSize().y);
				sf::Uint8* ptr = new sf::Uint8[textureSizeX * textureSizeY * 4];
				for (int i = 0; i < texture.getTextureSize().y; i++)
				{
					size_t source = (texture.getTexture().getSize().x * texture.getTextureRect().top + (i * texture.getTexture().getSize().x) + texture.getTextureRect().left);
					source *= sizeof(sf::Uint8) * 4;

					memcpy(ptr + (textureSizeX * i * sizeof(sf::Uint8) * 4), image.getPixelsPtr() + source, sizeof(sf::Uint8) * 4 * textureSizeX);
				}

				sf::Image imageprt;
				imageprt.create(textureSizeX, textureSizeY, ptr);

				auto t = m_path.string() + "/" + texture.getName() + ".png";
				imageprt.saveToFile(t);
			}
		}
		else if (ressource->m_type == lc::Ressource::TYPE::FONT)
		{
			auto& font = dynamic_cast<lc::Font&>(*ressource);
			Tools::copyFile(font.getFontPath(), m_path.string() + "/" + font.getName() + ".ttf");

		}
		else if (ressource->m_type == lc::Ressource::TYPE::EVENT)
		{
			auto& event = dynamic_cast<lc::Event&>(*ressource);
			std::ofstream save(m_path.string() + "/" + event.getName() + ".evt", std::ios::binary);
			save.close();
		}
	}

	for (auto& child : m_childList)
		child.Save();
}

void Folder::Load()
{
	if (std::filesystem::exists(m_path))
		for (auto dir : std::filesystem::directory_iterator(m_path))
		{
			if (dir.path().filename().extension().string() == ".png")
				this->AddRessources(std::make_unique<lc::Texture>(dir.path().filename().stem().string(), sf::IntRect(), dir.path().string()));
			else if (dir.path().filename().extension().string() == ".ttf" or dir.path().filename().extension().string() == ".otf")
				this->AddRessources(std::make_unique<lc::Font>(dir.path().filename().stem().string(), dir.path().string()));
			else if (dir.path().filename().extension().string() == ".evt")
				this->AddRessources(std::make_unique<lc::Event>(dir.path().filename().stem().string(), -1));
		}
}

Folder::Folder(std::string path, Folder* parent) : m_ressources()
{
	if(fs::exists(path))
		m_path = fs::path(path);
	else
	{
		m_path = fs::path(path);
		fs::create_directory(m_path);
	}

	m_parentFolder = m_path.parent_path().string();
	m_parent = parent;
	Load();

	for (auto& p : fs::directory_iterator(m_path))
	{
		if (fs::is_directory(p))
		{
			m_childList.push_back(std::move(Folder(p.path().string(),this)));
		}
	}
	m_open = false;
	m_newFolderName[0] = '\0';
}

Folder::Folder(Folder&& other) noexcept
{
	m_path = other.m_path;
	other.m_path.clear();
	m_parentFolder = other.m_parentFolder;
	other.m_parentFolder.clear();
	m_parent = other.m_parent;
	other.m_parent = nullptr;
	m_childList = std::move(other.m_childList);
	m_ressources = std::move(other.m_ressources);
	m_open = other.m_open;
	other.m_open = false;
	m_newFolder = other.m_newFolder;
	other.m_newFolder = false;
	m_newFolderName=  other.m_newFolderName;
	other.m_newFolderName[0] = '\0';
}

const std::list<std::unique_ptr<lc::Ressource>>& Folder::getFolderRessources()
{
	return m_ressources;
}

std::unique_ptr<lc::Ressource>& Folder::AddRessources(std::unique_ptr<lc::Ressource>&& ressource)
{
	m_ressources.push_back(std::move(ressource));	
	auto& ptrToReturn = m_ressources.back();
	Sort();
	return ptrToReturn;
}

void Folder::update(std::string& selectedFolder)
{
	displayFolder(selectedFolder);
	if(m_newFolder)
		m_newFolder = CreateNewFolder();
}

bool Folder::CreateNewFolder()
{
	ImGui::Begin("Create New Folder##lc", &m_newFolder, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(300.f, 100.f), ImGuiCond_FirstUseEver);
	ImGui::SetWindowPos(ImVec2(700.f, 440.f), ImGuiCond_FirstUseEver);
	if (ImGui::InputText("Folder Name", m_newFolderName, 25, ImGuiInputTextFlags_EnterReturnsTrue) or ImGui::Button("Create"))
	{
		createChild(std::string(m_newFolderName));
		ImGui::End();
		m_newFolderName[0] = '\0';
		return false;
	}
	ImGui::End();
	return m_newFolder;
}

std::filesystem::path Folder::getPath()
{
	return m_path;
}

void Folder::createChild(std::string name)
{
	//We need to use semantics move cause of unique_ptr
	m_childList.push_back(std::move(Folder(m_path.string() + "/" + name, this)));
}


void Folder::displayFolder(std::string& selectedFolder)
{
	m_open = false;
	if(selectedFolder == m_path.string())
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31/ 255.f,57 / 255.f, 88.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31 / 255.f, 57 / 255.f, 88.f / 255.f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.f));
	}
	if (ImGui::TreeNodeEx(m_path.filename().string().c_str(),
		 ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick |ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		m_open = true;		
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		selectedFolder = m_path.string();
	}	

	if(m_open)
	{
		for (auto& child : m_childList)
		{
			child.displayFolder(selectedFolder);
		}

		ImGui::TreePop();
	}


	ImGui::PopStyleColor(2);
}

void Folder::Sort()
{
	m_ressources.sort([](const std::unique_ptr<lc::Ressource>& first, const std::unique_ptr<lc::Ressource>& second) {return first->m_type < second->m_type; });
}

void Folder::reload()
{
	m_childList.clear();
	m_ressources.clear();
	Load();
	for (auto& p : fs::directory_iterator(m_path))
	{
		if (fs::is_directory(p))
		{
			m_childList.push_back(std::move(Folder(p.path().string(), this)));
		}
	}
	m_open = false;
}
