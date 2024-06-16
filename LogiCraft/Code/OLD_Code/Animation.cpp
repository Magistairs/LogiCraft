#include "Animation.h"

#include <ranges>


namespace lc
{
	AnimationKey::AnimationKey()
		: m_total_frame_(0), m_actual_frame_(0), m_frame_timer_(0.f), m_frame_time_(0.f)
	{
	}

	AnimationKey::AnimationKey(const std::string& name, const int& total_frame, const sf::Vector2i& max_frame, const float& frame_time, const sf::IntRect& frame_rect)
		: m_name_(name), m_base_frame_rect_(frame_rect), m_max_frame_(max_frame), m_total_frame_(total_frame), m_actual_frame_(0), m_frame_timer_(0.f),
		  m_frame_time_(frame_time)
	{
		this->create_frames_rect();
	}

	AnimationKey::~AnimationKey() = default;

	std::string& AnimationKey::get_name()
	{
		return m_name_;
	}

	std::vector<sf::IntRect>& AnimationKey::get_frames_rects()
	{
		return m_frames_rects_;
	}

	sf::IntRect& AnimationKey::get_base_int_rect()
	{
		return m_base_frame_rect_;
	}

	sf::Vector2i& AnimationKey::get_max_frame()
	{
		return m_max_frame_;
	}

	int& AnimationKey::get_total_frame()
	{
		return m_total_frame_;
	}

	int& AnimationKey::get_actual_frame()
	{
		return m_actual_frame_;
	}

	float& AnimationKey::get_frame_timer()
	{
		return m_frame_timer_;
	}

	float& AnimationKey::get_frame_time()
	{
		return m_frame_time_;
	}

	void AnimationKey::create_frames_rect()
	{
		m_frames_rects_.clear();

		int tmp_x(0);
		int tmp_y(0);
		for (int i = 0; i < m_total_frame_; i++)
		{
			if (m_max_frame_.x != 0)
				if (i % m_max_frame_.x == 0 && i != 0)
				{
					tmp_x = 0;
					tmp_y++;
				}

			m_frames_rects_.emplace_back(
				m_base_frame_rect_.left + (m_base_frame_rect_.width * tmp_x),
				m_base_frame_rect_.top + (m_base_frame_rect_.height * tmp_y),
				m_base_frame_rect_.width, m_base_frame_rect_.height);

			tmp_x++;
		}
	}

	void AnimationKey::update_animation_key(const std::shared_ptr<lc::Texture>& texture, 
	const bool& animation_is_paused, const bool& animation_is_reversed,
		const bool& stop_at_last_frame)
	{
		if ((!animation_is_paused && !stop_at_last_frame) || (!animation_is_paused && stop_at_last_frame
			&& m_actual_frame_ != m_total_frame_ - 1))
		{
			m_frame_timer_ += Tools::getDeltaTime();
			if (m_frame_timer_ >= m_frame_time_)
			{
				animation_is_reversed ? m_actual_frame_-- : m_actual_frame_++;

				if (m_actual_frame_ < 0 && animation_is_reversed)
					m_actual_frame_ = m_total_frame_ - 1;
				else if (m_actual_frame_ > m_total_frame_ - 1 && !animation_is_reversed)
					m_actual_frame_ = 0;

				m_frame_timer_ = 0.f;
			}

			if (!m_frames_rects_.empty())
			{
				texture->getShape().setTextureRect(m_frames_rects_[m_actual_frame_]);
				texture->getShape().setSize(sf::Vector2f(m_frames_rects_[m_actual_frame_].getSize()));
			}
		}
	}

	Animation::Animation()
		: m_base_total_frame_(0), m_base_frame_time_(0.f),
		  m_window_his_open_(false), m_animation_is_paused_(false), m_animation_is_reversed_(false),
		  m_stop_at_last_frame_(false), m_want_to_load_anim_(false)
	{
		m_name = "Animation";
		m_typeName = "Animation";
		m_type = TYPE::ANIMATION;

		m_texture_to_search_ = std::make_pair(false, "");
	}

