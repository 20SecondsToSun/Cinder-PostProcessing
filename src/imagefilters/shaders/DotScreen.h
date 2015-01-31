#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class DotScreen : public BaseShader
		{	    
			float angle;	
			float scale;
			ci::Vec2i center;

		public:
			DotScreen(ci::Vec2i center, float angle, float scale)
				:BaseShader("DotScreen filter"), center(center), angle(angle), scale(scale)
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

					float pattern(int x, int y) 
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
						float average = (color.r + color.g + color.b) / 3.0;
						float f = average * 10.0 - 5.0 + pattern(texSizeX, texSizeY);
						gl_FragColor = vec4(vec3(f,f,f), color.a);
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
		typedef std::shared_ptr<DotScreen> DotScreenRef;
	}
}