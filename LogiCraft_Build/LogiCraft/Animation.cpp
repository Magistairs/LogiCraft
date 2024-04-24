#include "Animation.h"

#include <ranges>

namespace lc
{
	AnimationKey::AnimationKey()
		: m_total_frame_(0), m_actual_frame_(0), m_frame_timer_(0.f), m_frame_time_(0.f)
	{
	}

	AnimationKey::AnimationKey(std::string name, int total_frame, sf::Vector2i max_frame, float frame_time, sf::IntRect frame_rect)
		: m_name_(name), m_base_frame_rect_(frame_rect), m_max_frame_(max_frame), m_total_frame_(total_frame), m_actual_frame_(0), m_frame_timer_(0.f),
		  m_frame_time_(frame_time)
	{
		this->create_frames_rect();
	}

	AnimationKey::~AnimationKey()
	{
	}

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

	Animation::Animation()
		: m_animation_name_("No Name"), m_base_total_frame_(0), m_base_frame_time_(0.f), m_window_his_open_(false), m_animation_is_paused_(false)
	{
		m_name = "Animation";
		m_typeName = "Animation";
		m_type = TYPE::ANIMATION;
	}

	Animation::~Animation()
	{
	}

	void Animation::UpdateEvent(sf::Event& event)
	{
		m_renderer_.UpdateZoom(event);
	}

	void Animation::Update(WindowManager& window)
	{
		this->texture_to_search();

		this->update_renderer_window();
	}

	void Animation::Draw(WindowManager& window)
	{
	}

	void Animation::Draw(sf::RenderTexture& window)
	{
		m_renderer_.Clear();

		if (const auto tmp_texture = m_texture_.lock())
		{
			if (const auto tmp_animation_key = m_actual_animation_key_.lock())
			{
				if (!m_animation_is_paused_)
				{
					tmp_animation_key->get_frame_timer() += Tools::getDeltaTime();
					if (tmp_animation_key->get_frame_timer() >= tmp_animation_key->get_frame_time())
					{
						m_animation_is_reversed_ ? tmp_animation_key->get_actual_frame()-- : tmp_animation_key->get_actual_frame()++;

						if (tmp_animation_key->get_actual_frame() <= 0 && m_animation_is_reversed_)
							tmp_animation_key->get_actual_frame() = tmp_animation_key->get_total_frame() - 1;
						else if (tmp_animation_key->get_actual_frame() > tmp_animation_key->get_total_frame() - 1 && !m_animation_is_reversed_)
							tmp_animation_key->get_actual_frame() = 0;

						tmp_animation_key->get_frame_timer() = 0.f;
					}

					tmp_texture->getShape().setTextureRect(tmp_animation_key->get_frames_rects()[tmp_animation_key->get_actual_frame()]);
					tmp_texture->getShape().setSize(sf::Vector2f(tmp_animation_key->get_frames_rects()[tmp_animation_key->get_actual_frame()].getSize()));
				}
			}

			if (!m_isUsedByAComponent)
			{
				tmp_texture->getShape().setOrigin(getParent()->getTransform().getOrigin());
				tmp_texture->getShape().setScale(getParent()->getTransform().getScale());
				tmp_texture->getShape().setRotation(getParent()->getTransform().getRotation());

				tmp_texture->getShape().setPosition(getParent()->getTransform().getPosition());
				window.draw(tmp_texture->getShape());
			}

			tmp_texture->getShape().setPosition(m_renderer_.get_size() / 2.f);
			m_renderer_.Draw(tmp_texture->getShape());
		}

		m_renderer_.Display();
	}

	void Animation::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
	{
		save << static_cast<int>(m_type)
			 << " " << m_typeName
			 << " " << m_animation_name_
			 << " " << (m_texture_.expired() ? static_cast<std::string>("No_Texture") : m_texture_.lock()->getName())
			 << " " << static_cast<int>(m_animation_keys_.size()) << '\n';

		for (const auto& animation_key_pair : m_animation_keys_)
		{
			const auto tmp_animation_key = animation_key_pair.second;

			save << tmp_animation_key->get_name() << '\n';
			save << tmp_animation_key->get_base_int_rect() << '\n';
			save << tmp_animation_key->get_max_frame() << '\n';
			save << tmp_animation_key->get_frame_time() << '\n';
			save << tmp_animation_key->get_total_frame() << '\n';
		}
	}

