#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class Vibrance : public BaseShader
		{	    
			float amount;	
		
		public:
			Vibrance(float amount)
				:BaseShader("Vibrance filter"), amount(amount)
			{
				using namespace ci;
				shader = gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());	
			} 

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("amount", &this->amount).min(-3).max(3).step(.1);
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					  uniform sampler2D texture;
					  uniform float amount;				

					  void main()
					  {
						vec2 vUv = gl_TexCoord[0].st;							
						vec4 color = texture2D(texture, vUv);
						
						float average = (color.r + color.g + color.b) / 3.0;
						float mx = max(color.r, max(color.g, color.b));
						float amt = (mx - average) * (-amount * 3.0);
						color.rgb = mix(color.rgb, vec3(mx), amt);          

						gl_FragColor = color;
					});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("texture", 0);				
				shader.uniform("amount", amount);			
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<Vibrance> VibranceRef;
	}
}