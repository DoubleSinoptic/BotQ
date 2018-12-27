#include "pch.h"
/*
схема многопоточного пуска:
после окончания обновления движка отправляем рендеру сигнал о пуске отрисовки нового
кадра. при этом пока сигнала о отрисовки не приходит выполняем переброску операций в графичесий поток
 чтобы физический поток мог создавать в GPU меши и текстуры.
*/



/*
GPU->main thread
Phys->child thread
*/

#include <vector>
#include <mutex>
const char* vertexShader = 
"#version 130\n"
"#extension GL_ARB_explicit_attrib_location : enable\n"
"layout (location = 0) in vec3 inPosition;\n"
"layout (location = 1) in vec3 inNormal;\n"
"out vec3 var_normal;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"\t\tgl_Position = projection * view * model * vec4(inPosition, 1.0); \n"
"var_normal = mat3(model) * inNormal;"
"}";

const char* fragmentShader = 
"#version 130\n"
"#extension GL_ARB_explicit_attrib_location : enable\n"
"layout ( location = 0 ) out vec4 OutFragColor;\n"
"\n"
"in vec3 var_normal;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"\tOutFragColor = vec4(vec3(1.0) * clamp(dot(normalize(var_normal), -vec3(0, 0, 1)), 0.0, 1.0) ,1.0);\t\n"
"}";

DynamicArray<class SampleSyzaRender*> m_renders;

class SampleSyzaRender : public Component 
{
public:
	Matrix4			m_transform;

	void Destroy() override
	{
		renderThreadQueue.QueueFunctionWait([=]()
		{
			m_renders.Remove(this);
		});
	}

	void Awake() override
	{
		SetEnabled(true);
		
		renderThreadQueue.QueueFunction([=]() 
		{	
			m_renders.Add(this);
		});
	
	}

	void FrameUpdate() override 
	{			
		GetGameObject()->FillMatrix4x4((float*)&m_transform);
	}
};


class Renderer
{
public:
	
	Ref<Gl3dVertexArrayBase>	_sphere;
	Ref<Gl3dArray<Vector3>>		_sphere_vertexes;
	Ref<Gl3dArray<Vector3>>		_sphere_normals;
	Ref<Gl3dArray<int>>			_sphere_indeces;

	Ref<Gl3dShader>				_basicShader;
 	Renderer() 
	{
		_sphere = new Gl3dVertexArrayBase();
		_sphere_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_sphere_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_vertexes->Add((Vector3*)model_monkey::vertexes, model_monkey::vertexesCount);
		_sphere_indeces->Add((int*)model_monkey::indeces, model_monkey::indecesCount);
		_sphere_normals->Add((Vector3*)model_monkey::normals, model_monkey::vertexesCount);
		_sphere->Attach(0, _sphere_vertexes.GetPtr());
		_sphere->Attach(1, _sphere_normals.GetPtr());
		_sphere->Attach(2, _sphere_indeces.GetPtr());


		_basicShader = new Gl3dShader();
		_basicShader->Build(vertexShader, ShaderType::VertexShader);
		_basicShader->Build(fragmentShader, ShaderType::FragmentShader);
		_basicShader->Relink();
	
	}


	void OnDraw()
	{
		Size viewSize = Display::GetCurrent()->GetSize();
		
	
		{
			if (viewSize.width < 1 || viewSize.height < 1)
				return;

			Gl3dRenderPassDesc passDesc;
			passDesc.cullFace = Gl3dCullFace::Front;
			passDesc.depth = Gl3dDepth::LessEqual;	
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::DepthColor;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			Gl3dRenderPas pass(_basicShader.GetPtr(), nullptr, &passDesc);
			
			Matrix4 view = Matrix4::LookAt(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, 1.0, 0.0));
			Matrix4 projection = Matrix4::Perspective(3.14f /2.0f, float(viewSize.width) / float(viewSize.height), 0.1, 1000);
			pass.Uniform("projection", projection);
			pass.Uniform("view", view);

			for (size_t i = 0; i < m_renders.Length(); i++)
			{
				pass.Uniform("model", m_renders[i]->m_transform);
				_sphere->DrawIndexed(Gl3dDrawPrimitive::Triangles, _sphere_indeces->Length());
			}

			

		}

	}

};

/*

hello world
hello
*/

class GameViewManager2 : public IRenderThreadInstance
{
	Renderer* m_renderer;
	Window* m_window;
	CommandQueue m_queue;
	std::atomic_bool m_closeFlag;
	std::atomic_bool m_destroyFlag;
	std::thread m_renderThread;
public:
	GameViewManager2() :
		m_destroyFlag(false),
		m_closeFlag(false),
		m_renderer(nullptr),
		m_window(nullptr),
		m_renderThread([&]()
		{
			m_queue.Attach();
			while (!m_destroyFlag.load())
			{
				m_queue.Playback();
			}
		})
	{

	}

	~GameViewManager2() 
	{
		m_destroyFlag.store(true);
		m_renderThread.join();
	}

	virtual CommandQueue & GetCommandQueue() override
	{
		return m_queue;
	}

	virtual bool GetCloseStatus() override
	{
		return m_closeFlag;
	}

	virtual void Update(GameInstance* instance) override
	{
		m_queue.QueueFunction([=]()
		{
			m_closeFlag = !m_window->Update();
		});
	}

	virtual void Draw(GameInstance* instance) override
	{
		m_queue.QueueFunction([=]()
		{
			m_renderer->OnDraw();
			m_window->SwapBuffers();
		});
	}

	virtual void Start(GameInstance* instance) override
	{
		m_queue.QueueFunctionWait([=]()
		{
			m_window = new Window();
			m_window->AttachKeyCallbacks(instance);
			Gl3dDevice::LinkAddresses((Gl3dDeviceLinker)glfwGetProcAddress);
			m_renderer = new Renderer();
		});
	}

	virtual void Stop(GameInstance* instance) override
	{
		m_queue.QueueFunctionWait([=]() 
		{
			m_window->MakeCurrent();
			if (m_renderer)
			{
				delete m_renderer;
				m_renderer = nullptr;
			}
			if (m_window)
			{
				delete m_window;
				m_window = nullptr;
			}
		});
	}
};



int main()
{

	{
		Guard g([]()
		{
			Network::UnboundDisconent();
		});

		GameViewManager2 mag;
		GameInstance instnace(&mag);

		instnace.renderTickRate = 60;
		instnace.SetThisCurrent();


		GameObject* camera = new GameObject("Camera");
		camera->SetTransform(Vector3(3.483976, 27.737846, -9.487122), Quaternion(-0.025084, 0.013085, 0.000328, 0.999600));
		Display::GetCurrent()->SetCamera(camera);

	

		GameObject* lookObject = new GameObject();
		lookObject->SetPosition(Vector3(1, 0, 4));
		lookObject->AddComponent<FixedRotation>();
		lookObject->AddComponent<SampleSyzaRender>();


		lookObject = new GameObject();
		lookObject->SetPosition(Vector3(-1, 0, 4));
		lookObject->AddComponent<FixedRotation>();
		lookObject->AddComponent<SampleSyzaRender>();


		lookObject = new GameObject();
		lookObject->SetPosition(Vector3(0, 0, 4));
		lookObject->AddComponent<FixedRotation>();
		lookObject->AddComponent<SampleSyzaRender>();

	
		while (!mag.GetCloseStatus())
		{
			instnace.Update();
			instnace.RenderUpdate();
			
		}

	
	}



	return 0;
}

