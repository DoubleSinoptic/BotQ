#ifndef CONTROL_H
#define CONTROL_H

#include "GuiRenderData.h"
#include "Resource.h"

enum class Anchor
{
	Left = (1 << 0),
	Right = (1 << 1),
	Top = (1 << 2),
	Bottom = (1 << 3),
	None = 0
};

inline Anchor operator |(Anchor a, Anchor b)
{
	return Anchor(unsigned(a) | unsigned(b));
}

class SGE_EXPORT Control
{
	String	mText;
	Point	mPosition;
	Size	mSize;
	DynamicArray<Ref<Control>> mControls;
	Control* mParent;
	int mZIndex;
	Ref<Texture> mTexture;
	Anchor mAnhor;
	bool mHandle;

	void ResizeQuery(int dx, int dy);

public:
	virtual ~Control();
	Control()
		: mParent(nullptr), mPosition(0, 0), mSize(0, 0), mZIndex(0), mAnhor(Anchor::Left | Anchor::Top), mHandle(true)
	{}

	virtual void OnProcessUpdate(const Point& cursorGLoblPosition);

	bool IsHandled() const;

	void EnabledProcess(bool val);

	void SetAnchor(Anchor anch);

	bool IsMouseHovered = false;

	virtual void OnCursorEnter(const Point& cursorGLoblPosition);

	virtual void OnCursorLeave(const Point& cursorGLoblPosition);

	virtual void OnCursorStartDown(const Point& cursorGLoblPosition);

	virtual void OnCursorEndDown(const Point& cursorGLoblPosition);

	virtual void OnCursorMove(const Point& cursorGLoblPosition);

	void SetTexutre(const Ref<Texture>& tex);

	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& clip);

	size_t ControlsCount() const;

	Ref<Control>& ControlAt(int index);

	void ClearControls();

	const Ref<Control>& ControlAt(int index) const;

	void AddControl(const Ref<Control>& control);

	void RemoveControl(const Ref<Control>& control);

	Size GetSize() const;

	void SetSize(const Size& s);

	virtual void SetText(const String& s);

	virtual void SetPosition(const Point& s);

	const String& GetText() const;

	Point GetPosition() const;

	void SetZIndex(int index);

	Control* GetParent() const;

	int GetZIndex() const;
};

#endif