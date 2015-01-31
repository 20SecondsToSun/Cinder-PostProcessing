#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class BrightnessContrast : public BaseShader
		{		
			float contrast;
			float brightness;
		
		public:
			BrightnessContrast(float _contrast, float _brightness) 
				:BaseShader("Brightness Contrast filter"), contrast(_contrast), brightness(_brightness)
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("contrast", &contrast).min(-1.0f).max(1.0f).step(.05f);
				params->addParam("brightness", &brightness).min(-1.0f).max(1.0f).step(.05f);
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
										 uniform sampler2D tex0;
                                         uniform float contrast;
                                         uniform float brightness;
                                        
                                         void main()
										 {
                                            vec4 color = texture2D(tex0, gl_TexCoord[0].st);
                                             
                                            color.rgb += brightness;

											if (contrast > 0.0)											
												color.rgb = (color.rgb - 0.5) / (1.0 - contrast) + 0.5;											
											else 											
												color.rgb = (color.rgb - 0.5) * (1.0 + contrast) + 0.5;											
                                             
                                             gl_FragColor =  vec4(color.r , color.g, color.b, 1.0);
                                         });

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("basetex", 0);	
				shader.uniform("contrast", contrast);
				shader.uniform("brightness", brightness);
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<BrightnessContrast> BrightnessContrastRef;
	}
}