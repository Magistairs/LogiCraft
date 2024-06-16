#pragma once
#include "Ressource.h"

namespace lc
{
	namespace Shader
	{
		class Shader : public lc::Ressource
		{
		public:
			Shader();
			virtual ~Shader() override;

			virtual void UpdateEvent(sf::Event& event) = 0;
			virtual void Update(WindowManager& window) = 0;
			virtual void Draw(WindowManager& window) = 0;
			virtual void Draw(sf::RenderTexture& window) = 0;

			virtual void Save(std::ofstream& save, sf::RenderTexture& texture, int depth) = 0;
			virtual void SaveRenderer(sf::RenderTexture& texture, int depth) override {}
			virtual void Export(std::ofstream& exportation) override {}
			virtual void Load(std::ifstream& load) = 0;

			virtual std::shared_ptr<lc::GameComponent> Clone() = 0;
			virtual void setHierarchieFunc() = 0;
		protected:
			virtual void setup_shader_script_string() = 0;

			bool is_totally_in(const std::shared_ptr<lc::GameObject>& game_object);
		protected:
			std::shared_ptr<sf::Shader> m_shader_;
			sf::RenderStates m_shader_states_;

			sf::Texture m_texture_map_;
			
			float m_time_;

			std::string m_shader_script_;
		};		
	}
}

