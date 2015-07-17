#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "imagefilters/shaders/BaseShader.h"
#include "imagefilters/ImageFiltersFactory.h"

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

	void fileDrop(FileDropEvent event);
};

void ImageFiltersApp::setup()
{
	setWindowSize(1200, 650);
	try {
		texture = gl::Texture(loadImage(loadAsset("fc.jpg")));
	}
	catch (...)
	{

	}
	

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

void ImageFiltersApp::fileDrop(FileDropEvent event)
{
	try {
		gl::TextureRef	mTexture = gl::Texture::create(loadImage(event.getFile(0)));
		texture = *mTexture.get();
	}
	catch (...) {
		console() << "unable to load the texture file!" << std::endl;
	};
}

void ImageFiltersApp::draw()
{
	gl::clear(Color::white()); 
	if(texture)
	shader->render(Surface(texture));
	params->draw();
}

CINDER_APP_NATIVE( ImageFiltersApp, RendererGl )