	void Animation::Load(std::ifstream& load)
	{
		int tmp_animation_key_cout(0);
		std::string tmp_texture_name;

		load >> m_typeName
			 >> m_animation_name_
			 >> tmp_texture_name
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

			m_animation_keys_.emplace(tmp_animation_key->get_name(), tmp_animation_key);
		}
		
		m_texture_to_search_ = std::make_pair(true, tmp_texture_name);
	}

	std::shared_ptr<lc::GameComponent> Animation::Clone()
	{
		return std::make_shared<lc::Animation>(*this);
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

				ImGui::InputText("Animation Name", m_animation_name_, 80);

				Tools::ReplaceCharacter(m_animation_name_, ' ', '_');

				if (ImGui::BeginCombo("Selected Texture", m_texture_.expired() ? "No Texture Selected" : m_texture_.lock()->getName().c_str()))
				{
					const bool tmp_is_selected(false);

					if (!m_texture_.expired())
					{
						if (ImGui::Selectable(std::string("No Texture ##" + std::to_string(m_ID)).c_str(), tmp_is_selected))
						{
							if (!m_texture_.expired())
							{
								const auto tmp_texture = m_texture_.lock();
								tmp_texture->isUsedByAComponent() = false;
								tmp_texture->getShape().setTextureRect(tmp_texture->getTextureRect());
							}

							m_texture_.reset();
						}
					}

					for (auto& component : getParent()->getComponents())
					{
						if (auto tmp_texture_component = std::dynamic_pointer_cast<lc::Texture>(component))
						{
							if (!tmp_texture_component->isUsedByAComponent())
							{
								if (ImGui::Selectable(std::string(tmp_texture_component->getName() + "##" + std::to_string(tmp_texture_component->getID())).c_str(), tmp_is_selected))
								{
									if (!m_texture_.expired())
										m_texture_.lock()->isUsedByAComponent() = false;

									m_texture_ = tmp_texture_component;
									tmp_texture_component->isUsedByAComponent() = true;
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
						if (ImGui::Selectable("No Animation Key", tmp_is_selected))
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

					ImGui::BeginChild(std::string("All Key" + std::to_string(m_ID)).c_str(), sf::Vector2f(ImGui::GetContentRegionAvail().x, 100.f), ImGuiChildFlags_Border);
					{
						for (auto animation_key = m_animation_keys_.begin(); animation_key != m_animation_keys_.end();)
						{
							auto tmp_need_to_be_deleted(false);
							const auto tmp_animation_key = (*animation_key);
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
								animation_key = m_animation_keys_.erase(animation_key);
							else
								++animation_key;
						}
					}
					ImGui::EndChild();
				}
			};
	}

	void Animation::select_animation_key(const std::string& name, const bool reset_last_anim_key)
	{
		const auto tmp_act_anim_key = m_actual_animation_key_.lock();
		if (tmp_act_anim_key ? tmp_act_anim_key->get_name() != name : true)
		{
			if (tmp_act_anim_key && reset_last_anim_key)
			{
				tmp_act_anim_key->get_actual_frame() = 0;
				tmp_act_anim_key->get_frame_timer() = 0.f;
			}

			m_actual_animation_key_ = m_animation_keys_.at(name);

			const auto tmp_act_anim_key = m_actual_animation_key_.lock();
			const auto tmp_texture = m_texture_.lock();
			tmp_texture->getShape().setTextureRect(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()]);
			tmp_texture->getShape().setSize(sf::Vector2f(tmp_act_anim_key->get_frames_rects()[tmp_act_anim_key->get_actual_frame()].getSize()));
		}
	}

	void Animation::current_animation_is_paused(const bool paused)
	{
		m_animation_is_paused_ = paused;
	}

	void Animation::current_animation_is_reversed(const bool reversed)
	{
		m_animation_is_reversed_ = reversed;
	}

	void Animation::texture_to_search()
	{
		if (m_texture_to_search_.first)
		{
			for (auto& component : getParent()->getComponents())
			{
				if (auto tmp_texture = std::dynamic_pointer_cast<lc::Texture>(component))
				{
					if (m_texture_to_search_.second == tmp_texture->getName())
					{
						m_texture_ = tmp_texture;
						tmp_texture->isUsedByAComponent() = true;
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
}