	Animation::Animation(const std::shared_ptr<lc::Texture>& used_texture, const std::string& name)
		: m_base_total_frame_(0), m_base_frame_time_(0.f),
		  m_window_his_open_(false), m_animation_is_paused_(false), m_animation_is_reversed_(false),
		  m_stop_at_last_frame_(false), m_want_to_load_anim_(false)
	{
		m_name = name;
		m_typeName = "Animation";
		m_type = TYPE::ANIMATION;

		m_texture_ = used_texture;
		used_texture->isVisible(false);

		m_texture_to_search_ = std::make_pair(false, "");
	}

	Animation::~Animation()
	{
		if (!m_texture_.expired())
			m_texture_.lock()->isVisible(true);
	}

	void Animation::UpdateEvent(sf::Event& event)
	{
		m_renderer_.UpdateZoom(event);
	}

	void Animation::Update(WindowManager& window)
	{
		this->texture_to_search();

		this->update_renderer_window();

		if (const auto tmp_texture = m_texture_.lock())
			if (const auto tmp_animation_key = m_actual_animation_key_.lock())
				tmp_animation_key->update_animation_key(tmp_texture, m_animation_is_paused_, m_animation_is_reversed_, m_stop_at_last_frame_);
	}

	void Animation::Draw(WindowManager& window)
	{
	}

	void Animation::Draw(sf::RenderTexture& window)
	{
		m_renderer_.Clear();

		this->draw_animation(window);

		m_renderer_.Display();
	}

	void Animation::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
	{
		save << static_cast<int>(m_type)
			 << " " << m_typeName
			 << " " << m_name
			 << " " << (m_texture_.expired() ? static_cast<std::string>("No_Texture") : m_texture_.lock()->getName())
			 << " " << (m_actual_animation_key_.expired() ? "No_Actual_Key" : m_actual_animation_key_.lock()->get_name())
			 << " " << static_cast<int>(m_animation_keys_.size()) << '\n';

		for (const auto& animation_key_pair : m_animation_keys_)
		{
			const auto tmp_animation_key = animation_key_pair.second;

			save << tmp_animation_key->get_name()
				 << " " << tmp_animation_key->get_base_int_rect()
				 << " " << tmp_animation_key->get_max_frame()
				 << " " << tmp_animation_key->get_frame_time()
				 << " " << tmp_animation_key->get_total_frame() <<'\n';
		}
	}

	void Animation::Export(std::ofstream& exportation)
	{
		exportation << static_cast<int>(m_type)
			 << " " << m_name
			 << " " << (m_texture_.expired() ? static_cast<std::string>("No_Texture") : m_texture_.lock()->getName())
			 << " " << (m_actual_animation_key_.expired() ? "No_Actual_Key" : m_actual_animation_key_.lock()->get_name())
			 << " " << static_cast<int>(m_animation_keys_.size()) << '\n';

		for (const auto& animation_key_pair : m_animation_keys_)
		{
			const auto tmp_animation_key = animation_key_pair.second;

			exportation << tmp_animation_key->get_name()
				 << " " << tmp_animation_key->get_base_int_rect()
				 << " " << tmp_animation_key->get_max_frame()
				 << " " << tmp_animation_key->get_frame_time()
				 << " " << tmp_animation_key->get_total_frame() <<'\n';
		}
	}

	void Animation::Load(std::ifstream& load)
	{
		int tmp_animation_key_cout(0);
		std::string tmp_texture_name;
		std::string tmp_actual_key_name;

		load >> m_typeName
			 >> m_name
			 >> tmp_texture_name
			 >> tmp_actual_key_name
			 >> tmp_animation_key_cout;

		std::shared_ptr<AnimationKey> tmp_animation_key;
		for (int i = 0; i < tmp_animation_key_cout; i++)
		{
			tmp_animation_key = std::make_shared<AnimationKey>();
			
			load >> tmp_animation_key->get_name()
				 >> tmp_animation_key->get_base_int_rect()
				 >> tmp_animation_key->get_max_frame()
				 >> tmp_animation_key->get_frame_time()
				 >> tmp_animation_key->get_total_frame();

			tmp_animation_key->create_frames_rect();

			if (tmp_actual_key_name != "No_Actual_Key")
				if (tmp_animation_key->get_name() == tmp_actual_key_name)
					m_actual_animation_key_ = tmp_animation_key;

			m_animation_keys_.emplace(tmp_animation_key->get_name(), tmp_animation_key);
		}
		
		m_texture_to_search_ = std::make_pair(true, tmp_texture_name);
	}

