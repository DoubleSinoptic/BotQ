#include "GuiInstance.h"
#include "../Common/TinyFile.h"
#include "Gl3dDevice.h"
#include "Gl3dRenderPas.h"
#include "../Math/Matrix4.h"
#include "Display.h"
#include "Input.h"
#include "Graphics/Bitmap.h"
#include "../Graphics/Font.h"

#include "Box.h"
#include "ImageCanvas.h"
#include "Button.h"
#include "Label.h"
#include "CheckBox.h"
#include "HScrollBox.h"
#include "HSlider.h"
#include "../Time.hpp"


GuiInstance::GuiInstance()
{


}


GuiInstance::~GuiInstance()
{
}

void GuiInstance::InstallGui()
{
	host->ClearControls();
	Ref<Box> c = New<Box>();
	c->SetPosition({10, 10});	
	c->SetAnchor(Anchor::Top | Anchor::Left);
	c->SetSize({ 200, 300 });
	this->host->AddControl(c);
	host->AddControl(c);

	{
		Ref<Button> a = New<Button>();
		a->SetAnchor(Anchor::Top);
	
		a->SetPosition({ 10, 10 });
		a->SetSize({ 80, 22 });
		a->SetText(u8"Play engine!");
		c->AddControl(a);
	}
	{
		Ref<Button> a = New<Button>();
		a->SetAnchor(Anchor::Top);

		a->SetPosition({ 10, 42 });
		a->SetSize({ 80, 22 });
		a->SetText(u8"Reload shaders");
		a->OnClikedEvent = []() 
		{
			Resource* r =Resource::Find("/Functions/ReloadShaders", false);
			if (r) r->GetObject<StorageFunction<void(void)>>()();
		};
		c->AddControl(a);
	}
	{
		Ref<Button> a = New<Button>();
		a->SetAnchor(Anchor::Top);
		
		a->SetPosition({ 10, 42 + 32 });
		a->SetSize({ 80, 22 });
		a->SetText(u8"Close game");
		a->OnClikedEvent = []()
		{
			Resource* r = Resource::Find("/Functions/CloseGame", false);
			if (r) r->GetObject<StorageFunction<void(void)>>()();
		};
		c->AddControl(a);
	}

	//Ref<CheckBox> b = New<CheckBox>();
	//b->SetPosition({10, 80});
	//b->SetSize({20, 15});
	//c->AddControl(b);

	Ref<HScrollBox> xxx = New<HScrollBox>();
	xxx->SetPosition({5, 200 - 90 - 5 });
	xxx->SetSize({190, 90});
	xxx->SetAnchor(Anchor::Left | Anchor::Right | Anchor::Top | Anchor::Bottom);
	c->AddControl(xxx);


	Ref<Button> h= New<Button>();

	h->SetPosition({ 10, 5 });
	h->SetSize({ 20, 10 });
	h->SetText("hello world");
	xxx->GetItems()->AddControl(h);
	/*
	new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = true; }), "/Functions/EnableOutPostProcTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutChromaticAberationTexel = true; }), "/Functions/EnableChromaticAberationTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = true; }), "/Functions/EnablePostProcTexel");
	*/
	int i;
	for (i = 33; i < 360; i+=20) 
	{
		Ref<CheckBox> f = New<CheckBox>();
		f->SetPosition({ 10, i + 4 });
		f->SetSize({ 18, 18 });
		xxx->GetItems()->AddControl(f);
		
		Ref<Label> fl = New<Label>();
		
		fl->EnableAutoSize(false);
		fl->SetText(String::Format(u8"включение чегото: %d", i));
		fl->SetPosition({ 10 + 18 + 3, i + 4 });
		fl->SetSize({ 218, 24 });
		xxx->GetItems()->AddControl(fl);
	}

	{
		Ref<CheckBox> f = New<CheckBox>();
		f->SetPosition({ 10, i + 4 });
		f->SetSize({ 18, 18 });
		xxx->GetItems()->AddControl(f);
		f->SetCheked(true);
		f->OnCheckChangedEvent = [](bool a)
		{
			if (a)
			{
				Resource* r = Resource::Find("/Functions/EnablePostProcTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}
			else
			{
				Resource* r = Resource::Find("/Functions/DisablePostProcTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}
		};
		Ref<Label> fl = New<Label>();
	
		fl->EnableAutoSize(false);
		fl->SetText(String::Format(u8"PostProcTexel: %d", i));
		fl->SetPosition({ 10 + 18 + 3, i + 4 });
		fl->SetSize({ 218, 24 });
		xxx->GetItems()->AddControl(fl);
		i += 20;
	}
	{
		Ref<CheckBox> f = New<CheckBox>();
		f->SetPosition({ 10, i + 4 });
		f->SetSize({ 18, 18 });
		xxx->GetItems()->AddControl(f);
		f->OnCheckChangedEvent = [](bool a)
		{
			if (a)
			{
				Resource* r = Resource::Find("/Functions/EnableChromaticAberationTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}
			else
			{
				Resource* r = Resource::Find("/Functions/DisableChromaticAberationTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}
		};
		Ref<Label> fl = New<Label>();
		
		fl->EnableAutoSize(false);
		fl->SetText(String::Format(u8"ChromaticAberationTexel: %d", i));
		fl->SetPosition({ 10 + 18 + 3, i + 4 });
		fl->SetSize({ 218, 24 });
		xxx->GetItems()->AddControl(fl);
		i += 20;
	}

	{
		Ref<CheckBox> f = New<CheckBox>();
		f->SetPosition({ 10, i + 4 });
		f->SetSize({ 18, 18 });
		xxx->GetItems()->AddControl(f);
		f->OnCheckChangedEvent = [](bool a) 
		{
			if (a) 
			{
				Resource* r = Resource::Find("/Functions/EnableOutPostProcTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}
			else 
			{
				Resource* r = Resource::Find("/Functions/DisableOutPostProcTexel", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			}			
		};
		Ref<Label> fl = New<Label>();
	
		fl->EnableAutoSize(false);
		fl->SetText(String::Format(u8"OutPostProcTexel: %d", i));
		fl->SetPosition({ 10 + 18 + 3, i + 4 });
		fl->SetSize({ 218, 24 });
		xxx->GetItems()->AddControl(fl);
		i += 20;
	}


	Ref<HSlider> sl = New<HSlider>();
	sl->SetPosition({10, 210});
	sl->SetSize({150, 13});
	c->AddControl(sl);
}

void SDASDASExportShader(Gl3dShader* tlg, const String& filename)
{
	FileStream f(filename.c_str(), OpenMode::Read);
	String content = f.AllContent();
	String vertex, fragment;
	int i = 0;
	for (; i < content.Length(); i++)
	{
		if (content[i] == '&')
			break;
		vertex += content[i];
	}
	i++;
	for (; i < content.Length(); i++)
		fragment += content[i];

	tlg->Build(vertex.c_str(), ShaderType::VertexShader);
	tlg->Build(fragment.c_str(), ShaderType::FragmentShader);
	tlg->Relink();
}

//
//layout(location = 0) in vec2 positon;
//layout(location = 1) in vec2 texcoord;
//layout(location = 2) in vec4 color;
void GuiInstance::DrawGpu()
{
	if (!shader)
	{
		shader = new Gl3dShader();
		SDASDASExportShader(shader.GetPtr(), "./Shaders/Gui.glsl");

		rgbSplit = new Gl3dShader();
		SDASDASExportShader(rgbSplit.GetPtr(), "./Shaders/RgbSplit.glsl");
		guiVertexArray = new Gl3dVertexArrayBase();
		guiVertexes = new Gl3dArray<GuiVertex>(Gl3dArrayTarget::Array);
		guiVertexArray->Attach(0, { Gl3dDataSplitType::Float2, Gl3dDataSplitType::Float2 , Gl3dDataSplitType::Float4 }, guiVertexes.GetPtr());

	}

	Size sz = Display::GetCurrent()->GetSize();
	Gl3dDevice::Viewport(sz.width, sz.height);
	{
		Gl3dDevice::CullTest(false);
		Gl3dDevice::DepthTest(false);
		Gl3dDevice::AlphaTest(true);
		Gl3dRenderPas pass(shader.GetPtr(), nullptr);
		//Gl3dDevice::Clear();
		const Matrix4 ortho = Matrix4::Ortho(0.0f, sz.width, sz.height, 0.0f, -1.0, 1.0);
		pass.Uniform("ortho", ortho);
		pass.Uniform("time", (float)Time::GetTotalTime());
		pass.Uniform("useEffectA", useEffectA ? 1 : 0);
		pass.Uniform("useEffectB", useEffectB ? 1 : 0);
		GuiInstance::CursorState state{};
		Point g = Input::GetMausePosition();
		state.x = g.x;
		state.y = g.y;
		state.buttonLeft = Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_LEFT);
		state.buttonMidleAxel = 0;
		state.buttonRight = Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_RIGHT);

		if (!host)
		{
			host = new Control();
			host->SetSize({ sz.width, sz.height });
			InstallGui();
		}
		host->SetSize({ sz.width, sz.height });
		AllProcess(sz.width, sz.height, state);

		Draw(sz.width, sz.height);

		auto& faces = renderData.faces;
		auto& clips = renderData.clips;
		auto& lines = renderData.lines;
		auto& quads = renderData.quads;

		Gl3dTexture* current = nullptr;

		for (auto& x : quads)
		{
			if (x.texture != nullptr)
			{
				if (current != static_cast<Gl3dTexture*>(x.texture->GetTextureObject()))
				{
					current = static_cast<Gl3dTexture*>(x.texture->GetTextureObject());
					pass.Uniform("map", current);
					pass.Uniform("mapEnabled", true);
				}
				
			}
			else
				pass.Uniform("mapEnabled", false);


			pass.Uniform("clipLU", Vector2(clips[x.cullid].left, clips[x.cullid].top));
			pass.Uniform("clipRB", Vector2(clips[x.cullid].right, clips[x.cullid].bottom));

			guiVertexes->Clear();
			guiVertexes->Add({ x.p2,	x.tc2,	x.c2.ToVector4() });
			guiVertexes->Add({ x.p1,	x.tc1,	x.c1.ToVector4() });
			guiVertexes->Add({ x.p0,	x.tc0,	x.c0.ToVector4() });

			guiVertexes->Add({ x.p2,	x.tc2,	x.c2.ToVector4() });
			guiVertexes->Add({ x.p0,	x.tc0,	x.c0.ToVector4() });
			guiVertexes->Add({ x.p3,	x.tc3,	x.c3.ToVector4() });
			guiVertexArray->Draw(Gl3dDrawPrimitive::Triangles, 6);
		}
		for (auto& x : lines)
		{
			pass.Uniform("mapEnabled", false);

			pass.Uniform("clipLU", Vector2(clips[x.cullid].left, clips[x.cullid].top));
			pass.Uniform("clipRB", Vector2(clips[x.cullid].right, clips[x.cullid].bottom));

			guiVertexes->Clear();
			guiVertexes->Add({ x.p0,	Vector2(),	x.c0.ToVector4() });
			guiVertexes->Add({ x.p1,	Vector2(),	x.c1.ToVector4() });
			guiVertexArray->Draw(Gl3dDrawPrimitive::Lines, 2);
		}

		Gl3dDevice::AlphaTest(false);
	}


}
void GuiInstance::ProcessMaxium(int depthId, const Ref<Control>& testControl, const Point& parentGloblPosition, const RectangleF & parentClip, const RectangleF & mousePoint)
{
	testControl->OnProcessUpdate({ mousePoint.left, mousePoint.top });
	depthId++;
	Point mePosition = parentGloblPosition + testControl->GetPosition();
	RectangleF meClip = parentClip * RectangleF(mePosition, testControl->GetSize());
	if (!meClip.IsEmpty())
	{
		if (!(meClip * mousePoint).IsEmpty())
		{
			if (maxId <= depthId)
			{
				maxId = depthId;
				max = testControl;
			}
		}

		for (int i = 0; i < testControl->ControlsCount(); i++)
		{
			Ref<Control>& childControlOfRenderControl = testControl->ControlAt(i);
			if (childControlOfRenderControl->IsHandled())
				ProcessMaxium(depthId, childControlOfRenderControl, mePosition, meClip, mousePoint);
		}
	}
}
void GuiInstance::AllProcess(int displayW, int displayH, const CursorState & state)
{
	Point cursorPos = Point(state.x - 1, state.y - 1);
	RectangleF mousePoint = RectangleF(cursorPos, Size(1, 1));
	maxId = 0;
	max = Ref<Control>();
	ProcessMaxium(0,
		host,
		Point(0, 0),
		RectangleF(Point(0, 0), Size(displayW, displayH)),
		mousePoint);

	if (max != current)
	{
		if (!current.IsNull())
			current->OnCursorLeave(cursorPos);
		if (!max.IsNull())
			max->OnCursorEnter(cursorPos);		
		current = max;
	}
	else
	{
		if (!current.IsNull())
			current->OnCursorMove(cursorPos);

	}

	//if (!state.buttonLeft && !state.buttonRight)
	//{
	//	HScrollBox* currP = dynamic_cast<HScrollBox*>(current.GetPtr());
	//	if (currP)
	//	{
	//		
	//		currP->Scroll(state.buttonMidleAxel);
	//		

	//	}
	//	
	//}

	if (state.buttonLeft)
	{
		if (!leftPressed)
		{
			if (!current.IsNull())
			{
				if (!controlInHand.IsNull())
					controlInHand->OnCursorEndDown(cursorPos);
				current->OnCursorStartDown(cursorPos);
				controlInHand = current;
			}

			leftPressed = true;
		}
	}
	else
	{
		if (leftPressed)
		{
			if (!controlInHand.IsNull())
			{
				controlInHand->OnCursorEndDown(cursorPos);
				controlInHand = Ref<Control>();
			}

			leftPressed = false;
		}

	}

}
void GuiInstance::ProcessDraw(GuiRenderData* renderData, const Point& globParentlPosition, const Ref<Control>& renderControl, const RectangleF& parentClip)
{

	Point mePosition = globParentlPosition + renderControl->GetPosition();
	RectangleF clipForControl = RectangleF(mePosition, renderControl->GetSize()) * parentClip;
	if (clipForControl.IsEmpty())
		return;
	renderControl->Draw(renderData, mePosition, clipForControl);
	for (int i = 0; i < renderControl->ControlsCount(); i++)
	{
		Ref<Control>& childControlOfRenderControl = renderControl->ControlAt(i);
		ProcessDraw(renderData, mePosition, childControlOfRenderControl, clipForControl);
	}
}
void GuiInstance::Draw(int displayW, int displayH)
{
	renderData.faces.Clear();
	renderData.lines.Clear();
	renderData.clips.Clear();
	renderData.quads.Clear();
	ProcessDraw(&renderData, Point(0, 0), host, RectangleF(Point(0, 0), Size(displayW, displayH)));
}
