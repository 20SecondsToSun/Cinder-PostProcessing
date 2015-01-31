#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class Lut : public BaseShader
		{				    
			float opacity;

		public:
			Lut(float _opacity)
				:BaseShader("Lut filter"), opacity(_opacity)
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				params->addParam("opacity", &opacity).min(0.0f).max(1).step(.1f);				
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					varying vec2 texcoord;
					uniform sampler2DRect tex;

					uniform sampler2DRect texLUT;

					void main (void)
					{
						vec4 textureColor = texture2DRect(tex, texcoord);
     
						float blueColor = textureColor.b * 63.0;
     
						vec2 quad1;
						quad1.y = floor(floor(blueColor) / 8.0);
						quad1.x = floor(blueColor) - (quad1.y * 8.0);
     
						vec2 quad2;
						quad2.y = floor(ceil(blueColor) / 8.0);
						quad2.x = ceil(blueColor) - (quad2.y * 8.0);
     
						vec2 texPos1;
						texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
						texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);
     
						vec2 texPos2;
						texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
						texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);
     
						vec4 newColor1 = texture2DRect(texLUT, texPos1*512.);
						vec4 newColor2 = texture2DRect(texLUT, texPos2*512.);
     
						vec4 newColor = mix(newColor1, newColor2, fract(blueColor));
						gl_FragColor = vec4(newColor.rgb, textureColor.w);
     
					});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				using namespace ci;
				using namespace ci::gl;

				Texture tex = Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("tDiffuse", 0);	
				shader.uniform("opacity", opacity);			
				gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<Lut> LutRef;
	}
}