	std::shared_ptr<lc::GameComponent> Animation::Clone()
	{
		auto tmp_clone = std::make_shared<lc::Animation>(*this);
		tmp_clone->m_renderer_.get_render_texture() = std::make_shared<sf::RenderTexture>();
		tmp_clone->m_renderer_.get_render_texture()->create(
			static_cast<unsigned int>(m_renderer_.get_size().x),
			static_cast<unsigned int>(m_renderer_.get_size().y));
		if (!tmp_clone->m_texture_.expired())
			tmp_clone->m_texture_to_search_ = std::make_pair(true, tmp_clone->m_texture_.lock()->getName());
		tmp_clone->m_texture_.reset();

		tmp_clone->m_animation_keys_.clear();
		for (const auto& animation_key_pair : m_animation_keys_)
		{
			tmp_clone->m_animation_keys_.emplace(animation_key_pair.first, std::make_shared<AnimationKey>(*animation_key_pair.second));
			tmp_clone->m_actual_animation_key_ = tmp_clone->m_animation_keys_[animation_key_pair.first];
		}

		return tmp_clone;
	}

	void Animation::setHierarchieFunc()
	{
		m_hierarchieInformation = [this]()
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				ImGui::Checkbox("Pause Animation", &m_animation_is_paused_);
				ImGui::Checkbox("Reverse Animation", &m_animation_is_reversed_);

				if (ImGui::Button("Open Animation Window Test"))
					m_window_his_open_ = true;
				if(ImGui::Button("Load A .anim"))
					m_want_to_load_anim_ = true;
			
				if(m_want_to_load_anim_)
				{
					ImGui::Begin(("Load animation##" + std::to_string(getID())).c_str(), &m_want_to_load_anim_, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
					
					if(Button("Search .anim"))
					{
						Tools::IG::LoadRessourcesFromFile(m_anim_path_,"anim");
					}

					if(!m_anim_path_.empty())
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
						
						ImGui::Text(std::string("Path : " + m_anim_path_).c_str());	
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
					
					if(Button("Load anim") and !m_anim_path_.empty())
					{
						load_animation_file(m_anim_path_);
						m_want_to_load_anim_ = false;
					}
					ImGui::End();
				}
			
				if (!m_texture_.expired())
					if (ImGui::Button("Save Animation As A .anim"))
						this->save_animation_file();

				ImGui::DragFloat2("Relative Position", m_relativePosition);

				ImGui::InputText("Animation Name", m_name, 80);

				if (m_name.empty())
					m_name = "No_Name";

				Tools::ReplaceCharacter(m_name, ' ', '_');

				if (ImGui::BeginCombo("Selected Texture", m_texture_.expired() ? "No Texture Selected" : m_texture_.lock()->getName().c_str()))
				{
					const bool tmp_is_selected(false);

					if (!m_texture_.expired())
					{
						if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_is_selected))
						{
							const auto tmp_texture = m_texture_.lock();
							tmp_texture->isVisible(true);
							tmp_texture->getShape().setTextureRect(tmp_texture->getTextureRect());

							m_texture_.reset();
						}
					}

					for (auto& component : getParent()->getComponents())
					{
						if (auto tmp_texture_component = std::dynamic_pointer_cast<lc::Texture>(component))
						{
							if (tmp_texture_component->isVisible())
							{
								if (ImGui::Selectable(std::string(tmp_texture_component->getName() + "##" + std::to_string(tmp_texture_component->getID())).c_str(), tmp_is_selected))
								{
									if (!m_texture_.expired())
										m_texture_.lock()->isVisible(true);

									m_texture_ = tmp_texture_component;
									tmp_texture_component->isVisible(false);
								}
							}
						}
					}

					ImGui::EndCombo();
				}

