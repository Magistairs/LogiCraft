#include "LightShader.h"

sf::Vector3f lc::Shader::LightShader::s_ambient_light_ = sf::Vector3f(0.f, 0.f, 0.f);
sf::Vector2i lc::Shader::LightShader::s_window_size_ = sf::Vector2i(0, 0);

unsigned int lc::Shader::LightShader::s_pass_number_ = 0u;
unsigned int lc::Shader::LightShader::s_max_light_per_pass_ = 10u;

std::pair<unsigned int, std::list<std::weak_ptr<lc::Shader::LightShader>>> lc::Shader::LightShader::s_lights_ = std::pair<unsigned int, std::list<std::weak_ptr<lc::Shader::LightShader>>>();

sf::Texture lc::Shader::LightShader::s_diffuse_texture_ = sf::Texture();
sf::Texture lc::Shader::LightShader::s_last_pass_texture_ = sf::Texture();

sf::Sprite lc::Shader::LightShader::s_last_pass_sprite_ = sf::Sprite();

lc::Shader::LightShader::LightShader()
    : m_color_(1.f, 1.f, 1.f), m_type_(light_type::NORMAL), m_flicker_timer_(0.f), m_radius_(100.f), m_fall_off_(0.5f),
      m_rand_radius_(0.f),
      m_actual_radius_(m_radius_),
      m_is_growing_(false), m_is_on_(true),
	  m_lerp_speed_(0.001f), m_max_radius_rand_(20.f), m_min_radius_rand_(10.f), m_radius_gap_(7.f), m_max_flicker_timer_(0.4f), m_min_flicker_timer_(0.1f),
	  m_has_been_add_to_lights_(false)
{
	m_name = "Light_Shader";
	m_typeName = "Light_Shader";
	m_type = TYPE::SHADER;
	
	LightShader::setup_shader_script_string();

	m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
}

lc::Shader::LightShader::~LightShader()
{
	m_shader_.reset();
	s_lights_.second.remove_if([this](const std::weak_ptr<lc::Shader::LightShader>& wptr_light)
	{
		return wptr_light.expired();
	});
}

void lc::Shader::LightShader::UpdateEvent(sf::Event& event)
{
}

void lc::Shader::LightShader::Update(WindowManager& window)
{
    if (!m_has_been_add_to_lights_)
    {
	    if (getParent()->getDepth() == 4)
	    	LightShader::s_lights_.second.emplace_back(std::dynamic_pointer_cast<lc::Shader::LightShader>(shared_from_this()));
	    else
		    m_needToBeDeleted = true;

    	m_has_been_add_to_lights_ = true;  
    }

	m_renderer.setSize({m_radius_, m_radius_});
	m_renderer.setPosition(getParent()->getTransform().getPosition() + m_relativePosition);
	m_renderer.setOrigin(getParent()->getTransform().getOrigin());
}

