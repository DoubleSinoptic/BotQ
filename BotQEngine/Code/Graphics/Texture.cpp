#include "Texture.h"
#include "../GameInstance.h"
#include "Gl3dTexture.h"
#include "GameInstance.h"

#define TRANSLATE_MESSAGE_BEGIN_SAFE auto __translator = [&](){ 
#define TRANSLATE_MESSAGE_END_SAFE }; if(bindendId != std::this_thread::get_id()) renderSyncRoot.SigTranslate(__translator); else __translator();

#define TRANSLATE_MESSAGE_QUEUE this->renderSyncRoot.Translate(nullptr, [=]() { 
#define TRANSLATE_MESSAGE_BEGIN this->renderSyncRoot.SigTranslate([&]() { 
#define TRANSLATE_MESSAGE_END });

Texture::~Texture()
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		delete handle.Get<Gl3dTexture*>();
	});
	
}

Texture::Texture() 
	: isCubeMap(true)
{

}

void* Texture::GetTextureObject() const
{
	return handle.Get<void*>();
}

void Texture::SetFromBitmap(const Ref<Bitmap>& bitmap)
{
	Assert(!bitmap.IsNull());
	w = bitmap->GetWidth();
	h = bitmap->GetHeight();
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		Gl3dTexture* gt = new Gl3dTexture();
		handle.Set(gt);
		gt->SetData(bitmap->GetWidth(), bitmap->GetHeight(), PixelFormat::RGBA_8, bitmap->GetBits());
		/*gt->SetMagMinFilters(Filter::LINEAR, Filter::LINEAR);*/
		gt->SetMagMinFilters(Filter::NEAREST, Filter::MIPMAP_LINEAR);
		gt->GenMipmaps();
	});
	isCubeMap = false;
}

void Texture::SetFromCubeBitmap(const Ref<Bitmap>& left, const Ref<Bitmap>& right, const Ref<Bitmap>& top, const Ref<Bitmap>& bottom, const Ref<Bitmap>& back, const Ref<Bitmap>& front)
{
	Assert(!left.IsNull());
	Assert(!right.IsNull());
	Assert(!top.IsNull());
	Assert(!bottom.IsNull());
	Assert(!back.IsNull());
	Assert(!front.IsNull());
	w = left->GetWidth();
	h = left->GetHeight();
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		Gl3dTexture* gt = new Gl3dTexture();
		handle.Set(gt);
		gt->SetDataCube(0, left->GetWidth(), left->GetHeight(), PixelFormat::RGBA_8, left->GetBits());
		gt->SetDataCube(1, right->GetWidth(), right->GetHeight(), PixelFormat::RGBA_8, right->GetBits());
		gt->SetDataCube(2, top->GetWidth(), top->GetHeight(), PixelFormat::RGBA_8, top->GetBits());
		gt->SetDataCube(3, bottom->GetWidth(), bottom->GetHeight(), PixelFormat::RGBA_8, bottom->GetBits());
		gt->SetDataCube(4, back->GetWidth(), back->GetHeight(), PixelFormat::RGBA_8, back->GetBits());
		gt->SetDataCube(5, front->GetWidth(), front->GetHeight(), PixelFormat::RGBA_8, front->GetBits());
	});

}

int Texture::GetWidth() const
{
	return w;
}

int Texture::GetHeight() const
{
	return h;
}

void Texture::EnableFiltration(bool value)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([=]()
	{
		if(value)
			handle.Get<Gl3dTexture*>()->SetMagMinFilters(Filter::NEAREST, Filter::LINEAR);
		else
			handle.Get<Gl3dTexture*>()->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
	});
}

void Texture::GenMipmaps()
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([=]()
	{
		handle.Get<Gl3dTexture*>()->GenMipmaps();
	});
}
