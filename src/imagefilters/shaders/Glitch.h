#pragma once
#include "BaseShader.h"

namespace shaders
{
	namespace imagefilters
	{
		class Glitch : public BaseShader
		{		
			float segments;

		public:
			Glitch()
				:BaseShader("Glitch filter")
			{
				shader = ci::gl::GlslProg(GET_PASSTHROUGH_VERTEX(), GET_FRAG());			
			}

			void createParams(ci::params::InterfaceGlRef params)
			{
				params->clear();
				//params->addParam("segments", &segments).min(0).max(100).step(1);
			}

			const char *GET_FRAG() override
			{
				static const std::string shdr = STRINGIFY(
					#ifdef GL_ES
					precision highp float;
					#endif

					uniform sampler2D tDiffuse;
					uniform sampler2D tLensDirt;
					uniform sampler2D tGlitch;				

					uniform float time;
					uniform vec3 resolution;

					uniform float glitch_intensity;
					uniform vec3 offset_color;

					vec4 mod289(vec4 x)
					{
					  return x - floor(x * (1.0 / 289.0)) * 289.0;
					}

					vec4 permute(vec4 x)
					{
					  return mod289(((x*34.0)+1.0)*x);
					}

					vec4 taylorInvSqrt(vec4 r)
					{
					  return 1.79284291400159 - 0.85373472095314 * r;
					}

					vec2 fade(vec2 t) {
					  return t*t*t*(t*(t*6.0-15.0)+10.0);
					}

					// Classic Perlin noise
					float cnoise(vec2 P)
					{
					  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
					  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
					  Pi = mod289(Pi); // To avoid truncation effects in permutation
					  vec4 ix = Pi.xzxz;
					  vec4 iy = Pi.yyww;
					  vec4 fx = Pf.xzxz;
					  vec4 fy = Pf.yyww;

					  vec4 i = permute(permute(ix) + iy);

					  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
					  vec4 gy = abs(gx) - 0.5 ;
					  vec4 tx = floor(gx + 0.5);
					  gx = gx - tx;

					  vec2 g00 = vec2(gx.x,gy.x);
					  vec2 g10 = vec2(gx.y,gy.y);
					  vec2 g01 = vec2(gx.z,gy.z);
					  vec2 g11 = vec2(gx.w,gy.w);

					  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
					  g00 *= norm.x;
					  g01 *= norm.y;
					  g10 *= norm.z;
					  g11 *= norm.w;

					  float n00 = dot(g00, vec2(fx.x, fy.x));
					  float n10 = dot(g10, vec2(fx.y, fy.y));
					  float n01 = dot(g01, vec2(fx.z, fy.z));
					  float n11 = dot(g11, vec2(fx.w, fy.w));

					  vec2 fade_xy = fade(Pf.xy);
					  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
					  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
					  return 2.3 * n_xy;
					}

					// Classic Perlin noise, periodic variant
					float pnoise(vec2 P, vec2 rep)
					{
					  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
					  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
					  Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
					  Pi = mod289(Pi); // To avoid truncation effects in permutation
					  vec4 ix = Pi.xzxz;
					  vec4 iy = Pi.yyww;
					  vec4 fx = Pf.xzxz;
					  vec4 fy = Pf.yyww;

					  vec4 i = permute(permute(ix) + iy);

					  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
					  vec4 gy = abs(gx) - 0.5 ;
					  vec4 tx = floor(gx + 0.5);
					  gx = gx - tx;

					  vec2 g00 = vec2(gx.x,gy.x);
					  vec2 g10 = vec2(gx.y,gy.y);
					  vec2 g01 = vec2(gx.z,gy.z);
					  vec2 g11 = vec2(gx.w,gy.w);

					  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
					  g00 *= norm.x;
					  g01 *= norm.y;
					  g10 *= norm.z;
					  g11 *= norm.w;

					  float n00 = dot(g00, vec2(fx.x, fy.x));
					  float n10 = dot(g10, vec2(fx.y, fy.y));
					  float n01 = dot(g01, vec2(fx.z, fy.z));
					  float n11 = dot(g11, vec2(fx.w, fy.w));

					  vec2 fade_xy = fade(Pf.xy);
					  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
					  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
					  return 2.3 * n_xy;
					}

					void main(void)
					{
					  vec2 vUv = gl_TexCoord[0].st;
					  vec4 colorDirt = texture2D(tLensDirt, vUv);
					  vec4 glitchDisplace = texture2D(tGlitch, vec2(mod(mod(time, 142.0) * 0.1 + vUv.x * 0.05, 1.0), vUv.y));
					  vec4 glitchColor = texture2D(tGlitch, vec2(mod(mod(time, 242.0) * 0.03 + vUv.x * 0.1, 1.0), vUv.y));
					  vec4 glitchImage = texture2D(tGlitch, vec2(mod(mod(time, 42.0) * 0.5 + vUv.x * 0.1, 1.0), vUv.y));
					  //vec4 colorBlur = getBlur();
					  vec2 uvDisp = vec2(mod(vUv.x + glitchDisplace.r * 0.01, 1.0), mod(vUv.y + glitchColor.b * 0.01, 1.0));
					  //vec2 uvDiff = uvDisp - vUv;
					  float noiseOffset = cnoise(vec2(vUv.x * 10.0 + time, vUv.y * 10.0)) * glitch_intensity;
					  vec4 colorR = texture2D( tDiffuse, vec2(mod(uvDisp.x + offset_color.x * noiseOffset, 1.0), uvDisp.y) );
					  vec4 colorG = texture2D( tDiffuse, vec2(mod((vUv.x + uvDisp.x) / 2.0 + offset_color.y * noiseOffset, 1.0), uvDisp.y) );
					  vec4 colorB = texture2D( tDiffuse, vec2(mod(vUv.x + offset_color.z * noiseOffset, 1.0), uvDisp.y) );


					  float darkDirtMult = 0.05;
					  colorR = colorR - colorDirt * darkDirtMult;
					  colorG = colorG - colorDirt * darkDirtMult;
					  colorB = colorB - colorDirt * darkDirtMult;

						gl_FragColor = vec4(colorR.r, colorG.g, colorB.b, 1.0);
					  //gl_FragColor = vec4(colorBlur.r, colorBlur.g, colorBlur.b, 1.0);
					  //gl_FragColor = vec4(noiseOffset, noiseOffset, noiseOffset, 1.0);
					  //gl_FragColor = vec4(colorDirt.r, colorDirt.g, colorDirt.b, 1.0);
					  //gl_FragColor = vec4(glitchDisplace.r, glitchDisplace.g, glitchDisplace.b, 1.0);
					}
					//uniform sampler2D tex;			

					//uniform float time ; 
					//uniform float globalSound ;
					//uniform float soundMultiplier ; 

					//uniform float rOffset ; 
					//uniform float gOffset ; 
					//uniform float bOffset ;
					//void main()
					//{
					//	vec2 texPos = gl_TexCoord[0].st;
					//
					//	texPos.x += tan( time ) * globalSound * soundMultiplier ; 
					//	texPos.y += cos( time * cos ( texPos.x ))  * globalSound * soundMultiplier ; 

					//	vec4 source = texture2DRect(tex, texPos );	
					//	//use the color from the image, but use the r channel of the mask as the alpha channel of our output
					//	gl_FragData[0] = vec4( source.r + rOffset , source.g + gOffset , source.b + bOffset , source.a ) ; //vec4(image.rgb,composite.r);  

					//	gl_FragColor = texture2D(tex, newUv);
					//}
				);

				return shdr.c_str();
			}

