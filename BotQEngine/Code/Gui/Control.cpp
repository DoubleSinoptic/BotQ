#include "Control.h"


void Control::ResizeQuery(int dx, int dy)
{
#define FLAG_TEST(y) ((((unsigned)mAnhor) & ((unsigned)y)) == ((unsigned)y))
	if (FLAG_TEST(Anchor::Right) && !FLAG_TEST(Anchor::Left))
		mPosition.x += dx;
	if (FLAG_TEST(Anchor::Right) && FLAG_TEST(Anchor::Left))
		SetSize(mSize + Size(dx, 0));

	if (FLAG_TEST(Anchor::Bottom) && !FLAG_TEST(Anchor::Top))
		mPosition.y += dy;
	if (FLAG_TEST(Anchor::Bottom) && FLAG_TEST(Anchor::Top))
		SetSize(mSize + Size(0, dy));

	if (!FLAG_TEST(Anchor::Left) && !FLAG_TEST(Anchor::Right))
		mPosition.x = (mParent->mSize.width / 2) - (mSize.width / 2);
	if (!FLAG_TEST(Anchor::Bottom) && !FLAG_TEST(Anchor::Top))
		mPosition.y = (mParent->mSize.height / 2) - (mSize.height / 2);
}

Control::~Control() {}

void Control::ClearControls()
{
	mControls.Clear();
}

void Control::OnProcessUpdate(const Point & cursorGLoblPosition)
{}

bool Control::IsHandled() const
{
	return mHandle;
}

void Control::EnabledProcess(bool val)
{
	mHandle = val;
}

 void Control::SetAnchor(Anchor anch)
{
	mAnhor = anch;
}

 void Control::OnCursorEnter(const Point & cursorGLoblPosition)
{
	IsMouseHovered = true;
	Log("OnCursorEnter as %s", *mText);
}

 void Control::OnCursorLeave(const Point & cursorGLoblPosition)
{
	IsMouseHovered = false;
	Log("OnCursorLeave as %s", *mText);
}

 void Control::OnCursorStartDown(const Point & cursorGLoblPosition)
{
	Log("OnCursorStartDown as %s", *mText);
}

 void Control::OnCursorEndDown(const Point & cursorGLoblPosition)
{
	Log("OnCursorEndDown as %s", *mText);
}

 void Control::OnCursorMove(const Point & cursorGLoblPosition)
{
	//Log("OnCursorMove as %s", *mText);
}

 void Control::SetTexutre(const Ref<Texture>& tex)
{
	mTexture = tex;
}

 void Control::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField)
{


}

 size_t Control::ControlsCount() const
{
	return mControls.Length();
}

 Ref<Control>& Control::ControlAt(int index)
{
	return mControls[index];
}

 const Ref<Control>& Control::ControlAt(int index) const
{
	return mControls[index];
}

 void Control::AddControl(const Ref<Control>& control)
{
	mControls.Add(control);
	if (control->mParent != nullptr)
		control->mParent->RemoveControl(control);
	control->mParent = this;
	control->ResizeQuery(0, 0);
}

 void Control::RemoveControl(const Ref<Control>& control)
{
	control->mParent = nullptr;
	mControls.Remove(control);
}

 Size Control::GetSize() const
{
	return mSize;
}

 void Control::SetSize(const Size & s)
{
	if (s == mSize)
		return;
	int dx = s.width - mSize.width;
	int dy = s.height - mSize.height;
	mSize = s;
	for (int i = 0; i < mControls.Length(); i++)
		mControls[i]->ResizeQuery(dx, dy);
}

 void Control::SetText(const String & s)
{
	mText = s;
}

 void Control::SetPosition(const Point & s)
{
	mPosition = s;
}

 const String & Control::GetText() const
{
	return mText;
}

 Point Control::GetPosition() const
{
	return mPosition;
}

 void Control::SetZIndex(int index)
{
	mZIndex = index;
}

 Control * Control::GetParent() const
{
	return mParent;
}

 int Control::GetZIndex() const
{
	return mZIndex;
}
