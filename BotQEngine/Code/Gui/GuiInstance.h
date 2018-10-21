#ifndef GUIINSTNACE_H
#define GUIINSTNACE_H

#include "Control.h"
#include "Gl3dShader.h"
#include "Gl3dVertexArrayBase.h"
#include "Gl3dFrameBuffer.h"
#include "Gl3dArray.h"
class SGE_EXPORT GuiInstance
{
public:
	GuiInstance();
	virtual ~GuiInstance();
	virtual void InstallGui();
	virtual void DrawGpu();

	Ref<Control> host;
protected:
	bool useEffectA = false;
	bool useEffectB = false;
private:
	GuiRenderData renderData;

	

	struct CursorState {
		int x;
		int y;
		bool buttonLeft;
		bool buttonRight;
		int  buttonMidleAxel;
	};

	Ref<Control> controlInHand;
	Ref<Control> current;
	Ref<Control> max;
	int maxId;

	void ProcessMaxium(int depthId, const Ref<Control>& testControl, const Point& parentGloblPosition, const RectangleF & parentClip, const RectangleF & mousePoint);
	bool leftPressed;
	bool rightPressed;
	void AllProcess(int displayW, int displayH, const CursorState& state);

	void ProcessDraw(GuiRenderData* renderData, const Point& globParentlPosition, const Ref<Control>& renderControl, const RectangleF& parentClip);

	void Draw(int displayW, int displayH);

	struct GuiVertex
	{
		Vector2 pos;
		Vector2 tex;
		Vector4 color;		
	};
	Ref<Gl3dVertexArrayBase> guiVertexArray;
	Ref<Gl3dArray<GuiVertex>> guiVertexes;
	Ref<Gl3dShader> shader;
	Ref<Gl3dShader> rgbSplit;
	
};

#endif