void lc::Shader::LightShader::Draw(WindowManager& window)
{
	auto& tmp_lights_index = LightShader::s_lights_.first;
	const auto& tmp_light_list = LightShader::s_lights_.second;
	
	tmp_lights_index++;
	
	if (tmp_lights_index >= tmp_light_list.size())
    {
		//If the index is good and the light we are in is the last one.
	    if (is_the_last_light_of_the_depth())
	    {
		    unsigned int tmp_light_index(0u);

	    	//Then, if s_window_size_ is different from the actual one, it gets resized.
	    	//So when the update function of a sf::texture is called it won't crash.
	        if (LightShader::s_window_size_ != window.getSize())
	        {
	            s_window_size_ = sf::Vector2i(window.getSize()); 
	            s_diffuse_texture_.create(s_window_size_.x, s_window_size_.y);
	            s_last_pass_texture_.create(s_window_size_.x, s_window_size_.y);
	        }

	        s_diffuse_texture_.update(window.getWindow());

	        for (auto light = tmp_light_list.begin(); light != tmp_light_list.end(); ++light)
	        {
	        	//I do this to avoid doing this (*light).
	            const auto& tmp_light = light->lock();
        		const auto& tmp_window_view = window.getWindow().getView();

	            const float tmp_zoom(tmp_window_view.getSize().x / sf::Vector2f(window.getSize()).x);
	            //const float radius_distance(((tmp_light->m_radius_ / tmp_zoom) / tmp_light->m_fall_off_) + 25.f);

        		if (tmp_light->m_type_ != light_type::OFF && tmp_light->getParent()->is_in_window_view(window) || light == tmp_light_list.begin())
	            {
        			//Set the light position with the view
        			const sf::Vector2i tmp_light_position_view(window.getWindow().mapCoordsToPixel(tmp_light->getParent()->getTransform().getPosition(), tmp_window_view));
        			m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_position_",
						sf::Vector3f(static_cast<float>(tmp_light_position_view.x), static_cast<float>(tmp_light_position_view.y), 10.f));

        			//If the light is flickering, we need to change the on/off state
        			if (tmp_light->m_type_ == light_type::FLICKERING)
        			{
        				tmp_light->m_flicker_timer_ -= Tools::getDeltaTime();

        				if (tmp_light->m_flicker_timer_ < 0.f)
        				{
        					tmp_light->m_is_on_ = !tmp_light->m_is_on_;
        					tmp_light->m_flicker_timer_ = Tools::Rand(tmp_light->m_min_flicker_timer_, tmp_light->m_max_flicker_timer_);
        				}
        			}

        			//If the light is a flame, we need to change the radius to growth and then shrink.
        			if (tmp_light->m_type_ == light_type::FLAME)
        			{
        				tmp_light->m_actual_radius_ = Tools::Lerp(tmp_light->m_actual_radius_, tmp_light->m_radius_ + tmp_light->m_rand_radius_, tmp_light->m_lerp_speed_);

        				if (tmp_light->m_is_growing_ && (tmp_light->m_radius_ + tmp_light->m_rand_radius_) - tmp_light->m_actual_radius_ < tmp_light->m_radius_gap_)
        				{
        					tmp_light->m_rand_radius_ = 0.f;
        					tmp_light->m_is_growing_ = false;
        				}
        				else if (!tmp_light->m_is_growing_ && std::abs((tmp_light->m_radius_ + tmp_light->m_rand_radius_) - tmp_light->m_actual_radius_) < tmp_light->m_radius_gap_)
        				{
        					tmp_light->m_rand_radius_ = Tools::Rand(tmp_light->m_min_radius_rand_, tmp_light->m_max_radius_rand_);
        					tmp_light->m_is_growing_ = true;
        				}

        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_radius_", tmp_light->m_actual_radius_ / tmp_zoom);
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_falloff_", tmp_light->m_fall_off_);
        			}
        			else
        			{
        				tmp_light->m_actual_radius_ = Tools::Lerp(tmp_light->m_actual_radius_, tmp_light->m_radius_, tmp_light->m_lerp_speed_);

        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_radius_", tmp_light->m_actual_radius_ / tmp_zoom);
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_falloff_", tmp_light->m_fall_off_);
        			}

        			if (tmp_light->m_is_on_)
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_color_", tmp_light->m_color_);
        			else
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_color_", sf::Vector3f());

        			if (tmp_light->m_radius_ <= 0.0f && light != tmp_light_list.begin())
        				tmp_light->m_radius_ = 1.0f;

        			if (tmp_light->m_type_ != light_type::OFF && tmp_light->m_type_ != light_type::FLICKERING)
        				tmp_light->m_is_on_ = true;

        			tmp_light_index++;
	            }

	        	//Two choose one the amount of light that as been done is equal to s_max_light_per_pass_ - 1,
	        	//or light is the last of the list.
		        if (tmp_light_index > s_max_light_per_pass_ - 1u || std::next(light) == tmp_light_list.end())
		        {
		        	m_shader_->setUniform("u_light_count", static_cast<int>(tmp_light_index));
		        	tmp_light_index = 0u;
		        	
		        	s_pass_number_++;
		        	m_shader_->setUniform("u_pass_number", static_cast<int>(s_pass_number_));

		        	s_last_pass_texture_.update(window.getWindow());

		        	m_shader_->setUniform("u_last_pass", s_last_pass_texture_);
		        	m_shader_->setUniform("u_diffuse_map", s_diffuse_texture_);
		        	m_shader_->setUniform("u_resolution",
						sf::Vector2f(static_cast<float>(s_window_size_.x), static_cast<float>(s_window_size_.y)));
		        	
		        	m_shader_->setUniform("u_ambient_light", s_ambient_light_);

		        	//Then we need to display the light on screen with the sprite.
		        	//This sprite is going to cover all the user windows.
		        	s_last_pass_sprite_.setTexture(s_last_pass_texture_);
		        	s_last_pass_sprite_.setTextureRect({ 0, 0,
						static_cast<int>(s_last_pass_texture_.getSize().x), static_cast<int>(s_last_pass_texture_.getSize().y) });
		        	s_last_pass_sprite_.setScale({ tmp_zoom, tmp_zoom });
		        	s_last_pass_sprite_.setPosition({ tmp_window_view.getCenter().x - (static_cast<float>(s_window_size_.x) * tmp_zoom / 2.0f),
						tmp_window_view.getCenter().y - (static_cast<float>(s_window_size_.y) * tmp_zoom / 2.0f) });

		        	window.draw(s_last_pass_sprite_, m_shader_states_);
		        }
	        }

			s_pass_number_ = 0u;
    		tmp_lights_index = 0u;   
	    }
    }
}

