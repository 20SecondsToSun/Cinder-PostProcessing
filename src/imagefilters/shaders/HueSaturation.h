#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class HueSaturation : public BaseShader
		{	    
			float hue;	
			float saturation;

		public:
			HueSaturation(float hue, float saturation)
				:BaseShader("HueSaturation filter"), hue(hue), saturation(saturation)
			{
				using namespace ci;
				shader = gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());	
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("hue", &this->hue).min(0).max(1).step(.1);
				params->addParam("saturation", &this->saturation).min(0).max(1).step(0.1);
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					uniform sampler2D texture;
					uniform float hue;
					uniform float saturation;
					//varying vec2 texCoord;
						void main()
						{
							vec2 vUv = gl_TexCoord[0].st;							
							vec4 color = texture2D(texture, vUv);

						//	///* hue adjustment, wolfram alpha: RotationTransform[angle, {1, 1, 1}][{x, y, z}] */
							float angle = hue * 3.14159265;
							float s = sin(angle);
							float c = cos(angle);
							vec3 weights = (vec3(2.0 * c, -sqrt(3.0) * s - c, sqrt(3.0) * s - c) + 1.0) / 3.0;
							float len = length(color.rgb);
							color.rgb = vec3(
								dot(color.rgb, weights.xyz),
								dot(color.rgb, weights.zxy),
								dot(color.rgb, weights.yzx)
							);

							///* saturation adjustment */

							float average = (color.r + color.g + color.b) / 3.0;
							if (saturation > 0.0) 
							{
								color.rgb += (average - color.rgb) * (1.0 - 1.0 / (1.001 - saturation));
							}
							else
							{
								color.rgb += (average - color.rgb) * (-saturation);
							}
							
							gl_FragColor = color;
						});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				using namespace ci;
				
				gl::Texture tex = gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("texture", 0);				
				shader.uniform("hue", hue);
				shader.uniform("saturation", saturation);			
				gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<HueSaturation> HueSaturationRef;
	}
}