				if (ImGui::BeginCombo("Selected Animation Key", m_actual_animation_key_.expired() ? "No Animation Key Selected" : m_actual_animation_key_.lock()->get_name().c_str()))
				{
					const bool tmp_is_selected(false);

					if (!m_actual_animation_key_.expired())
						if (ImGui::Selectable("No Animation Key", tmp_is_selected, ImGuiCol_ButtonHovered))
						{
							if (!m_texture_.expired())
							{
								auto tmp_texture = m_texture_.lock();
								tmp_texture->getShape().setSize(tmp_texture->getTextureSize());
								tmp_texture->getShape().setTextureRect(tmp_texture->getTextureRect());
							}

							m_actual_animation_key_.reset();
						}

					if (!m_texture_.expired())
						for (const auto& animation_key : m_animation_keys_)
							if (ImGui::Selectable(std::string(animation_key.second->get_name()).c_str(), tmp_is_selected))
								this->select_animation_key(animation_key.second->get_name(), true);

					ImGui::EndCombo();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				ImGui::InputText("Key Name", m_base_name_, 150);
				ImGui::DragInt4("Base Rect", m_base_int_rect_);
				ImGui::DragInt2("Max Frame", m_base_max_frame_);
				ImGui::DragInt("Base Total Frame", &m_base_total_frame_);
				ImGui::DragFloat("Base Frame Time", &m_base_frame_time_, 0.001f);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

				if (!m_texture_.expired())
				{
					if (ImGui::Button("Create Animation Key"))
					{
						auto tmp_name(m_base_name_.empty() ? "No_Name" : m_base_name_);

						Tools::ReplaceCharacter(tmp_name, ' ', '_');

						if (!m_animation_keys_.contains(tmp_name))
							m_animation_keys_.emplace(tmp_name, std::make_shared<lc::AnimationKey>(tmp_name, m_base_total_frame_, m_base_max_frame_, m_base_frame_time_, m_base_int_rect_));
						else
						{
							auto& tmp_anim = m_animation_keys_[tmp_name];
							tmp_anim->get_base_int_rect() = m_base_int_rect_;
							tmp_anim->get_frame_time() = m_base_frame_time_;
							tmp_anim->get_frame_timer() = 0.f;
							tmp_anim->get_max_frame() = m_base_max_frame_;
							tmp_anim->get_total_frame() = m_base_total_frame_;
							tmp_anim->get_actual_frame() = 0;

							tmp_anim->create_frames_rect();
						}
					}

					ImGui::BeginChild(std::string("All Key" + std::to_string(m_ID)).c_str(), sf::Vector2f(ImGui::GetContentRegionAvail().x, 200.f), ImGuiChildFlags_Border);
					{
						for (auto animation_key_pair = m_animation_keys_.begin(); animation_key_pair != m_animation_keys_.end();)
						{
							auto tmp_need_to_be_deleted(false);
							const auto tmp_animation_key = (*animation_key_pair);
							if (ImGui::TreeNodeEx(std::string(tmp_animation_key.first + "##" + std::to_string(m_ID)).c_str(), ImGuiTreeNodeFlags_Selected))
							{
								if (ImGui::ImageButton(std::string("Delete Animation Key" + std::to_string(m_ID)).c_str(), GET_MANAGER->getTexture(""), sf::Vector2f(15.f, 15.f)))
									tmp_need_to_be_deleted = true;

								ImGui::SameLine();

								ImGui::Text("Delete Animation Key");

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);

								ImGui::Text(std::string(tmp_animation_key.second->get_name() + " : Name").c_str());

								ImGui::Text(std::string(std::to_string(tmp_animation_key.second->get_total_frame()) + " : Total Frame").c_str());

								ImGui::Text(std::string(std::to_string(tmp_animation_key.second->get_max_frame().x) + " : x " +
									std::to_string(tmp_animation_key.second->get_max_frame().y) + " : y Max Frame").c_str());

								ImGui::Text(std::string(std::to_string(tmp_animation_key.second->get_frame_time()) + " : Frame Time").c_str());

								if (ImGui::TreeNodeEx("All Frame Rect", ImGuiTreeNodeFlags_Selected))
								{
									for (const auto& frame_rect : tmp_animation_key.second->get_frames_rects())
										ImGui::Text(std::string(std::to_string(frame_rect.left) + ", " + std::to_string(frame_rect.top) + ", " +
											std::to_string(frame_rect.width) + ", " + std::to_string(frame_rect.height)).c_str());

									ImGui::TreePop();
								}

								ImGui::TreePop();
							}

							if (tmp_need_to_be_deleted)
								animation_key_pair = m_animation_keys_.erase(animation_key_pair);
							else
								++animation_key_pair;
						}
					}
					ImGui::EndChild();
					
				}
			};
	}

	sf::RectangleShape& Animation::getShape()
	{
		return (m_texture_.expired() ? m_renderer : m_texture_.lock()->getShape());
	}

	void Animation::select_animation_key(
		const std::string& name, const int& start_frame, const float start_frame_timer, const bool reset_last_anim_key
	)
	{
		auto tmp_act_anim_key = m_actual_animation_key_.lock();

		if (tmp_act_anim_key && reset_last_anim_key)
		{
			tmp_act_anim_key->get_actual_frame() = 0;
			tmp_act_anim_key->get_frame_timer() = 0.f;
		}
		
		m_actual_animation_key_ = m_animation_keys_.at(name);

		tmp_act_anim_key = m_actual_animation_key_.lock();
		const auto tmp_texture = m_texture_.lock();
		tmp_act_anim_key->get_actual_frame() = start_frame;
		tmp_act_anim_key->get_frame_timer() = start_frame_timer;
		tmp_texture->getShape().setTextureRect(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()]);
		tmp_texture->getShape().setSize(sf::Vector2f(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()].getSize()));
	}

	void Animation::select_animation_key(
		const std::string& name, const bool reset_last_anim_key
	)
	{
		auto tmp_act_anim_key = m_actual_animation_key_.lock();

		if (tmp_act_anim_key && reset_last_anim_key)
		{
			tmp_act_anim_key->get_actual_frame() = 0;
			tmp_act_anim_key->get_frame_timer() = 0.f;
		}
		
		m_actual_animation_key_ = m_animation_keys_.at(name);

		tmp_act_anim_key = m_actual_animation_key_.lock();
		const auto tmp_texture = m_texture_.lock();
		tmp_texture->getShape().setTextureRect(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()]);
		tmp_texture->getShape().setSize(sf::Vector2f(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()].getSize()));
	}

	void Animation::current_animation_is_paused(const bool& paused)
	{
		m_animation_is_paused_ = paused;
	}

	void Animation::current_animation_is_reversed(const bool& reversed)
	{
		m_animation_is_reversed_ = reversed;
	}

	void Animation::set_stop_at_last_frame(const bool& stop_at_last_frame)
	{
		m_stop_at_last_frame_ = stop_at_last_frame;		
	}

	void Animation::load_animation_file(const std::string& path)
	{
		std::ifstream file(path);
		if(file.is_open() and fs::path(path).extension() == ".anim")
		{
			int tmp_key_anim_count;
			
			m_texture_to_search_.first = true;
			file >> m_name >> m_texture_to_search_.second >> tmp_key_anim_count;
			
			for (int i = 0; i < tmp_key_anim_count; i++){
				AnimationKey tmp_key_anim;
				file >> tmp_key_anim.get_name()
				 >> tmp_key_anim.get_base_int_rect()
				 >> tmp_key_anim.get_max_frame()
				 >> tmp_key_anim.get_frame_time()
				 >> tmp_key_anim.get_total_frame();

				tmp_key_anim.create_frames_rect();

				m_animation_keys_.emplace(tmp_key_anim.get_name(), std::make_shared<AnimationKey>(tmp_key_anim));
			}
		}
	}

	void Animation::save_animation_file(const bool open_file_browser, std::string path) const
	{
		std::string tmp_extention_name("anim");

		if (open_file_browser)
			Tools::IG::SaveRessourcesFromFile(path, tmp_extention_name.c_str());

		if (!path.empty())
		{
			//This is to separate the path and name of the file.
			std::string tmp_file_name(Tools::split_path_and_file_name(path));

			//This is to erase the .anim if is in the tmp_file_name.
			Tools::remove_extention(tmp_file_name, tmp_extention_name);

			if (!fs::exists(path + tmp_file_name))
				fs::create_directory(path + tmp_file_name);

			//Then we write every information in the .anim file.
			std::ofstream tmp_save_animation(path + '\\' + tmp_file_name + '\\' + m_name + "." + tmp_extention_name, std::ios::out);
			{
				tmp_save_animation << m_name << '\n';
				tmp_save_animation << (m_texture_.expired() ? "No_texture" : m_texture_.lock()->getName()) << '\n';
				tmp_save_animation << static_cast<int>(m_animation_keys_.size()) << '\n';
				for (const auto& animation_key_pair : m_animation_keys_)
				{
					tmp_save_animation << animation_key_pair.second->get_name() << '\n';
					tmp_save_animation << animation_key_pair.second->get_base_int_rect() << '\n';
					tmp_save_animation << animation_key_pair.second->get_max_frame() << '\n';
					tmp_save_animation << animation_key_pair.second->get_frame_time() << '\n';
					tmp_save_animation << animation_key_pair.second->get_total_frame() << '\n';
				}
			}
			
			tmp_save_animation.close();
		}
	}

	void Animation::add_animation_key(
		const std::string& name, const int& total_frame, const sf::Vector2i& max_frame, const float& frame_time,
		const sf::IntRect& frame_rect
	)
	{
		m_animation_keys_.emplace(name, std::make_shared<AnimationKey>(name, total_frame, max_frame, frame_time, frame_rect));
	}

	void Animation::delete_animation_key(const std::string& name)
	{
		m_animation_keys_.erase(name);
	}

	void Animation::texture_to_search()
	{
		if (m_texture_to_search_.first)
		{
			for (const auto& component : getParent()->getComponents())
			{
				if (const auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
				{
					if (m_texture_to_search_.second == tmp_texture->getName())
					{
						m_texture_ = tmp_texture;
						tmp_texture->isVisible(false);
						break;
					}
				}
			}

			m_texture_to_search_.first = false;
		}
	}

	void Animation::update_renderer_window()
	{
		if (m_window_his_open_)
		{
			ImGui::Begin(std::string("Window Animation Tester##" + std::to_string(m_ID) + std::to_string(getParent()->getID()) + "lc").c_str(), &m_window_his_open_);

			m_renderer_.Update();

			ImGui::End();
		}
	}

	void Animation::draw_animation(sf::RenderTexture& window)
	{
		if (const auto tmp_texture = m_texture_.lock())
		{
			tmp_texture->getShape().setOrigin(getParent()->getTransform().getOrigin());
			tmp_texture->getShape().setScale(getParent()->getTransform().getScale());
			tmp_texture->getShape().setRotation(getParent()->getTransform().getRotation());

			m_renderer.setOrigin(getParent()->getTransform().getOrigin());
			m_renderer.setScale(getParent()->getTransform().getScale());
			m_renderer.setRotation(getParent()->getTransform().getRotation());

			m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);

			tmp_texture->getShape().setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
			window.draw(tmp_texture->getShape());

			tmp_texture->getShape().setPosition(m_renderer_.get_size() / 2.f);
			m_renderer_.Draw(tmp_texture->getShape());
		}
	}
}
