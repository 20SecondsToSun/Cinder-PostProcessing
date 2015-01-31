#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class Noise : public BaseShader
		{	    
			float amount;	
		
		public:
			Noise(float amount)
				:BaseShader("Noise filter"), amount(amount)
			{
				using namespace ci;
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());		
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("amount", &this->amount).min(0).max(1).step(.1);
			}

			const char *GET_FRAG()
			{
				static const std::string shdr = STRINGIFY(
					  uniform sampler2D texture;
					  uniform float amount;

					  float rand(vec2 co) 
					  {
						 return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
					  }

					  void main()
					  {
						vec2 vUv = gl_TexCoord[0].st;							
						vec4 color = texture2D(texture, vUv);		       
      
						float diff = (rand(vUv) - 0.5) * amount;
						color.r += diff;
						color.g += diff;
						color.b += diff;

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
		typedef std::shared_ptr<Noise> NoiseRef;
	}
}