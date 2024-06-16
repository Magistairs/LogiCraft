
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
#include "tools_imgui.h" 
#include <any>
#include <functional> 

class PatronNode;

typedef std::unordered_map<std::string, std::function<void(PatronNode*, std::ofstream& file)>> DecoratorSaveMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*, std::ifstream& file)>> DecoratorLoadMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*)>> DecoratorMethod;
typedef std::unordered_map<std::string, std::function<void(PatronNode*, PatronNode*)>> DecoratorCopyMethod;

/*
 * @brief Class that represent a node of a behavior tree
*/


class PatronNode
{
	int m_type_;
	int m_condition_type_;
	std::list <PatronNode*> m_child_list_;
	PatronNode* m_p_parent_;
	int m_id_;
	bool m_is_open_ = false;
	bool m_new_node_is_added_ = false;
	std::any m_decorator_data_;
	std::weak_ptr<lc::GameObject> m_game_object_;

	static DecoratorSaveMethod s_decorator_save_method_;
	static DecoratorLoadMethod s_decorator_load_method_;
	static DecoratorMethod s_decorator_init_method_;
	static DecoratorMethod s_decorator_update_method_;
	static DecoratorCopyMethod s_decorator_copy_method_;
	static std::unordered_map<std::string, int> s_condition_container_;
	static std::unordered_map<std::string, int> s_node_container_;
	inline static int s_id_ = 0;
	inline static int s_action_node_start_ = 0;
	inline static int s_decorator_node_start_ = 0;	
	inline static int s_action_node_end_ = 0;
	inline static int s_decorator_node_end_ = 0;	
public:
	/**
	 * @brief Static method, MUST BE CALLED AT PROGRAM INIT
	 * Setup all the node identification and their method
	 * 
	 */
	static void SetupAllNode();

	/**
	 * @brief Static method, return true if the node exist
	 *
	 * @param _name Name of the node
	 */
	static bool DidNodeExist(const std::string _name);
	
	/**
	 * @brief Static method, return the name of the node
	 * 
	 * \param _type Type of the node asociated with the name
	 * \return the node name
	 */
	static std::string GetNodeName(int _type);

	/**
	*@brief Static method, return the name of the condition
	* 
	* \param _type Type of the condition associed with the name
	* \return the condition name
	*/
	static std::string GetConditionName(int _type);

	/**
	 * @brief Static method, return the type of the node
	 * 
	 * \param _name Name of the node asociated with the type
	 * \return the node type
	 */
	static int GetNodeType(std::string _name) { return s_node_container_[_name]; }

	/**
	*@brief Static method, return the container of all the nodes
	* 
	*/
	static const std::unordered_map< std::string, int>& GetNodeContainer() { return s_node_container_; } ;

	/**
	 * @brief Static method, return the container of all the decorator update method
	 */
	static const DecoratorMethod& GetNodeUpdateMethod() { return s_decorator_update_method_; };


	static int GetActionNodeStart() { return s_action_node_start_; }
	static int GetDecoratorNodeStart() { return s_decorator_node_start_; }
	static int GetActionNodeEnd() { return s_action_node_end_; }
	static int GetDecoratorNodeEnd() { return s_decorator_node_end_; }

	/**
	 * @brief Clone a node and all its children
	 * 
	 * \return 
	 */
	PatronNode Clone();


	PatronNode(int _type);
	PatronNode(const PatronNode& _node, bool _invoke_copy = false);
	PatronNode(PatronNode&& _node);

	PatronNode& operator=(const PatronNode& _node) = default;


	/**
	 * @brief Create a new node and add it to the current node
	 * 
	 * \param _node Node to add
	 * \return Node added
	 */
	PatronNode* Add(const PatronNode& _node, bool _invoke_copy = false);
	/**
	 * @brief Add a existing node to the current node
	 * 
	 * \param _node Node to add
	 * \return Node added
	 */
	PatronNode* Add(PatronNode* _node);

	int GetType() const { return m_type_; }

	std::list<PatronNode*>& GetChildren() { return m_child_list_; }

	PatronNode* GetChild(int _index) { return *std::next(m_child_list_.begin(), _index); }

	PatronNode* GetParent() const { return m_p_parent_; }

	void SetParent(PatronNode* _parent) { m_p_parent_ = _parent; }

	void RemoveChild(PatronNode* _node);

	void RemoveChild(int _node_id);

	/**
	 * @brief Set the node as open
	 * 
	 * \param _is_added true if you want to open the tree
	 */
	void SetNewNodeIsAdded(bool _is_added) { m_new_node_is_added_ = _is_added; }

	/**
	 * @brief Return if the node is a child of the current node
	 * 
	 * \param _id Id of the node to check
	 * \return nullptr if the node does not exist, the node if it exist
	 */
	PatronNode* CheckIfNodeIsAChild(int _id);

	void Save(std::ofstream& _file);
	void Load(std::ifstream& _file);

	/**
	 * @brief Display the node and all its children using ImGUI TreeNode and window.
	 * This function must be called inside a ImGUI window
	 * 
	 * \param _selected_node Node selected by the user in the tree
	 * \param executionOrder Execution order of the node, 0 mean its the root node
	 * \return 
	 */
	ImRect Display(PatronNode** _selected_node, std::weak_ptr<lc::GameObject> _game_object, int _execution_order = 0);
};