			void render(ci::Surface surf)
			{
				using namespace ci;
				ci::gl::Texture tex = ci::gl::Texture(surf);
				tex.bind(0);
			/*	shader.bind();			
				shader.uniform("time", 0);	
				shader.uniform("glitch_intensity", 0.1f);		
				shader.uniform("resolution", Vec3i(0,0,0));		
				shader.uniform("offset_color", Vec3f( 0.005, 0.0, 0.0));		
				shader.uniform("tDiffuse", 0);		
				shader.uniform("tLensDirt", 0);		
				shader.uniform("tGlitch", 0);		
				ci::gl::draw(surf);
				shader.unbind();*/
				tex.unbind();
				  //we want to pass in some varrying values to animate our type / color 
   /* glitchShader.setUniform1f("time", ofGetElapsedTimef() );
    glitchShader.setUniform1f("soundMultiplier",  timeline.getKeyframeValue("SOUND MULTIPLIER") );
    glitchShader.setUniform1f("globalSound", smoothedVol ) ; 
    
    glitchShader.setUniform1f("rOffset", timeline.getKeyframeValue("R OFFSET") ) ; 
    glitchShader.setUniform1f("gOffset", timeline.getKeyframeValue("G OFFSET") ) ; 
    glitchShader.setUniform1f("bOffset", timeline.getKeyframeValue("B OFFSET") ) ; */
			}
		};
		typedef std::shared_ptr<Glitch> GlitchRef;
	}
}