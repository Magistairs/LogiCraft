
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

#include "AI.h"
#include "GameObject.h"

lc::AI::AI() : m_root(PatronNode::GetNodeType("SEQUENCE"))
{
    m_type = TYPE::IA;
    m_typeName = "IA";
    m_name = "AI";
}

lc::AI::AI(AI& other) : m_root(0)
{
    m_root = other.m_root.Clone();
    m_aiName = other.m_aiName;
    m_type = other.m_type;
    m_typeName = other.m_typeName;
    m_name = other.m_name;
    std::function<void(PatronNode*)> reparenting = [&](PatronNode* node)
    {
        for (auto& i : node->GetChildren())
        {
            i->SetParent(node);
            reparenting(i);
        }
    };
            
    reparenting(&m_root);
}

lc::AI::~AI()
{
}

void lc::AI::Save(std::ofstream& save, sf::RenderTexture& texture, int _depth)
{
    save << static_cast<int>(m_type) << " " << Tools::replaceSpace(m_typeName, true) << std::endl;
	m_root.Save(save);
}

void lc::AI::Export(std::ofstream& exportation)
{
    exportation << static_cast<int>(m_type) << " " << std::endl;
    m_root.Save(exportation);
}


void lc::AI::Load(std::ifstream& load)
{
    load >> m_typeName;
    m_root.Load(load);
}


void lc::AI::UpdateEvent(sf::Event& _window)
{
}

void lc::AI::Update(WindowManager& _window)
{
    if (m_isEditionBehaviorTree)
    {
        if (ImGui::Begin(("Behavior Tree Information##lc" + getParent()->getName()).c_str(), &m_isEditionBehaviorTree, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::SetWindowPos(_window.getSize() / 4 - sf::Vector2i(ImGui::GetWindowSize()) / 4, ImGuiCond_Once);
            ImGui::SetWindowSize(sf::Vector2f(_window.getSize() / 2), ImGuiCond_Once);

            ImGui::BeginChild("Tree information##lc", ImVec2((ImGui::GetWindowSize().x / 2) - 20, ImGui::GetWindowSize().y - 40), true, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            m_root.Display(&m_selectedNode,getParent());
            ImGui::EndChild();



            ImGui::SameLine();
            ImGui::BeginChild("Node information##lc", ImVec2((ImGui::GetWindowSize().x / 2) - 20, ImGui::GetWindowSize().y - 40), true);
            this->UpdateSelectedNode();
            ImGui::EndChild();

           
		}
		ImGui::End();
	} 
    if (m_wantToCopyATreeNode)
    {
        auto scene = getParent();
        while (scene->getParent())
        {
            scene = scene->getParent();
        }
        this->CopyTreeNode(scene);
    }
}



void lc::AI::Draw(WindowManager& _window)
{
}

void lc::AI::Draw(sf::RenderTexture& _window)
{
}

std::shared_ptr<lc::GameComponent> lc::AI::Clone()
{
    return std::make_shared<lc::AI>(*this);
}

void lc::AI::setHierarchieFunc()
{
    m_hierarchieInformation = [&] {
        if (ImGui::Button("Edit Behavior Tree"))
        {
			m_isEditionBehaviorTree = !m_isEditionBehaviorTree;
            m_wantToCopyATreeNode = false;
        }
        if (ImGui::Button("Copy TreeNode"))
        {
			m_wantToCopyATreeNode = !m_wantToCopyATreeNode;
            m_isEditionBehaviorTree = false;
		}

    };
}

void lc::AI::UpdateSelectedNode()
{
    if (m_selectedNode)
    {
        ImGui::Text("Node Type : %s", PatronNode::GetNodeName(m_selectedNode->GetType()).c_str());

        if (m_selectedNode->GetType() < PatronNode::GetDecoratorNodeStart() or (m_selectedNode->GetType() >= PatronNode::GetDecoratorNodeStart() and m_selectedNode->GetType() < PatronNode::GetActionNodeStart() and m_selectedNode->GetChildren().empty())) //if the node if not a leaf node or the decorator node is empty, we can add a node to it
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.f);

            if (ImGui::BeginCombo("Node Type", PatronNode::GetNodeName(m_newNodeType).c_str())) //Make a combo with all the node type except the decorator
            {
                bool isSelected = false;

                for (int i = 0; i < PatronNode::GetActionNodeEnd(); i++)
                {
                    if(!PatronNode::DidNodeExist(PatronNode::GetNodeName(i)))
                        continue;
                    if(i < PatronNode::GetDecoratorNodeStart() || i >= PatronNode::GetActionNodeStart())
                        if (ImGui::Selectable(PatronNode::GetNodeName(i).c_str(), &isSelected, ImGuiSelectableFlags_SelectOnClick))
                        {
                            m_newNodeType = i;
                        }
                }


                ImGui::EndCombo();
            }

            if (ImGui::Button("Add Node"))
            {
                this->AddNode(m_newNodeType);
            }
        }

        if (m_selectedNode->GetParent()) //If the node has a parent, we can remove it (so we can't remove the root node)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.f);
            if (ImGui::Button("Delete Node"))
            {
                this->RemoveNode();
            }


            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.f);
            m_newDecorateType = MakeCombo(PatronNode::GetDecoratorNodeStart(), PatronNode::GetDecoratorNodeEnd(), m_newDecorateType, "Decorator Type"); // Make a combo with all the decorator node type

            if (ImGui::Button("Add Decorator"))
            {
                this->AddDecorateNode(m_newDecorateType);
            }
            if (m_selectedNode->GetType() >= PatronNode::GetDecoratorNodeStart() && m_selectedNode->GetType() < PatronNode::GetActionNodeStart())
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.f);
                if (ImGui::Button("Remove Decorator"))
                {
                    this->RemoveDecorateNode();
                }
            } 

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.f);
            auto t = PatronNode::GetNodeUpdateMethod();
            if (t[PatronNode::GetNodeName(m_selectedNode->GetType())])//Check if the node need an update method
            {
                t[PatronNode::GetNodeName(m_selectedNode->GetType())](m_selectedNode);//Update the node with the correct method if they need it
			}
			
        }
	}
}

