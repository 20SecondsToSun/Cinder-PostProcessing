#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class ColorBalance : public BaseShader
		{	    
			float red, green, blue, saturation, hue, exposure;
		
		public:
			ColorBalance(float red, float green, float blue, float saturation, float hue, float exposure)
				:BaseShader("Color Balance filter"), red(red), green(green), blue(blue), saturation(saturation), hue(hue), exposure(exposure)
			{
				using namespace ci;
				shader = gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());	
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("r", &this->red).min(0).max(1).step(.01);
				params->addParam("g", &this->green).min(0).max(1).step(.01);
				params->addParam("b", &this->blue).min(0).max(1).step(.01);
				params->addParam("saturation", &this->saturation).min(-1).max(1).step(.01);
				params->addParam("hue", &this->hue).min(0).max(1).step(.01);
				params->addParam("exposure", &this->exposure).min(0).max(1).step(.01);
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					  uniform sampler2D texture;
					  uniform float red;
					  uniform float green;
					  uniform float blue;
					  uniform float saturation;
					  uniform float hue;
					  uniform float exposure;

					  void main()
					  {
						vec2 vUv = gl_TexCoord[0].st;							
						vec4 color = texture2D(texture, vUv);	

						vec3 balance = 1.0f/(vec3(red, green, blue) *exposure);
				
						float r = color.r * balance.x;
						float g = color.g * balance.y;
						float b = color.b * balance.z;
           
						color.r = r;
						color.g = g;
						color.b = b;

					///hue adjustment, wolfram alpha: RotationTransform[angle, {1, 1, 1}][{x, y, z}] 
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

						///saturation adjustment 

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
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("texture", 0);				
				shader.uniform("red", red);
				shader.uniform("green", green);
				shader.uniform("blue", blue);
				shader.uniform("saturation", saturation); 
				shader.uniform("exposure", exposure); 
				shader.uniform("hue", hue);
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<ColorBalance> ColorBalanceRef;
	}
}