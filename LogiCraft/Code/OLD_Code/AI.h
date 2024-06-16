
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
#include "Ressource.h"
#include "Texture.h"
#include "BehaviorTree.h" 

namespace lc
{
	class AI : public Ressource
	{
	public:
		AI();
		AI(AI& other);
		virtual ~AI();

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int _depth) override;
		void SaveRenderer(sf::RenderTexture& texture, int _depth) override {};
		virtual void Load(std::ifstream& load) override;
		void Export(std::ofstream& exportation) override;
	private:
		std::string m_aiName;
		PatronNode m_root;
		PatronNode* m_selectedNode = nullptr;
		std::pair<std::weak_ptr<GameObject>, PatronNode*> m_copiedNode = {};
		int m_newNodeType = 0;
		int m_newDecorateType = PatronNode::GetDecoratorNodeStart();
		bool m_isEditionBehaviorTree = false;
		bool m_wantToCopyATreeNode = false;

		virtual void UpdateEvent(sf::Event& _window) override;
		virtual void Update(WindowManager& _window) override;
		virtual void Draw(WindowManager& _window) override;
		virtual void Draw(sf::RenderTexture& _window) override;
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc();

		/**
		 * @brief Update the selected node in the behavior tree
		 * 
		 */
		void UpdateSelectedNode();

		/**
		 * @brief Add a node to the selected node in the behavior tree
		 * 
		 * \param type Type of the node to add
		 */
		void AddNode(int type);

		/**
		* @brief Remove the selected node in the behavior tree	
		*/
		void RemoveNode();
		/**
		 * @brief Add a decorate node to the selected node in the behavior tree
		 * 
		 * \param type Type of the node to add
		 */
		void AddDecorateNode(int type);
		/**
		* @brief Remove the decorate node in the behavior tree
		*/
		void RemoveDecorateNode();
		/**
		* @brief Copy the selected TreeNode in this TreeNode
		* 
		*/
		void CopyTreeNode(std::shared_ptr<lc::GameObject> _game_object);
		/**
		 * @brief Old function create to make a combo box in the imgui window
		 */
		int MakeCombo(int start, int end, int defaultValue ,std::string name);
	};
}