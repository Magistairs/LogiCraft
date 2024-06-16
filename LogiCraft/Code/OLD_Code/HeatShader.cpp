#include "HeatShader.h"
#include "ToolsBar.h"

namespace lc
{
    namespace Shader
    {
        HeatShader::HeatShader()
            : m_distortion_factor_(0.01f), m_rise_factor_(0.5f), m_update_parent_iterator_timer_(0.f), m_is_in_view_(true)
        {
			m_name = "Heat_Shader";
        	m_typeName = "Heat_Shader";
        	m_type = TYPE::SHADER;
        	
            HeatShader::setup_shader_script_string();
            
            m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	
        	m_texture_map_.loadFromFile("../Ressources/ALL/SHADERS/Heat_map.png");
            m_texture_map_.setRepeated(true);
            m_texture_map_.setSmooth(true);

            m_shader_->setUniform("u_distortion_map_texture", m_texture_map_);
            m_shader_->setUniform("u_current_texture", sf::Shader::CurrentTexture);

        	m_render_size_ = { 500u, 500u };
    
        	m_render_texture_ = std::make_shared<sf::RenderTexture>();
        	m_render_texture_->create(m_render_size_.x, m_render_size_.y);
    
        	m_render_view_ = { sf::Vector2f(m_render_texture_->getSize() / 2u), sf::Vector2f(m_render_texture_->getSize()) };
        }

        HeatShader::~HeatShader()
        {
        	m_shader_.reset();
        	m_render_texture_.reset();
        }

        void HeatShader::UpdateEvent(sf::Event& event)
        {
        }

        void HeatShader::Update(WindowManager& window)
        {
        	if (getParent() && (!getParent()->isVisible() || !m_isVisible))
        	{
        		const std::function<void(const std::shared_ptr<lc::GameObject>&)> show_all_colliders = [&](const std::shared_ptr<lc::GameObject>& game_object)
        		{
        			if (game_object->getDepth() >= getParent()->getDepth() && game_object->isDrawByAShader())
        				game_object->isDrawByAShader(false);
				
        			for (const auto& tmp_game_object : game_object->getObjects())
        				show_all_colliders(tmp_game_object);
        		};

        		show_all_colliders(lc::GameObject::GetRoot(getParent()));
        	}

        	m_update_parent_iterator_timer_ += Tools::getDeltaTime();
        	if (m_update_parent_iterator_timer_ >= 1.f)
        	{
        		bool tmp_is_done(false);
        		const auto tmp_world = lc::GameObject::GetRoot(getParent());
        		const std::function<void(const std::list<std::shared_ptr<lc::GameObject>>::iterator&)> update_parent_iterator =
					[&](const std::list<std::shared_ptr<lc::GameObject>>::iterator& parent)
					{
						if (getParent() == (*parent))
						{
							tmp_is_done = true;
							if((*parent)->getParent() == tmp_world)
								m_parent_iterator_ = parent;
							return;  
						}
						else
						{
							for (auto game_object = (*parent)->getObjects().begin(); game_object != (*parent)->getObjects().end(); ++game_object)
							{
								update_parent_iterator(game_object);
								if (tmp_is_done)
								{
									if((*parent)->getParent() == tmp_world)
										m_parent_iterator_ = parent;
									return;   
								}
							}   
						}
					};
        
        		for (auto game_object = tmp_world->getObjects().begin(); game_object != tmp_world->getObjects().end(); ++game_object)
        		{
        			update_parent_iterator(game_object);
        			if (tmp_is_done)
        				break;
        		}

        		m_update_parent_iterator_timer_ = 0.f;
        	}
        }

