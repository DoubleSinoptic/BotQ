#pragma once

#include <stdexcept>
#include <Windows.h>
#include <Windowsx.h>


int translateKey(WPARAM wParam, LPARAM lParam)
{
	if ((HIWORD(lParam) & 0x100) == 0)
	{
		switch (MapVirtualKey(HIWORD(lParam) & 0xFF, 1))
		{
		case VK_INSERT:   return SGE_KEY_KP_0;
		case VK_END:      return SGE_KEY_KP_1;
		case VK_DOWN:     return SGE_KEY_KP_2;
		case VK_NEXT:     return SGE_KEY_KP_3;
		case VK_LEFT:     return SGE_KEY_KP_4;
		case VK_CLEAR:    return SGE_KEY_KP_5;
		case VK_RIGHT:    return SGE_KEY_KP_6;
		case VK_HOME:     return SGE_KEY_KP_7;
		case VK_UP:       return SGE_KEY_KP_8;
		case VK_PRIOR:    return SGE_KEY_KP_9;
		case VK_DIVIDE:   return SGE_KEY_KP_DIVIDE;
		case VK_MULTIPLY: return SGE_KEY_KP_MULTIPLY;
		case VK_SUBTRACT: return SGE_KEY_KP_SUBTRACT;
		case VK_ADD:      return SGE_KEY_KP_ADD;
		case VK_DELETE:   return SGE_KEY_KP_DECIMAL;
		default:          break;
		}
	}

	switch (wParam)
	{
		
	case VK_SHIFT:
	{
		const DWORD scancode = MapVirtualKey(VK_RSHIFT, 0);
		if ((DWORD)((lParam & 0x01ff0000) >> 16) == scancode)
			return SGE_KEY_RIGHT_SHIFT;

		return SGE_KEY_LEFT_SHIFT;
	}

	case VK_CONTROL:
	{
		MSG next;
		DWORD time;

		
		if (lParam & 0x01000000)
			return SGE_KEY_RIGHT_CONTROL;

		
		time = GetMessageTime();

		if (PeekMessage(&next, NULL, 0, 0, PM_NOREMOVE))
		{
			if (next.message == WM_KEYDOWN ||
				next.message == WM_SYSKEYDOWN ||
				next.message == WM_KEYUP ||
				next.message == WM_SYSKEYUP)
			{
				if (next.wParam == VK_MENU &&
					(next.lParam & 0x01000000) &&
					next.time == time)
				{
					
					return 0;
				}
			}
		}

		return SGE_KEY_LEFT_CONTROL;
	}


	case VK_MENU:
	{
		
		if (lParam & 0x01000000)
			return SGE_KEY_RIGHT_ALT;

		return SGE_KEY_LEFT_ALT;
	}


	case VK_RETURN:
	{
		
		if (lParam & 0x01000000)
			return SGE_KEY_KP_ENTER;

		return SGE_KEY_ENTER;
	}


	case VK_ESCAPE:        return SGE_KEY_ESCAPE;
	case VK_TAB:           return SGE_KEY_TAB;
	case VK_BACK:          return SGE_KEY_BACKSPACE;
	case VK_HOME:          return SGE_KEY_HOME;
	case VK_END:           return SGE_KEY_END;
	case VK_PRIOR:         return SGE_KEY_PAGE_UP;
	case VK_NEXT:          return SGE_KEY_PAGE_DOWN;
	case VK_INSERT:        return SGE_KEY_INSERT;
	case VK_DELETE:        return SGE_KEY_DELETE;
	case VK_LEFT:          return SGE_KEY_LEFT;
	case VK_UP:            return SGE_KEY_UP;
	case VK_RIGHT:         return SGE_KEY_RIGHT;
	case VK_DOWN:          return SGE_KEY_DOWN;
	case VK_F1:            return SGE_KEY_F1;
	case VK_F2:            return SGE_KEY_F2;
	case VK_F3:            return SGE_KEY_F3;
	case VK_F4:            return SGE_KEY_F4;
	case VK_F5:            return SGE_KEY_F5;
	case VK_F6:            return SGE_KEY_F6;
	case VK_F7:            return SGE_KEY_F7;
	case VK_F8:            return SGE_KEY_F8;
	case VK_F9:            return SGE_KEY_F9;
	case VK_F10:           return SGE_KEY_F10;
	case VK_F11:           return SGE_KEY_F11;
	case VK_F12:           return SGE_KEY_F12;
	case VK_F13:           return SGE_KEY_F13;
	case VK_F14:           return SGE_KEY_F14;
	case VK_F15:           return SGE_KEY_F15;
	case VK_F16:           return SGE_KEY_F16;
	case VK_F17:           return SGE_KEY_F17;
	case VK_F18:           return SGE_KEY_F18;
	case VK_F19:           return SGE_KEY_F19;
	case VK_F20:           return SGE_KEY_F20;
	case VK_F21:           return SGE_KEY_F21;
	case VK_F22:           return SGE_KEY_F22;
	case VK_F23:           return SGE_KEY_F23;
	case VK_F24:           return SGE_KEY_F24;
	case VK_NUMLOCK:       return SGE_KEY_NUM_LOCK;
	case VK_CAPITAL:       return SGE_KEY_CAPS_LOCK;
	case VK_SNAPSHOT:      return SGE_KEY_PRINT_SCREEN;
	case VK_SCROLL:        return SGE_KEY_SCROLL_LOCK;
	case VK_PAUSE:         return SGE_KEY_PAUSE;
	case VK_LWIN:          return SGE_KEY_LEFT_SUPER;
	case VK_RWIN:          return SGE_KEY_RIGHT_SUPER;
	case VK_APPS:          return SGE_KEY_MENU;

	
	case VK_NUMPAD0:       return SGE_KEY_KP_0;
	case VK_NUMPAD1:       return SGE_KEY_KP_1;
	case VK_NUMPAD2:       return SGE_KEY_KP_2;
	case VK_NUMPAD3:       return SGE_KEY_KP_3;
	case VK_NUMPAD4:       return SGE_KEY_KP_4;
	case VK_NUMPAD5:       return SGE_KEY_KP_5;
	case VK_NUMPAD6:       return SGE_KEY_KP_6;
	case VK_NUMPAD7:       return SGE_KEY_KP_7;
	case VK_NUMPAD8:       return SGE_KEY_KP_8;
	case VK_NUMPAD9:       return SGE_KEY_KP_9;
	case VK_DIVIDE:        return SGE_KEY_KP_DIVIDE;
	case VK_MULTIPLY:      return SGE_KEY_KP_MULTIPLY;
	case VK_SUBTRACT:      return SGE_KEY_KP_SUBTRACT;
	case VK_ADD:           return SGE_KEY_KP_ADD;
	case VK_DECIMAL:       return SGE_KEY_KP_DECIMAL;

	case VK_SPACE:         return SGE_KEY_SPACE;
	case 0x30:             return SGE_KEY_0;
	case 0x31:             return SGE_KEY_1;
	case 0x32:             return SGE_KEY_2;
	case 0x33:             return SGE_KEY_3;
	case 0x34:             return SGE_KEY_4;
	case 0x35:             return SGE_KEY_5;
	case 0x36:             return SGE_KEY_6;
	case 0x37:             return SGE_KEY_7;
	case 0x38:             return SGE_KEY_8;
	case 0x39:             return SGE_KEY_9;
	case 0x41:             return SGE_KEY_A;
	case 0x42:             return SGE_KEY_B;
	case 0x43:             return SGE_KEY_C;
	case 0x44:             return SGE_KEY_D;
	case 0x45:             return SGE_KEY_E;
	case 0x46:             return SGE_KEY_F;
	case 0x47:             return SGE_KEY_G;
	case 0x48:             return SGE_KEY_H;
	case 0x49:             return SGE_KEY_I;
	case 0x4A:             return SGE_KEY_J;
	case 0x4B:             return SGE_KEY_K;
	case 0x4C:             return SGE_KEY_L;
	case 0x4D:             return SGE_KEY_M;
	case 0x4E:             return SGE_KEY_N;
	case 0x4F:             return SGE_KEY_O;
	case 0x50:             return SGE_KEY_P;
	case 0x51:             return SGE_KEY_Q;
	case 0x52:             return SGE_KEY_R;
	case 0x53:             return SGE_KEY_S;
	case 0x54:             return SGE_KEY_T;
	case 0x55:             return SGE_KEY_U;
	case 0x56:             return SGE_KEY_V;
	case 0x57:             return SGE_KEY_W;
	case 0x58:             return SGE_KEY_X;
	case 0x59:             return SGE_KEY_Y;
	case 0x5A:             return SGE_KEY_Z;
	case 0xBD:             return SGE_KEY_MINUS;
	case 0xBB:             return SGE_KEY_EQUAL;
	case 0xDB:             return SGE_KEY_LEFT_BRACKET;
	case 0xDD:             return SGE_KEY_RIGHT_BRACKET;
	case 0xDC:             return SGE_KEY_BACKSLASH;
	case 0xBA:             return SGE_KEY_SEMICOLON;
	case 0xDE:             return SGE_KEY_APOSTROPHE;
	case 0xC0:             return SGE_KEY_GRAVE_ACCENT;
	case 0xBC:             return SGE_KEY_COMMA;
	case 0xBE:             return SGE_KEY_PERIOD;
	case 0xBF:             return SGE_KEY_SLASH;
	case 0xDF:             return SGE_KEY_WORLD_1;
	case 0xE2:             return SGE_KEY_WORLD_2;
	default:               break;
	}

	return 0;
}


typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_DRAW_TO_BITMAP_ARB            0x2002
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_NEED_PALETTE_ARB              0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB       0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB        0x2006
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_NUMBER_OVERLAYS_ARB           0x2008
#define WGL_NUMBER_UNDERLAYS_ARB          0x2009
#define WGL_TRANSPARENT_ARB               0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB     0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB   0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB    0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB   0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB   0x203B
#define WGL_SHARE_DEPTH_ARB               0x200C
#define WGL_SHARE_STENCIL_ARB             0x200D
#define WGL_SHARE_ACCUM_ARB               0x200E
#define WGL_SUPPORT_GDI_ARB               0x200F
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_STEREO_ARB                    0x2012
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_RED_BITS_ARB                  0x2015
#define WGL_RED_SHIFT_ARB                 0x2016
#define WGL_GREEN_BITS_ARB                0x2017
#define WGL_GREEN_SHIFT_ARB               0x2018
#define WGL_BLUE_BITS_ARB                 0x2019
#define WGL_BLUE_SHIFT_ARB                0x201A
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_ALPHA_SHIFT_ARB               0x201C
#define WGL_ACCUM_BITS_ARB                0x201D
#define WGL_ACCUM_RED_BITS_ARB            0x201E
#define WGL_ACCUM_GREEN_BITS_ARB          0x201F
#define WGL_ACCUM_BLUE_BITS_ARB           0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB          0x2021
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_AUX_BUFFERS_ARB               0x2024
#define WGL_NO_ACCELERATION_ARB           0x2025
#define WGL_GENERIC_ACCELERATION_ARB      0x2026
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_COPY_ARB                 0x2029
#define WGL_SWAP_UNDEFINED_ARB            0x202A
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_TYPE_COLORINDEX_ARB           0x202C
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001


