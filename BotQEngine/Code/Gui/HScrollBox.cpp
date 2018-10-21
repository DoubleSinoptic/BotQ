#include "HScrollBox.h"
#define inline
 Pottor::Pottor(Control * ths) :
	currsrStartLoc(0, 0), ct(ths),
	lastLoc(0, 0)
{
	thump = Resource::Find("/Skin/vertical scrollbar thumb.png")->GetObject<Ref<Texture>>();

}

 inline float Pottor::GetScrollTaping() const
 {
	 if (GetPosition().y == 0)
		 return 0.0;
	 return float(GetPosition().y) / float(GetParent()->GetSize().height);
 }

 void Pottor::OnProcessUpdate(const Point& cursorGLoblPosition)
{
	if (downed)
	{
		float newY = lastLoc.y + (cursorGLoblPosition - currsrStartLoc).y;
		this->SetPosition(Point(2, Mathf::Clamp(newY, 0, GetParent()->GetSize().height - GetSize().height)));
	}
}

 inline void Pottor::OnCursorStartDown(const Point & cursorGLoblPosition)
 {
	 Control::OnCursorStartDown(cursorGLoblPosition);
	 downed = true;
	 currsrStartLoc = cursorGLoblPosition;
	 lastLoc = GetPosition();
 }

 inline void Pottor::OnCursorEndDown(const Point & cursorGLoblPosition)
 {

	 Control::OnCursorEndDown(cursorGLoblPosition);
	 currsrStartLoc = { 0, 0 };
	 downed = false;
 }

 void Pottor::ScrollDelta(int ds)
 {
	 int newY = GetPosition().y + ds;
	 SetPosition(Point(2, Mathf::Clamp(newY, 0, GetParent()->GetSize().height - GetSize().height)));
 }

 inline void Pottor::Draw(GuiRenderData * drawData, const Point & globlPosition, const RectangleF & renderField)
 {
	 drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 6, 6, Color(255, 255, 255, 255), thump.GetPtr());
 }

 inline HScrollBox::HScrollBox() :
	 mScrollButton(new Pottor(this)),
	 mRight(new RightSwam()),
	 mItems(new Control())
 {
	 AddControl(mItems);
	 AddControl(mRight);
	 mRight->SetSize({0, 9});
	 mProxy = New<Control>();
	 mRight->AddControl(mProxy);
	 mProxy->SetPosition({0, 3});
	 mProxy->SetSize({0, 3});
	 mProxy->SetAnchor(Anchor::Left | Anchor::Right | Anchor::Bottom | Anchor::Top);
	 mProxy->AddControl(mScrollButton);
	 mScrollButton->SetSize(Size(11, 10));
	 mScrollButton->SetPosition({ 2, 0 });
 }

 inline const Ref<Control>& HScrollBox::GetItems() const
 {
	 return mItems;
 }

 void HScrollBox::Scroll(int ds)
 {
	 mScrollButton->ScrollDelta(ds);
 }

 inline float HScrollBox::Hermite()
 {
	 float ascpet = 1.0;
	 float totalLength = MaximusY();
	 float thisLength = GetSize().height;
	 if (totalLength >= thisLength)
		 ascpet = thisLength / totalLength;
	 mScrollButton->SetSize(Size(11, mProxy->GetSize().height * ascpet));
	 return totalLength;
 }

 inline float HScrollBox::MaximusY()
 {
	 float max = 0;
	 for (int i = 0; i < mItems->ControlsCount(); i++)
	 {
		 const Ref<Control>& ct = mItems->ControlAt(i);
		 if (max < ct->GetPosition().y + ct->GetSize().height)
			 max = ct->GetPosition().y + ct->GetSize().height;
	 }
	 return max;
 }

 inline void HScrollBox::ComuteState()
 {
	 Size sz = GetSize();
	 mRight->SetPosition(Point(sz.width - 15.0, 0));
	 mRight->SetSize(Size(15, sz.height));
	 mItems->SetSize(Size(sz.width - 16, 1e+6));
	 float pages = Hermite();;

	 mItems->SetPosition(Point(0, int(pages * -mScrollButton->GetScrollTaping())));
 }

 inline void HScrollBox::OnProcessUpdate(const Point & cursorGLoblPosition)
 {

	 ComuteState();
 }

 inline RightSwam::RightSwam()
 {
	 thump=  Resource::Find("/Skin/vertical scrollbar.png")->GetObject<Ref<Texture>>();

 }

 inline void RightSwam::Draw(GuiRenderData * drawData, const Point & globlPosition, const RectangleF & renderField)
 {
	 drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 7, 7, 7, 7, Color(255, 255, 255, 255), thump.GetPtr());
 }