void lc::Shader::LightShader::Draw(sf::RenderTexture& window)
{
	auto& tmp_lights_index = LightShader::s_lights_.first;
	const auto& tmp_depth_light_list = LightShader::s_lights_.second;
	
	tmp_lights_index++;

	if (tmp_lights_index >= tmp_depth_light_list.size())
    {
	    if (is_the_last_light_of_the_depth())
	    {
		    unsigned int tmp_light_index(0u);
		    
	        if (sf::Vector2u(LightShader::s_window_size_) != window.getSize())
	        {
	            s_window_size_ = sf::Vector2i(window.getSize());
	            s_diffuse_texture_.create(s_window_size_.x, s_window_size_.y);
	            s_last_pass_texture_.create(s_window_size_.x, s_window_size_.y);
	        }

	        s_diffuse_texture_.update(window.getTexture());

	        for (auto light = tmp_depth_light_list.begin(); light != tmp_depth_light_list.end(); ++light)
	        {
	            const auto& tmp_light = light->lock();
        		const auto& tmp_window_view = window.getView();

	            const float tmp_zoom(tmp_window_view.getSize().x / sf::Vector2f(window.getSize()).x);
	            //const float radius_distance(((tmp_light->m_radius_ / tmp_zoom) / tmp_light->m_fall_off_) + 25.f);

        		if (tmp_light->m_type_ != light_type::OFF && tmp_light->getParent()->is_in_window_view(window) || light == tmp_depth_light_list.begin())
	            {
        			//Set the light position with the view
        			const sf::Vector2i tmp_light_position_view(window.mapCoordsToPixel(tmp_light->getParent()->getTransform().getPosition() + m_relativePosition, tmp_window_view));
        			m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_position_",
						sf::Vector3f(static_cast<float>(tmp_light_position_view.x), static_cast<float>(tmp_light_position_view.y), 10.f));

        			//If the light is flickering we need to change the on/off state
        			if (tmp_light->m_type_ == light_type::FLICKERING)
        			{
        				tmp_light->m_flicker_timer_ -= Tools::getDeltaTime();

        				if (tmp_light->m_flicker_timer_ < 0.f)
        				{
        					tmp_light->m_is_on_ = !tmp_light->m_is_on_;
        					tmp_light->m_flicker_timer_ = Tools::Rand(tmp_light->m_min_flicker_timer_, tmp_light->m_max_flicker_timer_);
        				}
        			}

        			//If the light is a flame we need to change the radius and the falloff
        			if (tmp_light->m_type_ == light_type::FLAME)
        			{
        				tmp_light->m_actual_radius_ = Tools::Lerp(tmp_light->m_actual_radius_, tmp_light->m_radius_ + tmp_light->m_rand_radius_, tmp_light->m_lerp_speed_);

        				if (tmp_light->m_is_growing_ && (tmp_light->m_radius_ + tmp_light->m_rand_radius_) - tmp_light->m_actual_radius_ < tmp_light->m_radius_gap_)
        				{
        					tmp_light->m_rand_radius_ = 0.f;
        					tmp_light->m_is_growing_ = false;
        				}
        				else if (!tmp_light->m_is_growing_ && std::abs((tmp_light->m_radius_ + tmp_light->m_rand_radius_) - tmp_light->m_actual_radius_) < tmp_light->m_radius_gap_)
        				{
        					tmp_light->m_rand_radius_ = Tools::Rand(tmp_light->m_min_radius_rand_, tmp_light->m_max_radius_rand_);
        					tmp_light->m_is_growing_ = true;
        				}

        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_radius_", tmp_light->m_actual_radius_ / tmp_zoom);
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_falloff_", tmp_light->m_fall_off_);
        			}
        			else
        			{
        				tmp_light->m_actual_radius_ = Tools::Lerp(tmp_light->m_actual_radius_, tmp_light->m_radius_, tmp_light->m_lerp_speed_);

        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_radius_", tmp_light->m_actual_radius_ / tmp_zoom);
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_falloff_", tmp_light->m_fall_off_);
        			}

        			if (tmp_light->m_is_on_)
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_color_", tmp_light->m_color_);
        			else
        				m_shader_->setUniform("lights_info[" + std::to_string(tmp_light_index) + "].m_color_", sf::Vector3f());

        			if (tmp_light->m_radius_ <= 0.0f && light != tmp_depth_light_list.begin())
        				tmp_light->m_radius_ = 1.0f;

        			if (tmp_light->m_type_ != light_type::OFF && tmp_light->m_type_ != light_type::FLICKERING)
        				tmp_light->m_is_on_ = true;

        			tmp_light_index++;
	            }

		        if (tmp_light_index > s_max_light_per_pass_ - 1u || std::next(light) == tmp_depth_light_list.end())
		        {
		        	m_shader_->setUniform("u_light_count", static_cast<int>(tmp_light_index));
		        	tmp_light_index = 0u;
        				
		        	s_pass_number_++;
		        	m_shader_->setUniform("u_pass_number", static_cast<int>(s_pass_number_));

		        	s_last_pass_texture_.update(window.getTexture());

		        	m_shader_->setUniform("u_last_pass", s_last_pass_texture_);
		        	m_shader_->setUniform("u_diffuse_map", s_diffuse_texture_);
		        	m_shader_->setUniform("u_resolution",
						sf::Vector2f(static_cast<float>(s_window_size_.x), static_cast<float>(s_window_size_.y)));
		        	
		        	m_shader_->setUniform("u_ambient_light", s_ambient_light_);

		        	s_last_pass_sprite_.setTexture(s_last_pass_texture_);
		        	s_last_pass_sprite_.setTextureRect({ 0, 0,
						static_cast<int>(s_last_pass_texture_.getSize().x), static_cast<int>(s_last_pass_texture_.getSize().y) });
		        	s_last_pass_sprite_.setScale({ tmp_zoom, tmp_zoom });
		        	s_last_pass_sprite_.setPosition({ tmp_window_view.getCenter().x - (static_cast<float>(s_window_size_.x) * tmp_zoom / 2.0f),
						tmp_window_view.getCenter().y - (static_cast<float>(s_window_size_.y) * tmp_zoom / 2.0f) });

		        	window.draw(s_last_pass_sprite_, m_shader_states_);
		        }
	        }

			s_pass_number_ = 0u;
    		tmp_lights_index = 0u;   
	    }
    }
}