        void HeatShader::Draw(WindowManager& window)
        {
            m_time_ += Tools::getDeltaTime();

            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	if (m_render_size_ != m_render_texture_->getSize())
        		m_render_texture_->create(m_render_size_.x, m_render_size_.y);

        	m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
        	m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

        	if (m_is_in_view_)
        	{
        		m_render_texture_->setView(m_render_view_);
        		m_render_texture_->clear(sf::Color::Black);   
        	}

	        for (int i = static_cast<int>(ToolsBar::GetLayers().size()) - 1; i >= 0; --i)
	        {
	        	for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
	        		this->draw_in_shader(obj_element, window, i);   
	        }
        	
        	if (m_is_in_view_)
        	{
        		m_render_texture_->display();

        		m_renderer.setTexture(&m_render_texture_->getTexture());
        		m_renderer.setTextureRect({{0, 0}, sf::Vector2i(m_render_texture_->getSize())});
        		m_renderer.setSize(sf::Vector2f(m_render_texture_->getSize()));
        		m_renderer.setPosition(getParent()->getTransform().getPosition());
        		m_renderer.setScale(getParent()->getTransform().getScale());
        		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
        		m_renderer.setRotation(getParent()->getTransform().getRotation());

        		window.draw(m_renderer, m_shader_states_);
        	}

        	m_is_in_view_ = getParent()->is_in_window_view(window);
        }

        void HeatShader::Draw(sf::RenderTexture& window)
        {
            m_time_ += Tools::getDeltaTime();

        	//Set the shader uniform values.
            m_shader_->setUniform("u_distortion_factor", m_distortion_factor_);
            m_shader_->setUniform("u_rise_factor", m_rise_factor_);
            m_shader_->setUniform("u_time", m_time_);

        	//If the zone where the shader is applied changed, the render texture is recreated with the good size.
        	if (m_render_size_ != m_render_texture_->getSize())
        		m_render_texture_->create(m_render_size_.x, m_render_size_.y);

        	//Set the view center and size.
        	m_render_view_.setCenter(getParent()->getTransform().getPosition() + sf::Vector2f(m_render_texture_->getSize() / 2u));
        	m_render_view_.setSize(sf::Vector2f(m_render_texture_->getSize()));

        	//Set the view a clear the render texture.
        	if (m_is_in_view_)
        	{
        		m_render_texture_->setView(m_render_view_);
        		m_render_texture_->clear(sf::Color::Black);   
        	}

        	//Display of the object in the shader.
	        for (int i = static_cast<int>(ToolsBar::GetLayers().size()) - 1; i >= 0; --i)
	        {
	        	for (const auto& obj_element : lc::GameObject::GetRoot(getParent())->getObjects())
	        		this->draw_in_shader(obj_element, window, i);   
	        }

        	//Then set the texture of the render texture on the renderer and draw it with the shader.
        	if (m_is_in_view_)
        	{
        		m_render_texture_->display();

        		m_renderer.setTexture(&m_render_texture_->getTexture());
        		m_renderer.setTextureRect({{0, 0}, sf::Vector2i(m_render_texture_->getSize())});
        		m_renderer.setSize(sf::Vector2f(m_render_texture_->getSize()));
        		m_renderer.setPosition(getParent()->getTransform().getPosition());
        		m_renderer.setScale(getParent()->getTransform().getScale());
        		m_renderer.setOrigin(getParent()->getTransform().getOrigin());
        		m_renderer.setRotation(getParent()->getTransform().getRotation());

        		window.draw(m_renderer, m_shader_states_);
        	}

        	m_is_in_view_ = getParent()->is_in_window_view(window);
        }

        void HeatShader::Save(std::ofstream& save, sf::RenderTexture& texture, int depth)
        {
        	save << static_cast<int>(m_type)
			 << " " << m_typeName
             << " " << m_render_size_
        	 << " " << m_rise_factor_
        	 << " " << m_distortion_factor_;
        }

    	void HeatShader::Export(std::ofstream& exportation)
        {
        	exportation << static_cast<int>(m_type)
			 << " " << m_typeName
			 << " " << m_render_size_
			 << " " << m_rise_factor_
			 << " " << m_distortion_factor_;
        }


