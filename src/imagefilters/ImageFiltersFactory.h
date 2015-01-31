#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "BaseShader.h"
#include "Pixelate.h"
#include "HueSaturation.h"
#include "Sepia.h"
#include "Noise.h"
#include "Vibrance.h"
#include "DotScreen.h"
#include "ColorHalftone.h"
#include "Lut.h"
#include "Kaleidoscope.h"
#include "Bloom.h"
#include "Bleach.h"
#include "Brcosa.h"
#include "FishEye.h"
#include "BrightnessContrast.h"
#include "ColorMatrix.h"
#include "Glitch.h"

namespace shaders
{	
	namespace imagefilters
	{
		class ImageFiltersFactory
		{
		public:
			enum FilterType
			{
				PIXELATE,
				HUE_SATURATION,
				SEPIA,
				NOISE,
				VIBRANCE,
				DOT_SCREEN,
				COLOR_HALF_TONE,
				//LUT,
				KALEIDOSCOPE,
				//BLOOM,
				//BLEACH,
				BRCOSA,
				//FISH_EYE,
				BRIGHTNESS_CONTRAST,
				//COLOR_MATRIX,
				//GLITCH,
				_NULL
			};

			ImageFiltersFactory():currentID(PIXELATE){	}

			BaseShaderRef get(FilterType id, ci::params::InterfaceGlRef params)
			{
				using namespace shaders::imagefilters;
				using namespace ci;

				BaseShaderRef shader;
				currentID = id;

				switch(id)
				{
					case PIXELATE:				shader = PixelateRef(new Pixelate(Vec2f(100.0f, 100.0f))); break;
					case HUE_SATURATION:		shader = HueSaturationRef(new HueSaturation(0.5f, 0.5f)); break;
					case SEPIA:					shader = SepiaRef(new Sepia(0.5f)); break;
					case NOISE:					shader = NoiseRef(new Noise(0.5f)); break;
					case VIBRANCE:				shader = VibranceRef(new Vibrance(0.5f)); break;
					case DOT_SCREEN:			shader = DotScreenRef(new DotScreen(Vec2i(100, 100), 1, 1)); break;
					case COLOR_HALF_TONE:		shader = ColorHalftoneRef(new ColorHalftone(Vec2i(100, 100), 1, 1)); break;
					//case LUT:					shader = LutRef(new Lut(0.3f)); break;
					case KALEIDOSCOPE:			shader = KaleidoscopeRef(new Kaleidoscope(2.0f)); break;
					//case BLOOM:				shader = BloomRef(new Bloom(0.4f)); break;
					//case BLEACH:				shader = BleachRef(new Bleach(0.5f)); break;
					case BRCOSA:				shader = BrcosaRef(new Brcosa(0.5f, 0.5f, 0.5f, 1.0f)); break;
					//case FISH_EYE:			shader = FishEyeRef(new FishEye(0.0f, 0.0f)); break;						
					//case SWIRL:				shader = ; break;						
					case BRIGHTNESS_CONTRAST:	shader = BrightnessContrastRef(new BrightnessContrast(0.6f, 0.6f)); break;						
					//case GLITCH:				shader = GlitchRef(new Glitch()); break;						
					//case COLOR_MATRIX:		shader = ColorMatrixRef(new ColorMatrix()); break;						
				}

				shader->createParams(params);

				return shader;
			}

			BaseShaderRef getNext(ci::params::InterfaceGlRef params)
			{
				int cur = currentID;
				if (++cur == _NULL)	cur = 0;

				currentID = static_cast<FilterType>(cur);
				return get(currentID, params);
			}

			BaseShaderRef getPrev(ci::params::InterfaceGlRef params)
			{
				int cur = currentID;
				if (--cur == -1) cur = _NULL - 1;

				currentID = static_cast<FilterType>(cur);
				return get(currentID, params);
			}

		private:
			FilterType currentID;
		};
	}
}