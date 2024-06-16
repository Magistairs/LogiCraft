
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

#include "BehaviorTree.h"

#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>
#include "Animation.h"
#include "Particule.h"
#include "StatSystem.h"

std::unordered_map<std::string, int> PatronNode::s_node_container_ = {};
std::unordered_map<std::string, int> PatronNode::s_condition_container_ = {};
DecoratorSaveMethod PatronNode::s_decorator_save_method_ = {};
DecoratorLoadMethod PatronNode::s_decorator_load_method_ = {};
DecoratorMethod PatronNode::s_decorator_init_method_ = {};
DecoratorMethod PatronNode::s_decorator_update_method_ = {};
DecoratorCopyMethod PatronNode::s_decorator_copy_method_ = {};

enum COMPARE_ENUM {COMP_NULL = -1,HIGHER,LOWER,EQUAL_H,EQUAL_L,EQUAL_TO,NOT_EQUAL_TO};
enum OPERATION_ENUM {OPE_NULL =-1,ADD,SUB,DIVIDE,MULTIPLY};

void PatronNode::SetupAllNode()
{
	int previous_node = 0;
	auto pushNode = [&](std::string name) { s_node_container_[name] = previous_node++; };
	auto pushCondition = [&](std::string name) { s_condition_container_[name] = s_condition_container_.size(); };

	//Here, we declare all the node possible in the behavior tree
	pushNode("SEQUENCE");
	pushNode("SELECTOR");
	pushNode("INVERSER");
	s_node_container_["INVERSER"] = 2000;
	previous_node = 2001;
	pushNode("CONDITION");
	pushNode("LOOP");
	pushNode("COOLDOWN");
	pushNode("DIRECTION");
	pushNode("DO ON ANIM FRAME");
	pushNode("FORCE SUCCESS");
	pushNode("IF STATE IS");
	pushNode("IF STAT IS");
	//
	pushNode("WANDER");
	s_node_container_["WANDER"] = 5000;
	previous_node = 5001;
	pushNode("MOVE TO");
	pushNode("PLAY ANIMATION");
	pushNode("PLAY SOUND");
	pushNode("ROTATE TO");
	pushNode("WAIT");
	pushNode("ATTACK");
	pushNode("SHOT");
	pushNode("JUMP");	
	pushNode("CHANGE STATE");
	pushNode("PLAY PARTICLES");
	pushNode("CHANGE STAT");

	s_decorator_node_start_ = s_node_container_["INVERSER"];//Were the decorator node start
	s_action_node_start_ = s_node_container_["WANDER"];//Were the action node start and were the decorator node stop (leaf node)
	s_decorator_node_end_ = s_node_container_["IF STAT IS"] + 1;
	s_action_node_end_ = s_node_container_["CHANGE STAT"] + 1;
	

	
	/*Different condition for the Condition decorator Node*/
	pushCondition("IN_RANGE_OF_PLAYER");
	pushCondition("IS_PLAYER_IN_SIGHT");
	pushCondition("CUSTOM");


	//Here, we declare all the method for the node who need more data (like loop number, cooldown timer, etc...)
	//LOOP
	{
		//Init method set the std::any to the default value
		s_decorator_init_method_["LOOP"] = [](PatronNode* node) {
			node->m_decorator_data_ = 0;
			};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["LOOP"] = [](PatronNode* node) {
			int loopNumber = std::any_cast<int>(node->m_decorator_data_);
			ImGui::InputInt("Loop Number", &loopNumber, 1,100);
			(loopNumber <= 0) ? loopNumber = 1 : loopNumber;
			node->m_decorator_data_ = loopNumber;
			};
		//Save method save the data in the file
		s_decorator_save_method_["LOOP"] = [](PatronNode* node, std::ofstream& file) {
			int loopNumber = std::any_cast<int>(node->m_decorator_data_);
			file << loopNumber;
			};
		//Load method load the data from the file
		s_decorator_load_method_["LOOP"] = [](PatronNode* node, std::ifstream& file) {
			int loopNumber;
			file >> loopNumber;
			node->m_decorator_data_ = loopNumber;
			};
	}

	//CUSTOM
	{
		//Init method set the std::any to the default value
		s_decorator_init_method_["IF STATE IS"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["IF STATE IS"] = [](PatronNode* node, PatronNode* copied_node)		{
			node->m_decorator_data_ = std::any_cast<std::string>(copied_node->m_decorator_data_);
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["IF STATE IS"] = [](PatronNode* node) {
			auto customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("State needed", customCondition,100);
			Tools::ReplaceCharacter(customCondition,' ','_');
			node->m_decorator_data_ = customCondition;
		};
		//Save method save the data in the file
		s_decorator_save_method_["IF STATE IS"] = [](PatronNode* node, std::ofstream& file) {
			std::string customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			file << customCondition;
		};
		//Load method load the data from the file
		s_decorator_load_method_["IF STATE IS"] = [](PatronNode* node, std::ifstream& file) {
			std::string customCondition;
			file >> customCondition;
			node->m_decorator_data_ = customCondition;
		};
	}

	//CUSTOM
	{
		//Init method set the std::any to the default value
		s_decorator_init_method_["CHANGE STATE"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["CHANGE STATE"] = [](PatronNode* node, PatronNode* copied_node)		{
			node->m_decorator_data_ = std::any_cast<std::string>(copied_node->m_decorator_data_);
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["CHANGE STATE"] = [](PatronNode* node) {
			auto customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("New state", customCondition,100);
			Tools::ReplaceCharacter(customCondition,' ','_');
			node->m_decorator_data_ = customCondition;
		};
		//Save method save the data in the file
		s_decorator_save_method_["CHANGE STATE"] = [](PatronNode* node, std::ofstream& file) {
			std::string customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			file << customCondition;
		};
		//Load method load the data from the file
		s_decorator_load_method_["CHANGE STATE"] = [](PatronNode* node, std::ifstream& file) {
			std::string customCondition;
			file >> customCondition;
			node->m_decorator_data_ = customCondition;
		};
	}

	//CUSTOM
	{
		//Init method set the std::any to the default value
		s_decorator_init_method_["CUSTOM"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["CUSTOM"] = [](PatronNode* node, PatronNode* copied_node)		{
			node->m_decorator_data_ = std::any_cast<std::string>(copied_node->m_decorator_data_);
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["CUSTOM"] = [](PatronNode* node) {
			auto customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("Custom condition", customCondition,100);
			Tools::ReplaceCharacter(customCondition,' ','_');
			node->m_decorator_data_ = customCondition;
		};
		//Save method save the data in the file
		s_decorator_save_method_["CUSTOM"] = [](PatronNode* node, std::ofstream& file) {
			std::string customCondition = std::any_cast<std::string>(node->m_decorator_data_);
			file << customCondition;
		};
		//Load method load the data from the file
		s_decorator_load_method_["CUSTOM"] = [](PatronNode* node, std::ifstream& file) {
			std::string customCondition;
			file >> customCondition;
			node->m_decorator_data_ = customCondition;
		};
	}
	//IF STAT IS
	{
		typedef std::tuple<std::weak_ptr<lc::StatSystem>,float,std::string,int,COMPARE_ENUM> bt_stat_is;
		//Init method set the std::any to the default value
		s_decorator_init_method_["IF STAT IS"] = [](PatronNode* node) {
			node->m_decorator_data_ = bt_stat_is(std::weak_ptr<lc::StatSystem>(),0.f,"",-1,COMP_NULL);
		};
		s_decorator_copy_method_["IF STAT IS"] = [](PatronNode* node, PatronNode* copied_node)		{
			node->m_decorator_data_ = std::any_cast<bt_stat_is>(copied_node->m_decorator_data_);
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["IF STAT IS"] = [](PatronNode* node) {
			auto tuple = std::any_cast<bt_stat_is>(node->m_decorator_data_);
			if(std::get<0>(tuple).expired() and !std::get<2>(tuple).empty())
			{
				if(!node->m_game_object_.expired())
				{
					auto object = node->m_game_object_.lock();
					for (const auto& component : object->getComponents())
					{
						if(auto stat_system = std::dynamic_pointer_cast<lc::StatSystem>(component))
						{
							if(stat_system->getName() == std::get<2>(tuple))
							{
								std::get<0>(tuple) = stat_system;
								std::get<2>(tuple) = stat_system->getName();
							}
						}					
					}
				}
			}

		
			auto object = node->m_game_object_.lock();
			if(ImGui::BeginCombo("Choose Stat System", std::get<0>(tuple).expired() ? "Select Stat System" : std::get<0>(tuple).lock()->getName().c_str()))
			{
				bool is_selected = false;
				for (const auto& component : object->getComponents())
				{
					if(auto stat_system = std::dynamic_pointer_cast<lc::StatSystem>(component))
					{
						if(ImGui::Selectable(stat_system->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
						{
							std::get<0>(tuple) = stat_system;
							std::get<2>(tuple) = stat_system->getName();
						}
					}					
				}
				ImGui::EndCombo();
			}
			
			if(!std::get<0>(tuple).expired())
			{
				const std::string type[] = {"Health","Dexterity","Range","Speed"};
				if(ImGui::BeginCombo("Stat type",std::get<3>(tuple) >= 0 ? type[std::get<3>(tuple)].c_str() : "Select type"))
				{
					for(int i = 0; i < 4;i++)
					{
						if(ImGui::Selectable(type[i].c_str()))
							std::get<3>(tuple) = i;					
					}ImGui::EndCombo();
				}

				if(std::get<3>(tuple) >= 0)
				{
					const std::string comp[] = {"HIGHER","LOWER","EQUAL_OR_HIGHER","EQUAL_OR_LOWER","EQUAL_TO","NOT_EQUAL_TO"};
				
					if(ImGui::BeginCombo("Stat Compare type",std::get<4>(tuple) >= 0 ? comp[std::get<4>(tuple)].c_str() : "Select Compare type"))
					{
						for(int i = 0; i < 6;i++)
						{
							if(ImGui::Selectable(comp[i].c_str()))
								std::get<4>(tuple) = static_cast<COMPARE_ENUM>(i);					
						}ImGui::EndCombo();
					}
				
					if(std::get<3>(tuple) == 0)
					{
						int life = std::get<1>(tuple);
						ImGui::InputInt("Value",&life);
						std::get<1>(tuple) = life;
					}
					else
						ImGui::InputFloat("Value",&std::get<1>(tuple));
				}
			}
			
			node->m_decorator_data_ = tuple;
		};
		//Save method save the data in the file
		s_decorator_save_method_["IF STAT IS"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<bt_stat_is>(node->m_decorator_data_);
			file << std::get<1>(tuple)<< " " << std::get<2>(tuple)<< " " << std::get<3>(tuple)<< " " << std::get<4>(tuple);
		};
		//Load method load the data from the file
		s_decorator_load_method_["IF STAT IS"] = [](PatronNode* node, std::ifstream& file) {
			auto tuple = bt_stat_is();
			int type = 0;
			file >> std::get<1>(tuple) >> std::get<2>(tuple) >> std::get<3>(tuple) >> type;
			std::get<4>(tuple) = static_cast<COMPARE_ENUM>(type); 
			node->m_decorator_data_ = tuple;
		};
	}
	//CHANGE STAT
	{
		typedef std::tuple<std::weak_ptr<lc::StatSystem>,float,std::string,int,OPERATION_ENUM> bt_stat_is;
		//Init method set the std::any to the default value
		s_decorator_init_method_["CHANGE STAT"] = [](PatronNode* node) {
			node->m_decorator_data_ = bt_stat_is(std::weak_ptr<lc::StatSystem>(),0.f,"",-1,OPE_NULL);
		};
		s_decorator_copy_method_["CHANGE STAT"] = [](PatronNode* node, PatronNode* copied_node)		{
			node->m_decorator_data_ = std::any_cast<bt_stat_is>(copied_node->m_decorator_data_);
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["CHANGE STAT"] = [](PatronNode* node) {
			auto tuple = std::any_cast<bt_stat_is>(node->m_decorator_data_);
			if(std::get<0>(tuple).expired() and !std::get<2>(tuple).empty())
			{
				if(!node->m_game_object_.expired())
				{
					auto object = node->m_game_object_.lock();
					for (const auto& component : object->getComponents())
					{
						if(auto stat_system = std::dynamic_pointer_cast<lc::StatSystem>(component))
						{
							if(stat_system->getName() == std::get<2>(tuple))
							{
								std::get<0>(tuple) = stat_system;
								std::get<2>(tuple) = stat_system->getName();
							}
						}					
					}
				}
			}

		
			auto object = node->m_game_object_.lock();
			if(ImGui::BeginCombo("Choose Stat System", std::get<0>(tuple).expired() ? "Select Stat System" : std::get<0>(tuple).lock()->getName().c_str()))
			{
				bool is_selected = false;
				for (const auto& component : object->getComponents())
				{
					if(auto stat_system = std::dynamic_pointer_cast<lc::StatSystem>(component))
					{
						if(ImGui::Selectable(stat_system->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
						{
							std::get<0>(tuple) = stat_system;
							std::get<2>(tuple) = stat_system->getName();
						}
					}					
				}
				ImGui::EndCombo();
			}
			
			if(!std::get<0>(tuple).expired())
			{
				const std::string type[] = {"Health","Dexterity","Range","Speed"};
				if(ImGui::BeginCombo("Stat type",std::get<3>(tuple) >= 0 ? type[std::get<3>(tuple)].c_str() : "Select type"))
				{
					for(int i = 0; i < 4;i++)
					{
						if(ImGui::Selectable(type[i].c_str()))
							std::get<3>(tuple) = i;					
					}ImGui::EndCombo();
				}

				if(std::get<3>(tuple) >= 0)
				{
					const std::string op[] = {"ADD","SUBSTRACT","DIVIDE","MULTIPLY"};
				
					if(ImGui::BeginCombo("Stat Operation type",std::get<4>(tuple) >= 0 ? op[std::get<4>(tuple)].c_str() : "Select Compare type"))
					{
						for(int i = 0; i < 4;i++)
						{
							if(ImGui::Selectable(op[i].c_str()))
								std::get<4>(tuple) = static_cast<OPERATION_ENUM>(i);					
						}ImGui::EndCombo();
					}
				
					if(std::get<3>(tuple) == 0)
					{
						int life = std::get<1>(tuple);
						ImGui::InputInt("Value",&life);
						std::get<1>(tuple) = life;
					}
					else
						ImGui::InputFloat("Value",&std::get<1>(tuple));
				}
			}
			
			node->m_decorator_data_ = tuple;
		};
		//Save method save the data in the file
		s_decorator_save_method_["CHANGE STAT"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<bt_stat_is>(node->m_decorator_data_);
			file << std::get<1>(tuple)<< " " << std::get<2>(tuple)<< " " << std::get<3>(tuple)<< " " << std::get<4>(tuple);
		};
		//Load method load the data from the file
		s_decorator_load_method_["CHANGE STAT"] = [](PatronNode* node, std::ifstream& file) {
			auto tuple = bt_stat_is();
			int type = 0;
			file >> std::get<1>(tuple) >> std::get<2>(tuple) >> std::get<3>(tuple) >> type;
			std::get<4>(tuple) = static_cast<OPERATION_ENUM>(type); 
			node->m_decorator_data_ = tuple;
		};
	}
	//DO ON ANIM FRAME
	{
		//Init method set the std::any to the default value
		typedef std::tuple<std::weak_ptr<lc::Animation>, std::string, std::string, int> onframe_anim;
		s_decorator_init_method_["DO ON ANIM FRAME"] = [](PatronNode* node) {
			node->m_decorator_data_ = onframe_anim(std::weak_ptr<lc::Animation>(),"","",-1);
			
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["DO ON ANIM FRAME"] = [](PatronNode* node) {
			onframe_anim tuple = std::any_cast<onframe_anim>(node->m_decorator_data_);

			if(std::get<0>(tuple).expired() and !std::get<2>(tuple).empty())
			{
				if(!node->m_game_object_.expired())
				{
					auto object = node->m_game_object_.lock();
					for (const auto& component : object->getComponents())
					{
						if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
						{
							if(anim->getName() == std::get<2>(tuple))
							{
								std::get<0>(tuple) = anim;
								std::get<2>(tuple) = anim->getName();
							}
						}					
					}
				}
			}
			
			if(!node->m_game_object_.expired())
			{
				auto object = node->m_game_object_.lock();
				if(ImGui::BeginCombo("Choose Animation", std::get<0>(tuple).expired() ? "Select Animation" : std::get<0>(tuple).lock()->getName().c_str()))
				{
					bool is_selected = false;
					for (const auto& component : object->getComponents())
					{
						if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
						{
							if(ImGui::Selectable(anim->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<0>(tuple) = anim;
								std::get<2>(tuple) = anim->getName();
							}
						}					
					}
					ImGui::EndCombo();
				}

				if(!std::get<0>(tuple).expired())
				{
					auto anim = std::get<0>(tuple).lock();
					if(ImGui::BeginCombo("Choose Key Animation", std::get<1>(tuple).empty() ? "Select Key Animation" : std::get<1>(tuple).c_str()))
					{
						bool is_selected = false;					
						for(auto& key_anim : anim->get_all_key_animation()){
							if(ImGui::Selectable(key_anim.first.c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<1>(tuple) = key_anim.first;
							}
						}					
						
						ImGui::EndCombo();
					}				
				}

				if(!std::get<1>(tuple).empty())
				{
					auto anim = std::get<0>(tuple).lock();
					if(ImGui::BeginCombo("Choose Key Frame", std::get<3>(tuple) == -1 ? "Select Key Frame" : std::to_string(std::get<3>(tuple)).c_str()))
					{		
						for(auto& key_anim : anim->get_all_key_animation()){
							if(key_anim.first == std::get<1>(tuple))
								for(int frame = 0; frame < key_anim.second->get_total_frame(); frame++)
									if(ImGui::Selectable(std::to_string(frame).c_str(), false,ImGuiSelectableFlags_SelectOnClick))
									{
										std::get<3>(tuple) = frame;
									}
						}					
						
						ImGui::EndCombo();
					}
				}				
			}
			node->m_decorator_data_ = tuple;	
		};
		//Save method save the data in the file
		s_decorator_save_method_["DO ON ANIM FRAME"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<onframe_anim>(node->m_decorator_data_);
			file << std::get<1>(tuple) << " " << std::get<2>(tuple) << " " << std::get<3>(tuple);
		};
		//Load method load the data from the file
		s_decorator_load_method_["DO ON ANIM FRAME"] = [](PatronNode* node, std::ifstream& file) {
			std::string anim_name,key_name;
			int frame;
			file >> key_name >> anim_name >> frame;
			node->m_decorator_data_ = onframe_anim(std::weak_ptr<lc::Animation>(),key_name,anim_name,frame);
		};
	}

	//DIRECTION
	{
		//Init method set the std::any to the default value
		s_decorator_init_method_["DIRECTION"] = [](PatronNode* node) {
			node->m_decorator_data_ = -1;
		};
		//Update method display the data and set the std::any to the new value
		s_decorator_update_method_["DIRECTION"] = [](PatronNode* node) {
			int dir = std::any_cast<int>(node->m_decorator_data_);
			std::vector<std::string> direction {"Left", "Right"};
			if(BeginCombo("Direction",dir == -1 ? "Chose a direction" : direction[dir].c_str()))
			{
				for(int i = 0;i < direction.size();i++)
				{
					if(Selectable(direction[i].c_str()))
						dir = i;
				}
				EndCombo();
			}
			node->m_decorator_data_ = dir;
		};
		//Save method save the data in the file
		s_decorator_save_method_["DIRECTION"] = [](PatronNode* node, std::ofstream& file) {
			int dir = std::any_cast<int>(node->m_decorator_data_);
			file << dir;
		};
		//Load method load the data from the file
		s_decorator_load_method_["DIRECTION"] = [](PatronNode* node, std::ifstream& file) {
			int dir;
			file >> dir;
			node->m_decorator_data_ = dir;
		};
	}
	
	//COOLDOWN
	{
		s_decorator_init_method_["COOLDOWN"] = [](PatronNode* node) {
			node->m_decorator_data_ = 0.f;
			};
		s_decorator_update_method_["COOLDOWN"] = [](PatronNode* node) {
			float timer = std::any_cast<float>(node->m_decorator_data_);
			ImGui::DragFloat("Timer of cooldown", &timer, 0.1f);
			node->m_decorator_data_ = timer;
			};
		s_decorator_save_method_["COOLDOWN"] = [](PatronNode* node, std::ofstream& file) {
			float timer = std::any_cast<float>(node->m_decorator_data_);
			file << timer << std::endl;
			};
		s_decorator_load_method_["COOLDOWN"] = [](PatronNode* node, std::ifstream& file) {
			float timer;
			file >> timer;
			node->m_decorator_data_ = timer;
			};
	}

	//CONDITION
	{
		s_decorator_init_method_["CONDITION"] = [](PatronNode* node) {
			node->m_condition_type_ = -1;
			};
		s_decorator_update_method_["CONDITION"] = [](PatronNode* node) {

			if(ImGui::BeginCombo("Condition Type", PatronNode::GetConditionName(node->m_condition_type_).c_str()))
			{
				for (auto& i : s_condition_container_)
				{
					bool isSelected = false;
					if (ImGui::Selectable(i.first.c_str(), isSelected))
					{
						node->m_condition_type_ = i.second;
						if (s_decorator_init_method_[GetConditionName(i.second)])
							s_decorator_init_method_[GetConditionName(i.second)](node);
					}
				}
				ImGui::EndCombo();
			}

			if (s_decorator_update_method_[GetConditionName(node->m_condition_type_)])
				s_decorator_update_method_[GetConditionName(node->m_condition_type_)](node);

			};
		s_decorator_save_method_["CONDITION"] = [](PatronNode* node, std::ofstream& file) {
			file << node->m_condition_type_ << " ";
			if (s_decorator_save_method_[GetConditionName(node->m_condition_type_)])
				s_decorator_save_method_[GetConditionName(node->m_condition_type_)](node, file);
			};
		s_decorator_load_method_["CONDITION"] = [](PatronNode* node, std::ifstream& file) {
			file >> node->m_condition_type_;
			if (s_decorator_load_method_[GetConditionName(node->m_condition_type_)])
				s_decorator_load_method_[GetConditionName(node->m_condition_type_)](node, file);
			};
	}

	//MOVE TO
	{
		s_decorator_init_method_["MOVE TO"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["MOVE TO"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decorator_data_ = std::any_cast<std::string>(node_copied->m_decorator_data_);
		};
		s_decorator_update_method_["MOVE TO"] = [](PatronNode* node) {
			auto TAG = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("TAG", TAG, 100);
			node->m_decorator_data_ = TAG;
		};
		s_decorator_save_method_["MOVE TO"] = [](PatronNode* node, std::ofstream& file) {
			auto tag = std::any_cast<std::string>(node->m_decorator_data_);
			file << tag;
		};
		s_decorator_load_method_["MOVE TO"] = [](PatronNode* node, std::ifstream& file){
			std::string tag;
			file >> tag;
			node->m_decorator_data_ = tag;
		};
	}

	//IN_RANGE_OF_PLAYER
    	{
    		s_decorator_init_method_["IN_RANGE_OF_PLAYER"] = [](PatronNode* node) {
    			node->m_decorator_data_ = 100;
    			};
    		s_decorator_update_method_["IN_RANGE_OF_PLAYER"] = [](PatronNode* node) {
    			int range = std::any_cast<int>(node->m_decorator_data_);
    			ImGui::SliderInt("Range", &range, 100, 1500);
    			node->m_decorator_data_ = range;
    			};
    		s_decorator_save_method_["IN_RANGE_OF_PLAYER"] = [](PatronNode* node, std::ofstream& file) {
    			int range = std::any_cast<int>(node->m_decorator_data_);
    			file << range;
    
    			};
    		s_decorator_load_method_["IN_RANGE_OF_PLAYER"] = [](PatronNode* node, std::ifstream& file) {
    			int range;
    			file >> range;
    			node->m_decorator_data_ = range;
    			};
    	}

	//JUMP
	{
		s_decorator_init_method_["JUMP"] = [](PatronNode* node) {
			node->m_decorator_data_ = 100.f;
			};
		s_decorator_update_method_["JUMP"] = [](PatronNode* node) {
			auto jump_height = std::any_cast<float>(node->m_decorator_data_);
			ImGui::DragFloat("Jump Height", &jump_height);
			node->m_decorator_data_ = jump_height;
			};
		s_decorator_save_method_["JUMP"] = [](PatronNode* node, std::ofstream& file) {
			auto jump_height = std::any_cast<float>(node->m_decorator_data_);
			file << jump_height;

			};
		s_decorator_load_method_["JUMP"] = [](PatronNode* node, std::ifstream& file) {
			float jump_height;
			file >> jump_height;
			node->m_decorator_data_ = jump_height;
			};
	}


	//PLAY_ANIMATION
	{
		typedef std::tuple<std::weak_ptr<lc::Animation>, std::string, std::string, bool, bool, bool> bt_anim;
		s_decorator_init_method_["PLAY ANIMATION"] = [](PatronNode* node) {
			node->m_decorator_data_ = bt_anim();
			};
		s_decorator_copy_method_["PLAY ANIMATION"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decorator_data_ = std::any_cast<bt_anim>(node_copied->m_decorator_data_);
			
			};
		s_decorator_update_method_["PLAY ANIMATION"] = [](PatronNode* node) {			
			bt_anim tuple = std::any_cast<bt_anim>(node->m_decorator_data_);

			if(std::get<0>(tuple).expired() and !std::get<2>(tuple).empty())
			{
				if(!node->m_game_object_.expired())
				{
					auto object = node->m_game_object_.lock();
					for (const auto& component : object->getComponents())
					{
						if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
						{
							if(anim->getName() == std::get<2>(tuple))
							{
								std::get<0>(tuple) = anim;
								std::get<2>(tuple) = anim->getName();
							}
						}					
					}
				}
			}
			
			if(!node->m_game_object_.expired())
			{
				auto object = node->m_game_object_.lock();
				if(ImGui::BeginCombo("Choose Animation", std::get<0>(tuple).expired() ? "Select Animation" : std::get<0>(tuple).lock()->getName().c_str()))
				{
					bool is_selected = false;
					for (const auto& component : object->getComponents())
					{
						if(auto anim = std::dynamic_pointer_cast<lc::Animation>(component))
						{
							if(ImGui::Selectable(anim->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<0>(tuple) = anim;
								std::get<2>(tuple) = anim->getName();
							}
						}					
					}
					ImGui::EndCombo();
				}

				if(!std::get<0>(tuple).expired())
				{
					auto anim = std::get<0>(tuple).lock();
					if(ImGui::BeginCombo("Choose Key Animation", std::get<1>(tuple).empty() ? "Select Key Animation" : std::get<1>(tuple).c_str()))
					{
						bool is_selected = false;					
						for(auto& key_anim : anim->get_all_key_animation()){
							if(ImGui::Selectable(key_anim.first.c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<1>(tuple) = key_anim.first;
							}
						}					
						
						ImGui::EndCombo();
					}				
				}

				if(!std::get<1>(tuple).empty())
				{
					ImGui::Checkbox("Stop at last frame", &std::get<3>(tuple));
					ImGui::Checkbox("Revert animation", &std::get<4>(tuple));
					ImGui::Checkbox("Reset animation", &std::get<5>(tuple));

				}				
			}
			node->m_decorator_data_ = tuple;			
			};
		s_decorator_save_method_["PLAY ANIMATION"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<bt_anim>(node->m_decorator_data_);
			file << std::get<2>(tuple) << " " << std::get<1>(tuple) << " " << std::get<3>(tuple) << " " << std::get<4>(tuple);
			};
		s_decorator_load_method_["PLAY ANIMATION"] = [](PatronNode* node, std::ifstream& file) {
			auto tuple = bt_anim();
			file >> std::get<2>(tuple) >> std::get<1>(tuple) >> std::get<3>(tuple) >> std::get<4>(tuple);
			node->m_decorator_data_ = tuple;
			};
	}

	//PLAY_SOUND
	{
		s_decorator_init_method_["PLAY SOUND"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::tuple<std::string,bool,float,float>();
			};
		s_decorator_copy_method_["PLAY SOUND"] = [](PatronNode* node, PatronNode* node_copied) {
				node->m_decorator_data_ = std::any_cast<std::tuple<std::string,bool,float,float>>(node_copied->m_decorator_data_);
				};
		s_decorator_update_method_["PLAY SOUND"] = [](PatronNode* node) {
			auto tuple = std::any_cast<std::tuple<std::string,bool,float,float>>(node->m_decorator_data_);
			ImGui::InputText("Sound Name", std::get<0>(tuple), 100);
			ImGui::Checkbox("Start New Sound", &std::get<1>(tuple));
			ImGui::DragFloat("Attenuation", &std::get<2>(tuple));
			ImGui::DragFloat("Minimum Distance", &std::get<3>(tuple));
			node->m_decorator_data_ = tuple;
			};
		s_decorator_save_method_["PLAY SOUND"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<std::tuple<std::string,bool,float,float>>(node->m_decorator_data_);
			file << std::get<0>(tuple) << " " << std::get<1>(tuple) << " " << std::get<2>(tuple) << " " << std::get<3>(tuple);

			};
		s_decorator_load_method_["PLAY SOUND"] = [](PatronNode* node, std::ifstream& file) {
			std::tuple<std::string,bool,float,float> tuple;
			file >> std::get<0>(tuple) >> std::get<1>(tuple) >> std::get<2>(tuple) >> std::get<3>(tuple);
			node->m_decorator_data_ = tuple;
			};
	}
	
	//PLAY_PARTICLES
	{
		typedef std::tuple<std::weak_ptr<lc::Particles>,std::string, char> bt_particle;
		s_decorator_init_method_["PLAY PARTICLES"] = [](PatronNode* node) {
			node->m_decorator_data_ = bt_particle();
		};
		s_decorator_copy_method_["PLAY PARTICLES"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decorator_data_ = std::any_cast<bt_particle>(node_copied->m_decorator_data_);
		};
		s_decorator_update_method_["PLAY PARTICLES"] = [](PatronNode* node) {
			auto tuple = std::any_cast<bt_particle>(node->m_decorator_data_);
			if(std::get<0>(tuple).expired() and !std::get<1>(tuple).empty())
			{
				if(!node->m_game_object_.expired())
				{
					auto object = node->m_game_object_.lock();
					for (const auto& component : object->getComponents())
					{
						if(auto particles = std::dynamic_pointer_cast<lc::Particles>(component))
						{
							if(particles->getName() == std::get<1>(tuple))
							{
								std::get<0>(tuple) = particles;
								std::get<1>(tuple) = particles->getName();
							}
						}					
					}
				}
			}
			if(!node->m_game_object_.expired())
			{
				auto object = node->m_game_object_.lock();
				if(ImGui::BeginCombo("Choose Particles", std::get<0>(tuple).expired() ? "Select Particles" : std::get<0>(tuple).lock()->getName().c_str()))
				{
					bool is_selected = false;
					for (const auto& component : object->getComponents())
					{
						if(auto particles = std::dynamic_pointer_cast<lc::Particles>(component))
						{
							if(ImGui::Selectable(particles->getName().c_str(),&is_selected,ImGuiSelectableFlags_SelectOnClick))
							{
								std::get<0>(tuple) = particles;
								std::get<1>(tuple) = particles->getName();
							}
						}					
					}
					ImGui::EndCombo();
				}				
			}		
			node->m_decorator_data_ = tuple;
		};
		s_decorator_save_method_["PLAY PARTICLES"] = [](PatronNode* node, std::ofstream& file) {
			auto tuple = std::any_cast<bt_particle>(node->m_decorator_data_);
			file << std::get<1>(tuple) << " ";

		};
		s_decorator_load_method_["PLAY PARTICLES"] = [](PatronNode* node, std::ifstream& file) {
			bt_particle tuple;
			file >> std::get<1>(tuple);
			node->m_decorator_data_ = tuple;
		};
	}

	//ROTATE_TO
	{
		s_decorator_init_method_["ROTATE TO"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
			};
		s_decorator_copy_method_["ROTATE TO"] = [](PatronNode* node, PatronNode* node_copied) {
			node->m_decorator_data_ = std::any_cast<std::string>(node_copied->m_decorator_data_);
			};
		s_decorator_update_method_["ROTATE TO"] = [](PatronNode* node) {
			auto TAG = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("TAG", TAG, 100);
			node->m_decorator_data_ = TAG;
			};
		s_decorator_save_method_["ROTATE TO"] = [](PatronNode* node, std::ofstream& file) {
			auto tag = std::any_cast<std::string>(node->m_decorator_data_);
			file << tag;

			};
		s_decorator_load_method_["ROTATE TO"] = [](PatronNode* node, std::ifstream& file) {
			std::string tag;
			file >> tag;
			node->m_decorator_data_ = tag;
			};
	}

	//WAIT
	{
		s_decorator_init_method_["WAIT"] = [](PatronNode* node) {
			node->m_decorator_data_ = float(0.f);
			};
		s_decorator_update_method_["WAIT"] = [](PatronNode* node) {
			float timer = std::any_cast<float>(node->m_decorator_data_);
			ImGui::DragFloat("Timer of wait", &timer, 0.1f);
			node->m_decorator_data_ = timer;
			};
		s_decorator_save_method_["WAIT"] = [](PatronNode* node, std::ofstream& file) {
			float timer = std::any_cast<float>(node->m_decorator_data_);
			file << timer;
			};
		s_decorator_load_method_["WAIT"] = [](PatronNode* node, std::ifstream& file) {
			float timer;
			file >> timer;
			node->m_decorator_data_ = timer;
			};
	}

	//ATTACK
	{
		s_decorator_init_method_["ATTACK"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["ATTACK"] = [](PatronNode* node, PatronNode* nodeCopied){
			node->m_decorator_data_ = std::any_cast<std::string>(nodeCopied->m_decorator_data_);
		};
		s_decorator_update_method_["ATTACK"] = [](PatronNode* node) {
			std::string attack_name = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("Attack Name", attack_name, 150);
			node->m_decorator_data_ = attack_name;
		};
		s_decorator_save_method_["ATTACK"] = [](PatronNode* node, std::ofstream& file) {
			std::string	attack_name = std::any_cast<std::string>(node->m_decorator_data_);
			file << attack_name;
		};
		s_decorator_load_method_["ATTACK"] = [](PatronNode* node, std::ifstream& file) {
			std::string attack_name;
			file >> attack_name;
			node->m_decorator_data_ = attack_name;
		};
	}

	//SHOT
	{
		s_decorator_init_method_["SHOT"] = [](PatronNode* node) {
			node->m_decorator_data_ = std::string();
		};
		s_decorator_copy_method_["SHOT"] = [](PatronNode* node, PatronNode* nodeCopied){
			node->m_decorator_data_ = std::any_cast<std::string>(nodeCopied->m_decorator_data_);
		};
		s_decorator_update_method_["SHOT"] = [](PatronNode* node) {
			std::string attack_name = std::any_cast<std::string>(node->m_decorator_data_);
			ImGui::InputText("Attack Name", attack_name, 150);
			node->m_decorator_data_ = attack_name;
		};
		s_decorator_save_method_["SHOT"] = [](PatronNode* node, std::ofstream& file) {
			std::string	attack_name = std::any_cast<std::string>(node->m_decorator_data_);
			file << attack_name;
		};
		s_decorator_load_method_["SHOT"] = [](PatronNode* node, std::ifstream& file) {
			std::string attack_name;
			file >> attack_name;
			node->m_decorator_data_ = attack_name;
		};
	}
}

bool PatronNode::DidNodeExist(const std::string _name)
{
	if(_name == "NONE")
		return false;
	if(s_node_container_[_name] || s_node_container_[_name] == 0)
		return true;
	return false;
}

std::string PatronNode::GetNodeName(int _type)
{
	 for (auto& i : s_node_container_)
	 { 
		 if (i.second == _type)
			 return i.first; 
	 } return "NONE";
}

std::string PatronNode::GetConditionName(int _type)
{
	for (auto& i : s_condition_container_)
	{
		if (i.second == _type)
			return i.first;
	} return "NONE";
}

PatronNode PatronNode::Clone()
{
	return  PatronNode(*this,true);
}

PatronNode::PatronNode(int _type) : m_type_(_type), m_id_(s_id_++), m_p_parent_(nullptr)
{
	m_condition_type_ = -1;
	if (s_decorator_init_method_[GetNodeName(m_type_)])
		s_decorator_init_method_[GetNodeName(m_type_)](this);
	
}

PatronNode::PatronNode(const PatronNode& _node, bool _invoke_copy)
{
	m_type_ = _node.m_type_;
	m_condition_type_ = _node.m_condition_type_;
	m_decorator_data_ = _node.m_decorator_data_;
	m_id_ = s_id_++;
	m_is_open_ = _node.m_is_open_;
	m_new_node_is_added_ = _node.m_new_node_is_added_;
	m_p_parent_ = nullptr;
	m_child_list_.clear();
	for (auto& i : _node.m_child_list_)
	{
		Add(*i,_invoke_copy);
	}

	if(!_invoke_copy)
	{
		if (s_decorator_init_method_[GetNodeName(m_type_)])		
			s_decorator_init_method_[GetNodeName(m_type_)](this);
	}
	else
	{
		if (s_decorator_copy_method_[GetNodeName(m_type_)])
			s_decorator_copy_method_[GetNodeName(m_type_)](this, const_cast<PatronNode*>(&_node));
	}
}

PatronNode::PatronNode(PatronNode&& _node)
{
	m_type_ = _node.m_type_;
	m_condition_type_ = _node.m_condition_type_;
	m_decorator_data_ = _node.m_decorator_data_;
	m_id_ = s_id_++;
	m_is_open_ = _node.m_is_open_;
	m_new_node_is_added_ = _node.m_new_node_is_added_;
	m_p_parent_ = nullptr;
	m_child_list_.clear();
	for (auto& i : _node.m_child_list_)
	{
		Add(*i);
	}

	if (s_decorator_copy_method_[GetNodeName(m_type_)])
		s_decorator_copy_method_[GetNodeName(m_type_)](this, &_node);
}

PatronNode* PatronNode::Add(const PatronNode& _node, bool _invoke_copy)
{

	if (_node.m_type_ >= 0 and this->m_type_ < s_action_node_start_)
	{
		m_child_list_.push_back(new PatronNode(_node,_invoke_copy));
		m_child_list_.back()->m_p_parent_ = this;
		return m_child_list_.back();
	}
	else
	{
		std::cout << "Invalid Node Type" << std::endl;
		return this;
	}
}

PatronNode* PatronNode::Add(PatronNode* _node)
{
	if (_node->m_type_ >= 0 and this->m_type_ < s_action_node_start_)
	{
		m_child_list_.push_back(_node);
		m_child_list_.back()->m_p_parent_ = this;
		return m_child_list_.back();
	}
	else
	{
		std::cout << "Invalid Node Type" << std::endl;
		return this;
	}
}


void PatronNode::RemoveChild(PatronNode* _node)
{
	m_child_list_.remove_if([&](PatronNode*& node_) {return node_ == _node; });	
}

void PatronNode::RemoveChild(int _node_id)
{
	m_child_list_.remove_if([&](PatronNode*& node_) {return node_->m_id_ == _node_id; });
}



PatronNode* PatronNode::CheckIfNodeIsAChild(int _id)
{
	for (auto& i : m_child_list_)
	{
		if (i->m_id_ == _id)
			return i;
	}

	for (auto& i : m_child_list_)
	{
		auto tmp = i->CheckIfNodeIsAChild(_id);
		if (tmp)
			return tmp;
	}

	return nullptr;
}

void PatronNode::Save(std::ofstream& _file)
{
	_file << m_type_ << " " << static_cast<int>(m_child_list_.size());
	if(s_decorator_save_method_[GetNodeName(m_type_)])
	{
		_file << " ";
		s_decorator_save_method_[GetNodeName(m_type_)](this, _file);
	}
	_file << std::endl;
	for (auto& i : m_child_list_)
	{
		i->Save(_file);
	}
}

void PatronNode::Load(std::ifstream& _file)
{
	int size;
	_file >> m_type_ >> size;
	if (s_decorator_load_method_[GetNodeName(m_type_)])
		s_decorator_load_method_[GetNodeName(m_type_)](this, _file);
	for (int i = 0; i < size; i++)
	{
		PatronNode tmp(0);
		tmp.Load(_file);
		m_child_list_.push_back(new PatronNode(tmp, true));
		m_child_list_.back()->m_p_parent_ = this;
	}
}

ImRect PatronNode::Display(PatronNode** _selected_node, std::weak_ptr<lc::GameObject> _game_object, int _execution_order)
{
	if(m_game_object_.expired())
	{
		m_game_object_ = _game_object;		
	}
	if (*_selected_node == this)//If the node is selected, change the color of the node
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31 / 255.f, 57 / 255.f, 88.f / 255.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(31 / 255.f, 57 / 255.f, 88.f / 255.f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.f));
	}


	if (ImGui::TreeNodeEx((PatronNode::GetNodeName(m_type_) + (_execution_order ? " " + std::to_string(_execution_order) : "") + "## " + std::to_string(m_id_)).c_str(),
		ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		m_is_open_ = true;
	} else m_is_open_ = false;

	if(m_new_node_is_added_)//Automatically open the tree if a new node is added
	{
		ImGui::TreeNodeSetOpen(ImGui::GetID((PatronNode::GetNodeName(m_type_) + (_execution_order ? " " + std::to_string(_execution_order) : "") + "## " + std::to_string(m_id_)).c_str()), true);
		m_new_node_is_added_ = false;
	}

	const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		*_selected_node = this;
	}

	if(_execution_order != 0)//If the node is not the root node, the user can drag and drop the node
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			auto tmp = this;
			ImGui::SetDragDropPayload("TREE_NODE", &tmp, sizeof(this));
			ImGui::Text(PatronNode::GetNodeName(m_type_).c_str());
			ImGui::EndDragDropSource();
		}

		if (this != *std::prev(GetParent()->GetChildren().end()))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f);
			ImGui::SameLine();
			if(ImGui::ArrowButtonEx(std::string("Down##" + std::to_string(m_id_)).c_str(),ImGuiDir_Down,sf::Vector2f(15.f, 13.f)))
			{
				auto patron_node = std::find_if(GetParent()->GetChildren().begin(), GetParent()->GetChildren().end(), [&](PatronNode*& node_)
					{
						return (node_->m_id_ == this->m_id_);
					});

				auto tmp_changePN = *patron_node;
				*patron_node = *std::next(patron_node);
				*std::next(patron_node) = tmp_changePN;
			}			
		}
		if(this != *GetParent()->GetChildren().begin())
		{
			ImGui::SameLine();
			if(ImGui::ArrowButtonEx(std::string("Up##" + std::to_string(m_id_)).c_str(),ImGuiDir_Up,sf::Vector2f(15.f, 13.f)))
			{
				auto patron_node = std::find_if(GetParent()->GetChildren().begin(), GetParent()->GetChildren().end(), [&](PatronNode*& node_)
					{
						return (node_->m_id_ == this->m_id_);
					});

				auto tmp_changePN = *patron_node;
				*patron_node = *std::prev(patron_node);
				*std::prev(patron_node) = tmp_changePN;
			}
		}
	}
	if (m_type_ < s_decorator_node_start_ or (m_type_ >= PatronNode::GetDecoratorNodeStart() and m_type_ < PatronNode::GetActionNodeStart() and GetChildren().empty())) //if the node if not a leaf node or the decorator node is empty, we can drop the node dragged into it
		if (BeginDragDropTarget())
		{
			if (auto tmp_playLoad = ImGui::AcceptDragDropPayload("TREE_NODE"))
				if (auto node = reinterpret_cast<PatronNode**>(tmp_playLoad->Data))
				{
					auto parent = (*node)->m_p_parent_;
					if (this->CheckIfNodeIsAChild(m_id_))
					{
						if(m_is_open_)
						{
							ImGui::TreePop();
						}
						PopStyleColor(2);
						return nodeRect;

					}

					if(this != parent)
					{
						parent->RemoveChild((*node)->m_id_);
						this->Add((*node))->SetNewNodeIsAdded(true);
						this->SetNewNodeIsAdded(true);
						*_selected_node = this;
					}
				}
			ImGui::EndDragDropTarget();
		}
	
	if (m_is_open_)
	{
		
		
		//Used to display the line between the parent and the child node
		const ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
		const float SmallOffsetX = -10.0f; //for now, a hardcoded value; should take into account tree indent size
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
		verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
		ImVec2 verticalLineEnd = verticalLineStart;
		
		int index = 1;
		for (auto child : m_child_list_)
		{
			const float HorizontalTreeLineSize = 10.0f; //chosen arbitrarily
			const ImRect childRect = child->Display(_selected_node, _game_object,index);
			const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f - 1.f;
			drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
			verticalLineEnd.y = midpoint;
			index++;
		}

		drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

		
		

		ImGui::TreePop();
	}
	PopStyleColor(2);

	return nodeRect;
}
