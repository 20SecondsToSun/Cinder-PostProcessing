#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class Brcosa : public BaseShader
		{		
			float saturation;
			float contrast;
			float brightness;
			float alpha;

		public:
			Brcosa(float _saturation, float _contrast, float _brightness, float _alpha) 
				:BaseShader("Brcosa filter"), saturation(_saturation), contrast(_contrast), brightness(_brightness), alpha(_alpha)
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());				
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("saturation", &saturation).min(0.0f).max(1.0f).step(0.1f);
				params->addParam("contrast", &contrast).min(0.0f).max(10.0f).step(0.1f);
				params->addParam("brightness", &brightness).min(0.0f).max(1.0f).step(0.1f);
				params->addParam("alpha", &alpha).min(0.0f).max(1.0f).step(0.1f);		
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					    const vec3 LumCoeff = vec3 (0.2125, 0.7154, 0.0721);
						
						uniform sampler2D tex;

						uniform vec3 avgluma;
						uniform float saturation;
						uniform float contrast;
						uniform float brightness;
						uniform float alpha;

						void main (void)
						{
							vec2 vUv = gl_TexCoord[0].st;
							vec4 base = texture2D(tex, vUv);

							vec3 texColor  	= base.rgb;
							vec3 intensity 	= vec3 (dot(texColor, LumCoeff));
							vec3 color     	= mix(intensity, texColor, saturation);
							color          	= mix(avgluma, color, contrast);
							color			*= brightness;
							gl_FragColor   	= vec4(color,alpha);
						});

				return shdr.c_str();
			}

			void render(ci::Surface surf) override
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				ci::gl::enableAlphaBlending();
				shader.uniform("tex", 0);	
				shader.uniform("saturation", saturation);
				shader.uniform("contrast", contrast);
				shader.uniform("brightness", brightness);
				shader.uniform("alpha", alpha);

				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}		
		};
		typedef std::shared_ptr<Brcosa> BrcosaRef;
	}
}