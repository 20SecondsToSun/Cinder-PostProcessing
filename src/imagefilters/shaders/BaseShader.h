#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include "cinder/gl/GlslProg.h"

#define STRINGIFY(s) #s

namespace shaders
{	
	namespace imagefilters
	{
		class BaseShader
		{
		protected:			
			ci::gl::GlslProg shader;

		public:
			BaseShader(std::string title = "default"):title(title)
			{
								
			}

			// The GOL vertex shader is a simple pass-through:
			const char *GET_PASSTHROUGH_VERTEX() const
			{	
				static const std::string shdr = STRINGIFY(
					void main() {
						gl_FrontColor  = gl_Color;
						gl_TexCoord[0] = gl_MultiTexCoord0;
						gl_Position    = ftransform();
				}
				);

				return shdr.c_str();
			}

			virtual const char * GET_FRAG() = 0;//{return NULL;};
			virtual void render(ci::Surface surf) = 0;	
			virtual void createParams(ci::params::InterfaceGlRef params) = 0;	

			virtual std::string getTitle() {return title;};

		protected:
			std::string title;
		};

		typedef std::shared_ptr<BaseShader> BaseShaderRef;
	}
}