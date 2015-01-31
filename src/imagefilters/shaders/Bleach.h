#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class Bleach : public BaseShader
		{				    
			float opacity;

		public:
			Bleach(float _opacity) :BaseShader("Bleach filter"), opacity(_opacity)
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
					uniform float opacity;
					uniform sampler2D tDiffuse;
					void main() {
					vec2 vUv = gl_TexCoord[0].st;
					vec4 base = texture2D( tDiffuse, vUv );

					vec3 lumCoeff = vec3( 0.25, 0.65, 0.1 );
					float lum = dot( lumCoeff, base.rgb );
					vec3 blend = vec3( lum );

					float L = min( 1.0, max( 0.0, 10.0 * ( lum - 0.45 ) ) );

					vec3 result1 = 2.0 * base.rgb * blend;
					vec3 result2 = 1.0 - 2.0 * ( 1.0 - blend ) * ( 1.0 - base.rgb );

					vec3 newColor = mix( result1, result2, L );

					float A2 = opacity * base.a;
					vec3 mixRGB = A2 * newColor.rgb;
					mixRGB += ( ( 1.0 - A2 ) * base.rgb );

					gl_FragColor = vec4( mixRGB, base.a );
				});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
				shader.bind();
				shader.uniform("tDiffuse", 0);	
				shader.uniform("opacity", opacity);			
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<Bleach> BleachRef;
	}
}