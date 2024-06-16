
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

namespace lc
{
	class AnimationKey
	{
	public:
		AnimationKey();
		AnimationKey(const std::string& name, const int& total_frame, const sf::Vector2i& max_frame, const float& frame_time, const sf::IntRect& frame_rect);
		~AnimationKey();

		std::string& get_name();

		std::vector<sf::IntRect>& get_frames_rects();

		sf::IntRect& get_base_int_rect();

		sf::Vector2i& get_max_frame();
		int& get_total_frame();
		int& get_actual_frame();

		float& get_frame_timer();
		float& get_frame_time();

		void create_frames_rect();

		void update_animation_key(const std::shared_ptr<lc::Texture>& texture, 
			const bool& animation_is_paused, const bool& animation_is_reversed,
			const bool& stop_at_last_frame);
	private:
		std::string m_name_;

		std::vector<sf::IntRect> m_frames_rects_;

		sf::IntRect m_base_frame_rect_;

		sf::Vector2i m_max_frame_;

		int m_total_frame_;
		int m_actual_frame_;

		float m_frame_timer_;
		float m_frame_time_;
	};

	class Animation : public lc::Ressource
	{
	public:
		Animation();
		Animation(const std::shared_ptr<lc::Texture>& used_texture, const std::string& name);
		virtual ~Animation() override;

		virtual void UpdateEvent(sf::Event& event) override;
		virtual void Update(WindowManager& window) override;
		virtual void Draw(WindowManager& window) override;
		virtual void Draw(sf::RenderTexture& window) override;

		virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int depth) override;
		virtual void SaveRenderer(sf::RenderTexture& texture, int depth) override {}
		virtual void Export(std::ofstream& exportation) override;
		virtual void Load(std::ifstream& load) override;
		
		virtual std::shared_ptr<lc::GameComponent> Clone() override;
		virtual void setHierarchieFunc() override;

		virtual sf::RectangleShape& getShape() override;

		void select_animation_key(const std::string& name, const int& start_frame = 0u, const float start_frame_timer = 0.f, const bool reset_last_anim_key = false);
		
		void select_animation_key(const std::string& name, const bool reset_last_anim_key = false);

		void current_animation_is_paused(const bool& paused = true);

		void current_animation_is_reversed(const bool& reversed = true);

		void load_animation_file(const std::string& path = "");

		void save_animation_file(const bool open_file_browser = true, std::string path = "") const;

		void set_stop_at_last_frame(const bool& stop_at_last_frame);
		
		void add_animation_key(const std::string& name, const int& total_frame, const sf::Vector2i& max_frame, const float& frame_time, const sf::IntRect& frame_rect);
		
		void delete_animation_key(const std::string& name);

		const auto& get_all_key_animation() { return m_animation_keys_; }
		const auto& get_actual_animation_key() { return m_actual_animation_key_; }
		auto& get_texture() { return m_texture_; }
	private:
		void texture_to_search();

		void update_renderer_window();

		void draw_animation(sf::RenderTexture& window);
	private:
		int m_base_total_frame_;
		float m_base_frame_time_;
		std::string m_base_name_;
		sf::IntRect m_base_int_rect_;
		sf::Vector2i m_base_max_frame_;

		bool m_window_his_open_;
		bool m_animation_is_paused_;
		bool m_animation_is_reversed_;
		bool m_stop_at_last_frame_;
		bool m_want_to_load_anim_;
		
		Tools::Renderer m_renderer_;

		std::weak_ptr<lc::Texture> m_texture_;

		std::weak_ptr<AnimationKey> m_actual_animation_key_;
		std::unordered_map<std::string, std::shared_ptr<AnimationKey>> m_animation_keys_;

		RessourceToSearch m_texture_to_search_;

		std::string m_anim_path_;
	};
}