#pragma comment (lib, "opengl32.lib")

#define INVALID_HANDLE_EXT ((HANDLE)0)
#define DECLARATE_SAFE_CLOSE(type, destr)\
struct SafeClose ## type\
{\
	type value;\
	SafeClose ## type   &operator=(const SafeClose ## type  &) = delete;\
	SafeClose ## type (const SafeClose ## type &) = delete;\
	template<typename T>\
	void Set (T val) \
	{ \
		if (value != static_cast< type >(INVALID_HANDLE_EXT) )\
		{\
			destr ;\
		}\
		value = static_cast< type >(val); \
	}\
	template<typename T>\
	SafeClose ## type (T val) :\
		value(static_cast< type >(val))\
	{}\
	operator SafeClose ## type () const\
	{\
		return value;\
	}\
	bool IsValid() const\
	{\
		return value != static_cast<type>(INVALID_HANDLE_EXT);\
	}\
	~SafeClose ## type ()\
	{\
		if (value != static_cast< type >(INVALID_HANDLE_EXT) )\
		{\
			destr ;\
		}\
	}\
};





DECLARATE_SAFE_CLOSE(HWND, DestroyWindow(value))
DECLARATE_SAFE_CLOSE(HGLRC, wglDeleteContext(value))
DECLARATE_SAFE_CLOSE(HDC, ReleaseDC(WindowFromDC(value), value))


