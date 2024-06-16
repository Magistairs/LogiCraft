
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
#include "SFML_ENGINE/Tools.h"
#include "Ressource.h"

namespace fs = std::filesystem;


class Folder //reproduce Windows folder
{
	Folder* m_parent;
	std::string m_parentFolder;
	bool m_open;
	fs::path m_path;
	std::list<Folder> m_childList;
	std::list<std::unique_ptr<lc::Ressource>> m_ressources;
	/**
	 * @brief Create a child folder
	 * 
	 * \param name Name of the new folder
	 */
	void createChild(std::string name);
	/**
	 * @brief Display the folder and his childrens in a ImGui TreeNode, this function need to be called inside a ImGui Window
	 * 
	 * 
	 * \param selectedFolder set this string to the path of the pressed folder in the TreeNode
	 */
	void displayFolder(std::string& selectedFolder);

	void Sort();

	char* m_newFolderName = new char[25];
	bool m_newFolder = false;

public:
	Folder()
		: m_open(false), m_parent(nullptr) {}
	Folder(std::string path, Folder* parent = nullptr);
	~Folder() {}

	Folder(Folder&& other) noexcept; //Move constructor cause of unique_ptr

	//Return the list of all the ressources contains in the folder
	const std::list<std::unique_ptr<lc::Ressource>>& getFolderRessources();

	/**
	 * @brief Add a new Ressource in the folder
	 * 
	 * \param ressource New ressources to push
	 * \return Return a ref of the unique_ptr push into the list of ressources
	 */
	std::unique_ptr<lc::Ressource>& AddRessources(std::unique_ptr<lc::Ressource>&& ressource);

	/**
	 * @brief Make the update and the imgui display of the folder
	 * 
	 * \param selectedFolder set this string to the path of the pressed folder in the TreeNode
	 */
	void update(std::string& selectedFolder);

	/**
	 * @brief Open a new imgui window to create a child folder
	 * 
	 * \return True if the window is still open, else return false
	 */
	bool CreateNewFolder();


	/**
	 * @brief Return a folder if path matches the folder path
	 * 
	 * \param path path of the folder wanted
	 * \return a ref of the wanted folder
	 */
	Folder& getFolder(std::string path);

	/**
	 * @brief return the child list of the folder
	 * 
	 * \return a ref to the list of child folder
	 */
	std::list<Folder>& getFolderList() { return m_childList; }

	bool getNewFolder(bool value) { m_newFolder = value;return m_newFolder; }

	/**
	 * @brief Function to get the path of the folder
	 * 
	 * \return the path of the folder
	 */
	std::filesystem::path getPath();


	/**
	 * @brief Reload all the ressources of the folder
	 *
	 */
	void reload();

	/**
	 * @brief Save all the ressources of the folder into his real windows folder
	 * 
	 */
	void Save();
	/**
	 * @brief Load all the ressources in the windows folder into the memory folder
	 * 
	 */
	void Load();
};

