#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class ColorHalftone : public BaseShader
		{	    
			float angle;	
			float scale;
			ci::Vec2i center;

		public:
			ColorHalftone(ci::Vec2i center, float angle, float scale)
				:BaseShader("ColorHalftone filter"), center(center), angle(angle), scale(scale)
			{
				using namespace ci;
				shader = gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());	
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("center.x", &this->center.x).min(0).max(1000).step(1);
				params->addParam("center.y", &this->center.y).min(0).max(1000).step(1);
				params->addParam("angle", &this->angle).min(0).max(3.14*2).step(0.1);
				params->addParam("scale", &this->scale).min(0).max(10).step(0.1);	
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					uniform sampler2D texture;
					uniform vec2 center;
					uniform float angle;
					uniform float scale;
					uniform int texSizeX;				
					uniform int texSizeY;		

					float pattern(int x, int y, float angle) 
					{
						float s = sin(angle);
						float c = cos(angle);

						vec2 tex = gl_TexCoord[0].st * vec2(x, y) - center;
						vec2 point = vec2(c * tex.x - s * tex.y, s * tex.x + c * tex.y) * scale;
						return (sin(point.x) * sin(point.y)) * 4.0;
					}

					void main()
					{													
						vec4 color = texture2D(texture, gl_TexCoord[0].st);    						
				
						 vec3 cmy = 1.0 - color.rgb;
						 float k = min(cmy.x, min(cmy.y, cmy.z));
						 cmy = (cmy - k) / (1.0 - k);
						 cmy = clamp(cmy * 10.0 - 3.0 + vec3(pattern(texSizeX, texSizeY, angle + 0.26179), pattern(texSizeX, texSizeY, angle + 1.30899), pattern(texSizeX, texSizeY, angle)), 0.0, 1.0);
						 k = clamp(k * 10.0 - 5.0 + pattern(texSizeX, texSizeY, angle + 0.78539), 0.0, 1.0);
						 gl_FragColor = vec4(1.0 - cmy - k, color.a);
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
				shader.uniform("center", center);
				shader.uniform("angle", angle);
				shader.uniform("scale", scale);	
				shader.uniform("texSizeX", tex.getSize().x);			
				shader.uniform("texSizeY", tex.getSize().y);			
				gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<ColorHalftone> ColorHalftoneRef;
	}
}