void lc::Shader::LightShader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
{
	save << static_cast<int>(m_type)<< 
	 " " << m_typeName <<
	 " " << m_color_.x <<
	 " " << m_color_.y <<
	 " " << m_color_.z <<
	 " " << m_fall_off_ <<
	 " " << m_radius_ <<
	 " " << m_lerp_speed_ <<
	 " " << m_radius_gap_ <<
	 " " << m_max_radius_rand_ <<
	 " " << m_min_radius_rand_ <<
	 " " << m_max_flicker_timer_ <<
	 " " << m_min_flicker_timer_ <<
	 " " << m_relativePosition <<
	 " " << m_type_;
}

void lc::Shader::LightShader::Export(std::ofstream& exportation)
{
	exportation << static_cast<int>(m_type)<< 
	 " " << m_typeName <<
	 " " << m_color_.x <<
	 " " << m_color_.y <<
	 " " << m_color_.z <<
	 " " << m_fall_off_ <<
	 " " << m_radius_ <<
	 " " << m_lerp_speed_ <<
	 " " << m_radius_gap_ <<
	 " " << m_max_radius_rand_ <<
	 " " << m_min_radius_rand_ <<
	 " " << m_max_flicker_timer_ <<
	 " " << m_min_flicker_timer_ <<
	 " " << m_relativePosition <<
	 " " << m_type_;
}

void lc::Shader::LightShader::Load(std::ifstream& load)
{
	int type(0);
	load >>
	  m_color_.x >>
	  m_color_.y >>
	  m_color_.z >>
	  m_fall_off_ >>
	  m_radius_ >>
	  m_lerp_speed_ >>
	  m_radius_gap_ >>
	  m_max_radius_rand_ >>
	  m_min_radius_rand_ >>
	  m_max_flicker_timer_ >>
	  m_min_flicker_timer_ >>
	  m_relativePosition >>
	  type;
	m_type_ = static_cast<light_type>(type);
}

std::shared_ptr<lc::GameComponent> lc::Shader::LightShader::Clone()
{
    auto tmp_component = std::make_shared<LightShader>(*this);
    tmp_component->m_shader_ = std::make_shared<sf::Shader>();
    tmp_component->m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
    tmp_component->m_shader_states_.shader = tmp_component->m_shader_.get();
	tmp_component->m_has_been_add_to_lights_ = false;
    return tmp_component;
}