        void HeatShader::Load(std::ifstream& load)
        {
        	std::string tmp_texture_name;
        	load >> m_render_size_ >> m_rise_factor_ >> m_distortion_factor_;
        }

        std::shared_ptr<lc::GameComponent> HeatShader::Clone()
        {
        	auto tmp_component = std::make_shared<HeatShader>(*this);
        	tmp_component->m_shader_ = std::make_shared<sf::Shader>();
        	tmp_component->m_shader_->loadFromMemory(m_shader_script_, sf::Shader::Fragment);
        	tmp_component->m_shader_states_.shader = tmp_component->m_shader_.get();
        	tmp_component->m_render_texture_ = std::make_shared<sf::RenderTexture>();
        	tmp_component->m_render_texture_->create(m_render_size_.x, m_render_size_.y);
            return tmp_component;
        }

        void HeatShader::setHierarchieFunc()
        {
            m_hierarchieInformation = [this]()
            {
            	ImGui::DragFloat("Distortion Factor", &m_distortion_factor_, 0.001f);
            	ImGui::DragFloat("Rise Factor", &m_rise_factor_, 0.01f);

            	unsigned int tmp_tab[2]{ m_render_size_.x, m_render_size_.y };
            	ImGui::DragScalarN("Heat Size", ImGuiDataType_U32, &tmp_tab, 2);
            	m_render_size_ = {tmp_tab[0], tmp_tab[1]};
            };
        }

        void HeatShader::setup_shader_script_string()
        {
            m_shader_script_ = R"(#version 130

uniform sampler2D u_current_texture; // Our render texture 
uniform sampler2D u_distortion_map_texture; // Our heat distortion map texture

uniform float u_distortion_factor; // Factor used to control severity of the effect
uniform float u_rise_factor; // Factor used to control how fast air rises
uniform float u_time; // Time used to scroll the distortion map

void main()
{
    vec2 distortionMapCoordinate = gl_TexCoord[0].st;

    // We use the time value to scroll our distortion texture upwards
    // Since we enabled texture repeating, OpenGL takes care of
    // coordinates that lie outside of [0, 1] by discarding
    // the integer part and keeping the fractional part
    // Basically performing a floating point modulo 1
    // 1.1 = 0.1, 2.4 = 0.4, 10.3 = 0.3 etc.

    distortionMapCoordinate.t -= u_time * u_rise_factor;

    vec4 distortionMapValue = texture2D(u_distortion_map_texture, distortionMapCoordinate);

    // The values are normalized by OpenGL to lie in the range [0, 1]
    // We want negative offsets too, so we subtract 0.5 and multiply by 2
    // We end up with values in the range [-1, 1]

    vec2 distortionPositionOffset = distortionMapValue.xy;
    distortionPositionOffset -= vec2(0.5f, 0.5f);
    distortionPositionOffset *= 2.f;

    // The factor scales the offset and thus controls the severity
    distortionPositionOffset *= u_distortion_factor;

    // The latter 2 channels of the texture are unused... be creative
    vec2 distortionUnused = distortionMapValue.zw;

    // Since we all know that hot air rises and cools,
    // the effect loses its severity the higher up we get
    // We use the t (a.k.a. y) texture coordinate of the original texture
    // to tell us how high up we are and damp accordingly
    // Remember, OpenGL 0 is at the bottom

    distortionPositionOffset *= 1.f - gl_TexCoord[0].t; //Do 1.f - gl_TexCoord[0].t to reverse it and made the heat start upward.

    vec2 distortedTextureCoordinate = gl_TexCoord[0].st + distortionPositionOffset;

    gl_FragColor = gl_Color * texture2D(u_current_texture, distortedTextureCoordinate);
}
)";
        }