void lc::AI::AddNode(int type)
{
    if (m_selectedNode)
    {
        m_selectedNode->SetNewNodeIsAdded(true);
		m_selectedNode->Add(PatronNode(type));
	}
}

void lc::AI::RemoveNode()
{
    auto NewNode = m_selectedNode->GetParent();
    NewNode->RemoveChild(m_selectedNode);
    m_selectedNode = NewNode;
}

void lc::AI::AddDecorateNode(int type)
{
    auto nodeDecorated = m_selectedNode;
    auto parentNode = m_selectedNode->GetParent();
    m_selectedNode = parentNode->Add(type);
    m_selectedNode->SetNewNodeIsAdded(true);
    m_selectedNode->Add(*nodeDecorated);
    parentNode->RemoveChild(nodeDecorated);
}

void lc::AI::RemoveDecorateNode()
{
    if (m_selectedNode->GetParent())
    {
        auto NewNode = m_selectedNode->GetParent();
        auto children = m_selectedNode->GetChildren();
        for (auto& i : children)
        {
            NewNode->Add(i);
        }
        NewNode->RemoveChild(m_selectedNode);
        m_selectedNode = NewNode;
    }
}

void lc::AI::CopyTreeNode(std::shared_ptr<lc::GameObject> _game_object)
{
    //Recursive lambda function to check all the children of the gameobject and make a ImGui combo box to select the tree node to copy
    std::function<void(std::shared_ptr<lc::GameObject>, std::pair<std::weak_ptr<GameObject>,PatronNode*>*)> checkChild = [&](std::shared_ptr<lc::GameObject> object, std::pair<std::weak_ptr<GameObject>, PatronNode*>* nodeToCopy) {

        for (auto& i : object->getObjects())
        {
            if (i->hasComponent("AI") and &*i->getComponent<lc::AI>("AI") != this)//Check if the gameobject has a AI component and if it is not the current AI component
            {           
                if(ImGui::Selectable(i->getName().c_str()))
                {
                    nodeToCopy->second = &i->getComponent<lc::AI>("AI")->m_root;
                    nodeToCopy->first = i;
                }
			}
            checkChild(i,nodeToCopy);
		}
	};
    ImGui::Begin("Copy TreeNode##lc", &m_wantToCopyATreeNode, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(sf::Vector2f(ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowSize().x / 2, ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowSize().y / 2));
    
    if(ImGui::BeginCombo("TreeNode to copy", (m_copiedNode.first.lock() ? m_copiedNode.first.lock()->getName().c_str() : "")))
    {
        checkChild(_game_object, &m_copiedNode);
        ImGui::EndCombo();
    }
    if (ImGui::Button("Copy"))
    {
        if (m_copiedNode.second)//if the copied node is not null we proceed to the copy
        {
            m_root = m_copiedNode.second->Clone();
            std::function<void(PatronNode*)> reparenting = [&](PatronNode* node)
            {
            	for (auto& i : node->GetChildren())
            	{
                    i->SetParent(node);
					reparenting(i);
				}
			};
            
            reparenting(&m_root);

		}
        m_copiedNode = {};
		m_wantToCopyATreeNode = false;
	}
    ImGui::End();
}

int lc::AI::MakeCombo(int start, int end, int DefaultValue, std::string name)
{
    int type = DefaultValue;
    if (ImGui::BeginCombo(name.c_str(), PatronNode::GetNodeName(DefaultValue).c_str()))
    {
        bool isSelected = false;

        for (int i = start; i < end; i++)
        {
            if (ImGui::Selectable(PatronNode::GetNodeName(i).c_str(), &isSelected, ImGuiSelectableFlags_SelectOnClick))
            {
                type = i;
            }
        }


        ImGui::EndCombo();
    }
    return type;
}
