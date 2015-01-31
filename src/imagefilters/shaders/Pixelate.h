#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class Pixelate : public BaseShader
		{		    
			ci::Vec2f resolution;			

		public:
			Pixelate(const ci::Vec2f& resolution) 
				:BaseShader("Pixelate filter"),
				resolution(resolution)
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());			
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("resolution.x", &this->resolution.x).min(0).max(1000).step(1);
				params->addParam("resolution.y", &this->resolution.y).min(0).max(1000).step(1);				
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					uniform sampler2D tex;
					uniform float xPixels;
					uniform float yPixels;

				void main()
				{
					vec2 texCoords = vec2(floor(gl_TexCoord[0].s * xPixels) / xPixels, floor(gl_TexCoord[0].t * yPixels) / yPixels);
					gl_FragColor = texture2D(tex, texCoords);
				}
				);

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("tex", 0);	
				shader.uniform("xPixels", resolution.x);
				shader.uniform("yPixels", resolution.y);			
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<Pixelate> PixelateRef;
	}
}