void lc::Shader::LightShader::setHierarchieFunc()
{
	m_hierarchieInformation = [this]()
	{
		ImGui::SliderFloat("General Light Alpha", &LightShader::s_ambient_light_.x, 0.f, 1.f);
		LightShader::s_ambient_light_ =
			{ LightShader::s_ambient_light_.x, LightShader::s_ambient_light_.x, LightShader::s_ambient_light_.x };

		ImGui::DragFloat2("Relative Position",m_relativePosition);
			
		ImGui::ColorEdit3("Color", m_color_, ImGuiColorEditFlags_InputRGB);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);

		ImGui::SliderFloat("Fall Off", &m_fall_off_, 0.01f, 0.99f);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);

		ImGui::DragFloat("Radius", &m_radius_);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);

		// ImGui::DragFloat("Lerp Speed", &m_lerp_speed_);
		// ImGui::DragFloat("Radius Gap", &m_radius_gap_);

		//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
		
		ImGui::DragFloat("Max Radius Rand", &m_max_radius_rand_);
		ImGui::DragFloat("Min Radius Rand", &m_min_radius_rand_);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
		
		ImGui::DragFloat("Max Flicker Rand", &m_max_flicker_timer_);
		ImGui::DragFloat("Min Flicker Rand", &m_min_flicker_timer_);

		std::string light_type[] {"OFF","NORMAL","FLICKERING","FLAME"};
		if(ImGui::BeginCombo("Select light type",light_type[static_cast<int>(m_type_)].c_str()))
		{
			for(int i = 0;i < 4; i++)
			{
				if(Selectable(light_type[static_cast<int>(i)].c_str()))
					m_type_ = static_cast<enum light_type>(i);
			}
			ImGui::EndCombo();
		}
		
		
	};
}

void lc::Shader::LightShader::setup_shader_script_string()
{
    m_shader_script_ = R"(uniform sampler2D u_last_pass;
uniform sampler2D u_diffuse_map;

uniform vec3 u_ambient_light;
uniform vec2 u_resolution;

uniform int u_light_count;
uniform int u_pass_number;

struct light_info
{
	vec3 m_position_;
	vec3 m_color_;
	float m_radius_;
	float m_falloff_;
};

const int max_light_per_pass = 10;
uniform light_info lights_info[max_light_per_pass];

void main()
{
	vec4 pixel = texture2D(u_last_pass, gl_TexCoord[0].xy);
	
	vec4 diffusepixel = texture2D(u_diffuse_map, gl_TexCoord[0].xy);
	
	vec4 finalPixel = (gl_Color * pixel);
	
	if(u_pass_number == 1)
		finalPixel *= vec4(u_ambient_light, 1.0); //IF FIRST PASS ONLY!
	
	for(int i = 0; i < u_light_count; ++i)
	{
		vec3 tmp_light_position = lights_info[i].m_position_;

		tmp_light_position.y = u_resolution.y - tmp_light_position.y;
		
		float d = max(length(tmp_light_position - gl_FragCoord.xyz) - lights_info[i].m_radius_, 0.0);
		
		//Calculate basic attenuation.
		float attenuation = 1 / pow(d / lights_info[i].m_radius_ + 1, 2);
		attenuation = (attenuation - lights_info[i].m_falloff_) / ( 1 - lights_info[i].m_falloff_);
		attenuation = max(attenuation, 0.0);
		
		finalPixel += diffusepixel * ((vec4(lights_info[i].m_color_, 1.0) * attenuation));
	}	
	gl_FragColor = finalPixel;
})";
}

bool lc::Shader::LightShader::is_the_last_light_of_the_depth()
{
	bool tmp_is_the_last_one(false);

	const std::function<void(const std::shared_ptr<lc::GameObject>&)> tmp_check_if_is_the_last_function = [&]
	(const std::shared_ptr<lc::GameObject>& tmp_object) -> void
	{
		if (tmp_object->getDepth() == 4)
		{
			if (tmp_object == getParent())
				tmp_is_the_last_one = true;
			else
				tmp_is_the_last_one = !tmp_object->hasComponent("Light_Shader");
		}
		
		for (const auto& tmp_child : tmp_object->getObjects())
			tmp_check_if_is_the_last_function(tmp_child);
	};

	tmp_check_if_is_the_last_function(lc::GameObject::GetRoot(getParent()));

	return tmp_is_the_last_one;
}