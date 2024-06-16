#pragma once
#include "Shader.h"

namespace lc
{
    namespace Shader
    {
        enum light_type { OFF, NORMAL, FLICKERING, FLAME };
        
        class LightShader : public Shader
        {
        public:
            LightShader();
            virtual ~LightShader() override;

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
        private:
            virtual void setup_shader_script_string() override;

            bool is_the_last_light_of_the_depth();
        private:
            static sf::Vector3f s_ambient_light_;

            static std::pair<unsigned int, std::list<std::weak_ptr<LightShader>>> s_lights_;

            static sf::Texture s_diffuse_texture_;
            static sf::Texture s_last_pass_texture_;
            
            static sf::Sprite s_last_pass_sprite_;

            static unsigned int s_pass_number_;
            static unsigned int s_max_light_per_pass_;
            
            static sf::Vector2i s_window_size_;
            
            sf::Vector3f m_color_;
            light_type m_type_;

            float m_flicker_timer_;
            float m_radius_;
            float m_fall_off_;
            float m_rand_radius_;
            float m_actual_radius_;

            bool m_is_growing_;
            bool m_is_on_;

            float m_lerp_speed_;
            float m_max_radius_rand_;
            float m_min_radius_rand_;
            float m_radius_gap_;

            float m_max_flicker_timer_;
            float m_min_flicker_timer_;

            bool m_has_been_add_to_lights_;
        };

    }
}