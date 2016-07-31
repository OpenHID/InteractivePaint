#include "Layer.h"
#include <cinder/gl/draw.h>
#include <cinder/gl/scoped.h>

namespace touchpoints { namespace drawing
{
	Layer::Layer() {}

	Layer::Layer(int windowWidth, int windowHeight) 
		: windowWidth(windowWidth), windowHeight(windowHeight)
	{
		framebuffer = gl::Fbo::create(windowWidth, windowHeight, format);
	}

	void Layer::SetWindowWidth(int width)
	{
		windowWidth = width;
		resetFramebuffer();
	}

	void Layer::SetWindowHeight(int height)
	{
		windowHeight = height;
		resetFramebuffer();
	}

	void Layer::SetWindowDimensions(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		resetFramebuffer();
	}

	void Layer::resetFramebuffer()
	{
		framebuffer.reset();
		
		framebuffer = gl::Fbo::create(windowWidth, windowHeight, format);
		framebuffer->bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		framebuffer->unbindFramebuffer();
	}

	void Layer::AddDrawable(shared_ptr<IDrawable> drawable)
	{
		auto guid = guidGenerator.newGuid();
		drawablesStack.push_back(make_pair(drawable, guid));
	}

	void Layer::AddSymmetricDrawables(pair<shared_ptr<IDrawable>, shared_ptr<IDrawable>> symmetricDrawables)
	{
		auto guid = guidGenerator.newGuid();
		drawablesStack.push_back(make_pair(symmetricDrawables.first, guid));
		drawablesStack.push_back(make_pair(symmetricDrawables.second, guid));
	}

	void Layer::Draw()
	{
		drawSetup();

		for(auto drawable: drawablesStack)
		{
			drawable.first->Draw();
		}

		drawTearDown();
	}

	void Layer::drawSetup() const
	{
		framebuffer->bindFramebuffer();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}

	void Layer::drawTearDown() const
	{
		framebuffer->unbindFramebuffer();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		gl::color(1.0, 1.0, 1.0, alpha.GetValue());
		gl::draw(framebuffer->getColorTexture());
	}

	void Layer::Undo()
	{
		if (drawablesStack.size() > 0)
		{
			auto firstGuid = drawablesStack.back().second;
			drawablesStack.pop_back();
			if(drawablesStack.size() > 0)
			{
				auto secondGuid = drawablesStack.back().second;
				if (firstGuid == secondGuid)
				{
					//only remove this shape if it is a symmetric pair of the last shape
					drawablesStack.pop_back();
				}
			}
		}
	}

	void Layer::ClearLayer()
	{
		drawablesStack.clear();
		resetFramebuffer();
	}
}}
