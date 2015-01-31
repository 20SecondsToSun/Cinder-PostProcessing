#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class Bloom : public BaseShader
		{		
			float bloom;

		public:
			Bloom(float _bloom) :BaseShader("Bloom filter"), bloom(_bloom)
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());				
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("bloom", &bloom).min(-10.1f).max(100.0f).step(.1f);				
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					    // varying vec2 texcoord;  
						uniform sampler2D tex0;
						uniform float bloom;  

						void main()  
						{  
							vec2 vUv = gl_TexCoord[0].st;
							vec4 base = texture2D(tex0, vUv);

							vec4 sum;						 
							int i;
							int j; 
							
							for( i = -3 ;i <= 3; i++)    
							{    
								for (j = -3; j <= 3; j++)    
								{    
									sum += texture2D(tex0, vUv  + vec2(j, i) * 2.0) * 0.25;  // 2.0 = "radius step"
								}    
							}    

							 sum = sum * sum * 0.012; // 0.012 = arbitrary value   
							 vec4 dry = texture2D(tex0, vUv);
							 gl_FragColor = bloom * (1.0 - ((1.0 - sum) * (1.0 - dry))) +  ((1.0 - bloom) * dry); // screen mode blending  
						});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				ci::gl::enableAlphaBlending();
				shader.uniform("tex0", 0);	
				shader.uniform("bloom", bloom);
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}	
		};
		typedef std::shared_ptr<Bloom> BloomRef;
	}
}