        void HeatShader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, WindowManager& window, const unsigned char& depth)
        {
		    //The shader will affect only the objects who are under or on the same depth.
		    if (game_object->getDepth() == depth && !game_object->hasComponent("Heat_Shader"))
		    {
		        bool m_can_be_drawn(true);
		        if (game_object->getDepth() == getParent()->getDepth())
		        {
		            const auto tmp_world = lc::GameObject::GetRoot(getParent());

		            auto tmp_higher_parent = getParent();
		            while (tmp_higher_parent->getParent() != tmp_world)
		            {
		                tmp_higher_parent = tmp_higher_parent->getParent();
		                if (game_object == tmp_higher_parent)
		                {
		                    m_can_be_drawn = false;
		                    break;
		                }
		            }
		        
		            if (m_parent_iterator_._Ptr && !m_can_be_drawn)
		            {
		                if (m_parent_iterator_._Ptr->_Myval == tmp_higher_parent)
		                    if (std::next(m_parent_iterator_) != tmp_world->getObjects().end())
		                        for (auto it_game_object = std::next(m_parent_iterator_); it_game_object != tmp_world->getObjects().end(); ++it_game_object)
		                            if ((*it_game_object) == game_object)
		                            {
		                                m_can_be_drawn = false;
		                                break;
		                            }
		            }
		        }
		        else if (game_object->getDepth() < getParent()->getDepth())
		            m_can_be_drawn = false;

		        if (m_can_be_drawn)
		        {
		        	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		        	{
		        		game_object->getBeforeSimulateParallaxPos() = game_object->getTransform().getPosition();
		        		game_object->getTransform().getPosition() += game_object->GetOffset(lc::GameObject::GetRoot(game_object->getParent()), game_object->getDepth());
		        	}

		        	const auto tmp_position = game_object->getTransform().getPosition();
		        	const auto tmp_size = game_object->getTransform().getSize();
		            
		            game_object->getTransform().getPosition() = game_object->getTransform().getPosition() - sf::Vector2f(100.f, 100.f);
		            game_object->getTransform().getSize() = game_object->getTransform().getSize() + sf::Vector2f(200.f, 200.f);
		            
		            //If the object is totally in the zone of the shader, then one draw is done.
		            if (this->is_totally_in(game_object) && m_is_in_view_)
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;
		                
		                if (game_object->isVisible())
		                    for (const auto& component : game_object->getComponents())
		                        if (component->isVisible())
		                            component->Draw(*m_render_texture_);

		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();

		                game_object->isDrawByAShader(true); //The object is made invisible so is not drawn two times.
		            }
		            //If the object is not totally in the zone of the shader,
		            //then he needs to be drawn one time in the window and a second time in the render texture.
		            else if (Tools::Collisions::rect_rect(
		                {getParent()->getTransform().getPosition(),
		                    getParent()->getTransform().getSize() *
		                        sf::Vector2f(std::abs(getParent()->getTransform().getScale().x), std::abs(getParent()->getTransform().getScale().y))},
		                {game_object->getTransform().getPosition(),
		                    game_object->getTransform().getSize() *
		                        sf::Vector2f(std::abs(game_object->getTransform().getScale().x), std::abs(game_object->getTransform().getScale().y))}))
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;
		            	
		                if (game_object->isVisible())
		                    for (const auto& component : game_object->getComponents())
		                        if (component->isVisible())
		                            if (m_is_in_view_)
		                                component->Draw(*m_render_texture_);

		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();
		            	
		                game_object->isDrawByAShader(false);
		            }
		            else
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;

		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();
		                
		                game_object->isDrawByAShader(false);
		            }
		        }
		    }

		    //We do the same if the object has children.
		    for (const auto& obj_element : game_object->getObjects())
		        this->draw_in_shader(obj_element, window, depth);
        }

        void HeatShader::draw_in_shader(const std::shared_ptr<lc::GameObject>& game_object, sf::RenderTexture& window, const unsigned char& depth)
        {
		    //The shader will affect only the objects who are under or on the same depth.
		    if (game_object->getDepth() == depth && !game_object->hasComponent("Heat_Shader"))
		    {
		        bool m_can_be_drawn(true);
		        if (game_object->getDepth() == getParent()->getDepth())
		        {
		            const auto tmp_world = lc::GameObject::GetRoot(getParent());

		            auto tmp_higher_parent = getParent();
		            while (tmp_higher_parent->getParent() != tmp_world)
		            {
		                tmp_higher_parent = tmp_higher_parent->getParent();
		                if (game_object == tmp_higher_parent)
		                {
		                    m_can_be_drawn = false;
		                    break;
		                }
		            }
		        
		            if (m_parent_iterator_._Ptr && !m_can_be_drawn)
		            {
		                if (m_parent_iterator_._Ptr->_Myval == tmp_higher_parent)
		                    if (std::next(m_parent_iterator_) != tmp_world->getObjects().end())
		                        for (auto it_game_object = std::next(m_parent_iterator_); it_game_object != tmp_world->getObjects().end(); ++it_game_object)
		                            if ((*it_game_object) == game_object)
		                            {
		                                m_can_be_drawn = false;
		                                break;
		                            }
		            }
		        }
		        else if (game_object->getDepth() < getParent()->getDepth())
		            m_can_be_drawn = false;

		        if (m_can_be_drawn)
		        {
		        	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		        	{
		        		game_object->getBeforeSimulateParallaxPos() = game_object->getTransform().getPosition();
		        		game_object->getTransform().getPosition() += game_object->GetOffset(lc::GameObject::GetRoot(game_object->getParent()), game_object->getDepth());
		        	}

		        	const auto tmp_position = game_object->getTransform().getPosition();
		        	const auto tmp_size = game_object->getTransform().getSize();
		            
		            game_object->getTransform().getPosition() = game_object->getTransform().getPosition() - sf::Vector2f(100.f, 100.f);
		            game_object->getTransform().getSize() = game_object->getTransform().getSize() + sf::Vector2f(200.f, 200.f);
		            
		            //If the object is totally in the zone of the shader, then one draw is done.
		            if (this->is_totally_in(game_object) && m_is_in_view_)
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;
		                
		                if (game_object->isVisible())
		                    for (const auto& component : game_object->getComponents())
		                        if (component->isVisible())
		                            component->Draw(*m_render_texture_);
		            	
		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();

		                game_object->isDrawByAShader(true); //The object is made invisible so is not drawn two times.
		            }
		            //If the object is not totally in the zone of the shader,
		            //then he needs to be drawn one time in the window and a second time in the render texture.
		            else if (Tools::Collisions::rect_rect(
		                {getParent()->getTransform().getPosition(),
		                    getParent()->getTransform().getSize() *
		                        sf::Vector2f(std::abs(getParent()->getTransform().getScale().x), std::abs(getParent()->getTransform().getScale().y))},
		                {game_object->getTransform().getPosition(),
		                    game_object->getTransform().getSize() *
		                        sf::Vector2f(std::abs(game_object->getTransform().getScale().x), std::abs(game_object->getTransform().getScale().y))}))
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;
		                
		                if (game_object->isVisible())
		                    for (const auto& component : game_object->getComponents())
		                        if (component->isVisible())
		                            if (m_is_in_view_)
		                                component->Draw(*m_render_texture_);

		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();

		                game_object->isDrawByAShader(false);
		            }
		            else
		            {
		                game_object->getTransform().getPosition() = tmp_position;
		                game_object->getTransform().getSize() = tmp_size;

		            	if(game_object->getParent() && game_object->getName() != PLAYER_NAME && !game_object->getFirstPassInit()[1])
		            		game_object->getTransform().getPosition() = game_object->getBeforeSimulateParallaxPos();
		                
		                game_object->isDrawByAShader(false);
		            }
				}
			}

		    //We do the same if the object has children.
		    for (const auto& obj_element : game_object->getObjects())
		        this->draw_in_shader(obj_element, window, depth);
        }
    }
}