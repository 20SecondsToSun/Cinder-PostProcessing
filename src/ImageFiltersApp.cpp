#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "BaseShader.h"
#include "ImageFiltersFactory.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace shaders::imagefilters;

class ImageFiltersApp : public AppNative
{
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	void clickedNext();
	void clickedPrev();

	void addButtons();

	gl::Texture texture;
	params::InterfaceGlRef params;

	ImageFiltersFactory filtersFactory;
	BaseShaderRef shader;
};

void ImageFiltersApp::setup()
{
	setWindowSize(1200, 650);
	texture = gl::Texture(loadImage(loadAsset("fc.jpg")));

	params = params::InterfaceGl::create(app::getWindow(), "Params", app::toPixels(ci::Vec2i(300, 400)));	
	shader = filtersFactory.get(ImageFiltersFactory::NOISE, params);
	addButtons();
}

void ImageFiltersApp::addButtons()
{
	params->addSeparator();
	params->addButton("NextFilter", std::bind( &ImageFiltersApp::clickedNext, this ));
	params->addButton("PrevFilter", std::bind( &ImageFiltersApp::clickedPrev, this ));
	params->addText(shader->getTitle());
}

void ImageFiltersApp::clickedNext()
{
	shader = filtersFactory.getNext(params);
	addButtons();
}

void ImageFiltersApp::clickedPrev()
{
	shader = filtersFactory.getPrev(params);
	addButtons();
}

void ImageFiltersApp::mouseDown( MouseEvent event )
{
}

void ImageFiltersApp::update()
{
	
}

void ImageFiltersApp::draw()
{
	gl::clear(Color::white()); 
	
	shader->render(Surface(texture));
	params->draw();
}

CINDER_APP_NATIVE( ImageFiltersApp, RendererGl )