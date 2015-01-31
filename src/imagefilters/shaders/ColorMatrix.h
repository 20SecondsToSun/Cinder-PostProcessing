#pragma once
#include "BaseShader.h"

namespace shaders
{	
	namespace imagefilters
	{
		class ColorMatrix : public BaseShader
		{	   
			
		public:
			ColorMatrix()
				:BaseShader("Color Matrix filter")
			{
				using namespace ci;
				shader = gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());	
			
			} 

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY
					(
					  uniform sampler2D texture;
					  uniform float matrix[20];			

					  void main()
					  {
						vec2 vUv = gl_TexCoord[0].st;							
						vec4 c = texture2D(texture, vUv);				
						
						gl_FragColor.r = m[0] * c.r + m[1] * c.g + m[2] * c.b + m[4];
						gl_FragColor.g = m[5] * c.r + m[6] * c.g + m[7] * c.b + m[9];
						gl_FragColor.b = m[10] * c.r + m[11] * c.g + m[12] * c.b + m[14];
						gl_FragColor.a = c.a; 
					});

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				ci::gl::Texture tex = ci::gl::Texture(surf);
				GLfloat  colorMatrix[20] = {0.5997023498159715,0.34553243048391263,-0.2708298674538042,0,47.43192855600873,
			-0.037703249837783157,0.8609577587992641,0.15059552388459913,0,-36.96841498319127,
			0.24113635128153335,-0.07441037908422492,0.44972182064877153,0,-7.562075277591283,
			0,0,0,1,0};

				tex.bind(0);
				shader.bind();
				shader.uniform("texture", 0);				
				shader.uniform("matrix", *colorMatrix);				
				ci::gl::draw(surf);
				shader.unbind();
				tex.unbind();
			}
		};
		typedef std::shared_ptr<ColorMatrix> ColorMatrixRef;
	}
}