void CreaeteStrongContext(SafeCloseHDC& dc, SafeCloseHGLRC& rc)
{

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int win32PixFormat = ChoosePixelFormat(dc.value, &pfd);
	if (!win32PixFormat)
		throw std::runtime_error("error of choose pixel format");
	
	if (!SetPixelFormat(dc.value, win32PixFormat, &pfd))
		throw std::runtime_error("error of set pixel format");

	rc.Set(wglCreateContext(dc.value));
	if (!rc.IsValid())
		throw std::runtime_error("error of create opengl context");
	if (!wglMakeCurrent(dc.value, rc.value))
		throw std::runtime_error("error of make current");

	float  fAttributes[] = { 0, 0 };
	int    iAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, true,
		WGL_SUPPORT_OPENGL_ARB, true,
		WGL_DOUBLE_BUFFER_ARB, true,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_SAMPLE_BUFFERS_ARB, true,
		WGL_SAMPLES_ARB, 4,
		0,0
	};

	UINT ctNum;
	int valid = ((PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB")))
		(dc.value, iAttributes, fAttributes, 1, &win32PixFormat, &ctNum);
	if (valid && ctNum < 1)
		throw std::runtime_error("error of install format ARB ext");


	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	rc.Set(((PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB")))(dc.value, 0, attribs));
	if (!rc.IsValid())
		throw std::runtime_error("error of create opengl context");
	if (!wglMakeCurrent(dc.value, rc.value))
		throw std::runtime_error("error of make current");

}



const TCHAR* const WindowStaticClassName =
TEXT("_ZN20SgeSimpleWindowClassTP");

const TCHAR* const WindowStaticTitleText =
TEXT("Sge Win32 Backend writed for HOMEWORK");

struct WindowStaticAdapter
{
	bool registerd;
	WNDCLASS wc;

	WindowStaticAdapter();

	~WindowStaticAdapter();

} __staticProxy;

bool key_tab[400] = {0};
bool mk_tab[12] = {0};
int oldCursorX = 0;
int oldCursorY = 0;

class WindowHM
{
	MSG   msg;
	SafeCloseHWND window;
	SafeCloseHGLRC context;
	SafeCloseHDC   userDc;


public:
	static LRESULT CALLBACK OpenGlWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			int kewy = translateKey(wParam, lParam);
			key_tab[kewy] = true;
			break;;
		}		
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			int kewy = translateKey(wParam, lParam);
			key_tab[kewy] = false;
			break;
		}		
		
		case WM_MOUSEMOVE:
		{
			const int newCursorX = GET_X_LPARAM(lParam);
			const int newCursorY = GET_Y_LPARAM(lParam);

			if (newCursorX != oldCursorX ||
				newCursorY != oldCursorY)
			{
				int x, y;

				
				x = newCursorX;
				y = newCursorY;
				

				oldCursorX = newCursorX;
				oldCursorY = newCursorY;
				
				if(GameInstance::GetCurrent())
					GameInstance::GetCurrent()->display->SetMausePosition({x, y});
			}


			break;
		}

		case WM_LBUTTONUP:
			mk_tab[0] = false;
			break;
		case WM_RBUTTONUP:
			mk_tab[1] = false;
			break;
		case WM_LBUTTONDOWN:
			mk_tab[0] = true;
			break;
		case WM_RBUTTONDOWN:
			mk_tab[1] = true;
			break;

		}
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

	/*fwd:MakeCurrent*/
	void InstallContextToCurrentThread()
	{
		if (context.IsValid())
			if (!wglMakeCurrent(userDc.value, context.value))
				throw std::runtime_error("error of make current");
	}


	WindowHM() :
		window(INVALID_HANDLE_EXT),
		context(INVALID_HANDLE_EXT),
		userDc(INVALID_HANDLE_EXT)
	{
		window.Set(CreateWindow(WindowStaticClassName, WindowStaticTitleText,
			WS_OVERLAPPEDWINDOW |
			WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN, 100, 100, 1280, 768, NULL, NULL, __staticProxy.wc.hInstance, NULL));
		if (!window.IsValid())
			throw std::runtime_error("error of create window");

		ShowWindow(window.value, 1);

		userDc.Set(GetDC(window.value));
		CreaeteStrongContext(userDc, context);
	}


	void AttachKeyCallbacks(class GameInstance* instance)
	{
		instance->display->SetKeyCallback([=](int code) -> int
		{	
			return key_tab[code];
		});

		instance->display->SetMouseKeyCallback([=](int code) -> int
		{
			return mk_tab[code];
		});
	}

	static PROC GetProcessAddress(const char* asciiProcId)
	{
		PROC d = wglGetProcAddress(asciiProcId);
		if (d == 0) 
		{
			HMODULE module = LoadLibraryA("opengl32.dll");
			d = GetProcAddress(module, asciiProcId);
		}
		return d;
	}

	void SwapBuffers() 
	{
		glFlush();
		::SwapBuffers(userDc.value);
	}

	bool Update()
	{
		if (!GetMessage(&msg, window.value, 0, 0))
			return false;

		RECT r;
		GetClientRect(window.value, &r);
		if (GameInstance::GetCurrent())
			GameInstance::GetCurrent()->display->SetSize({ r.right - r.left , r.bottom - r.top  });
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		InvalidateRect(window.value, &r, false);

		return true;
	}
};

WindowStaticAdapter::WindowStaticAdapter() :
	registerd(false)

{
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)(WindowHM::OpenGlWindowProc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowStaticClassName;

	if (!RegisterClass(&wc))
		throw std::runtime_error("error of register window class");
	registerd = true;
}

inline WindowStaticAdapter::~WindowStaticAdapter()
{
	if (registerd)
		UnregisterClass(WindowStaticClassName, wc.hInstance);
}
