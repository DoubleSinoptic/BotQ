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

static Gl3dWrapMode addModes[] =
{
	Gl3dWrapMode::Repeat,
	Gl3dWrapMode::MirroredRepeat,
	Gl3dWrapMode::ClampToEdge,
	Gl3dWrapMode::ClampToBorder
};

static Gl3dFilter filterModes[]
{
	Gl3dFilter::NEAREST,
	Gl3dFilter::LINEAR,
	Gl3dFilter::MIPMAP_NEAREST,
	Gl3dFilter::MIPMAP_LINEAR
};

void Texture::SetSampler(const SamplerStateDesc & state)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunction([=]()
	{
		Gl3dTexture* c = handle.Get<Gl3dTexture*>();
		c->SetMagMinFilters(filterModes[(size_t)state.mag], filterModes[(size_t)state.min]);
		c->SetWrapMode(addModes[(size_t)state.addressModeU], addModes[(size_t)state.addressModeV], addModes[(size_t)state.addressModeW]);
	});
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

		Gl3dSubImageDesc desc;
		desc.data = bitmap->GetBits();
		desc.type = Gl3dFundamentalType::UByte;
		desc.changelsCount = 4;
		desc.width = bitmap->GetWidth();
		desc.height = bitmap->GetHeight();

		gt->SetData(Gl3dPixelFormat::RGBA_8, &desc);
	
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
	
		gt->SetData(Gl3dSide::PosX, Gl3dPixelFormat::RGBA_8, left->FormatDesc());
		gt->SetData(Gl3dSide(int(Gl3dSide::PosX) + 1), Gl3dPixelFormat::RGBA_8, right->FormatDesc());
		gt->SetData(Gl3dSide(int(Gl3dSide::PosX) + 2), Gl3dPixelFormat::RGBA_8, top->FormatDesc());
		gt->SetData(Gl3dSide(int(Gl3dSide::PosX) + 3), Gl3dPixelFormat::RGBA_8, bottom->FormatDesc());
		gt->SetData(Gl3dSide(int(Gl3dSide::PosX) + 4), Gl3dPixelFormat::RGBA_8, back->FormatDesc());
		gt->SetData(Gl3dSide(int(Gl3dSide::PosX) + 5), Gl3dPixelFormat::RGBA_8, front->FormatDesc());
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



void Texture::GenMipmaps()
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([=]()
	{
		handle.Get<Gl3dTexture*>()->GenMipmaps();
	});
}
