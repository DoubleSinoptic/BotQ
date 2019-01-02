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
"out vec3 var_position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"\t\tgl_Position = projection * view * model * vec4(inPosition, 1.0); \n"
"var_normal = mat3(model) * inNormal;"
"var_position = (model * vec4(inPosition, 1.0)).xyz;"
"}";

const char* fragmentShader =
"#version 130\n"
"#extension GL_ARB_explicit_attrib_location : enable\n"
"layout ( location = 0 ) out vec3 OutFragColor;\n"
"layout ( location = 1 ) out vec3 OutPosition;\n"
"layout ( location = 2 ) out vec3 OutNormal;\n"
"\n"
"in vec3 var_normal;\n"
"in vec3 var_position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"\tOutFragColor = vec3(1.0);\t\n"
"\tOutPosition = var_position; \t\n"
"\tOutNormal = var_normal; \t\n"
"}";

DynamicArray<class SampleSyzaRender*> m_renders;

class SampleSyzaRender : public Component
{
public:
	Matrix4			m_transform;
	Matrix4			m_scale;
	bool			m_isCube = false;
	void Destroy() override
	{
		renderThreadQueue.QueueFunctionWait([=]()
		{
			m_renders.Remove(this);
		});
	}

	void Awake() override
	{
		m_scale = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
		SetEnabled(true);

		renderThreadQueue.QueueFunction([=]()
		{
			m_renders.Add(this);
		});

	}

	void FrameUpdate() override
	{
		GetGameObject()->FillMatrix4x4((float*)&m_transform);
		m_transform = m_transform * m_scale;
	}
};

const float quadVertexes[] =
{
	-1.0f, 1.0f,
	1.0f, 1.0f, 
	1.0f, -1.0f, 
	-1.0f, 1.0f,
	1.0f, -1.0f,
	-1.0f, -1.0f
};

class Renderer
{
public:
	Ref<Gl3dShader>             _blit;
	Gl3dFrameBufferInstance     _rezult;
	Gl3dLayoutInstance			_cube;
	Ref<Gl3dArray<Vector3>>		_cube_vertexes;
	Ref<Gl3dArray<Vector3>>		_cube_normals;
	Ref<Gl3dArray<int>>			_cube_indeces;

	Gl3dLayoutInstance			_sphere;
	Ref<Gl3dArray<Vector3>>		_sphere_vertexes;
	Ref<Gl3dArray<Vector3>>		_sphere_normals;
	Ref<Gl3dArray<int>>			_sphere_indeces;

	Ref<Gl3dArray<Vector2>>		_quadVertexes;
	Gl3dLayoutInstance			_quad;

	Ref<Gl3dShader>				_basicShader;

	Gl3dTexture					_depth;
	Gl3dTexture					_color;
	Gl3dTexture					_normal;
	Gl3dTexture					_position;




	Renderer()
	{
		UpdateShaders();
		UpdateFrameBuffers(Size(1, 1));

		Gl3dFrameBufferDesc buffDesc = {};
		buffDesc.depthAttachment = &_depth;
		buffDesc.colorAttachments[0] = &_color;
		buffDesc.colorAttachments[1] = &_position;
		buffDesc.colorAttachments[2] = &_normal;
		_rezult.Create(&buffDesc);

	
		_quadVertexes = new Gl3dArray<Vector2>(Gl3dArrayTarget::Array);
		_quadVertexes->Add((Vector2*)quadVertexes, 6);

		_cube_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_cube_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_vertexes->Add((Vector3*)model_cube::vertexes, model_cube::vertexesCount);
		_cube_indeces->Add((int*)model_cube::indeces, model_cube::indecesCount);
		_cube_normals->Add((Vector3*)model_cube::normals, model_cube::vertexesCount);

		_sphere_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_sphere_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_vertexes->Add((Vector3*)model_monkey::vertexes, model_monkey::vertexesCount);
		_sphere_indeces->Add((int*)model_monkey::indeces, model_monkey::indecesCount);
		_sphere_normals->Add((Vector3*)model_monkey::normals, model_monkey::vertexesCount);

		Gl3dLayoutDesc desc = {};
		desc.index = _sphere_indeces.GetPtr();
		desc.layouts[0] = { _sphere_vertexes.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		desc.layouts[1] = { _sphere_normals.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		_sphere.Create(&desc);
		desc.index = _cube_indeces.GetPtr();
		desc.layouts[0] = { _cube_vertexes.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		desc.layouts[1] = { _cube_normals.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		_cube.Create(&desc);
		desc.index = nullptr;
		desc.layouts[0] = { _quadVertexes.GetPtr(), 2, Gl3dFundamentalType::Float, 2 * sizeof(float), 0, false };
		desc.layouts[1] = {};
		_quad.Create(&desc);

	

	}


	void UpdateShaders() 
	{
		_blit = New<Gl3dShader>();
		_basicShader = New<Gl3dShader>();

		_blit->LoadFiles("./blit.vert", "./blit.frag", nullptr, 0);

		_basicShader->Build(vertexShader, ShaderType::VertexShader);
		_basicShader->Build(fragmentShader, ShaderType::FragmentShader);
		_basicShader->Relink();
	}

	void UpdateFrameBuffers(Size viewSize)
	{
		_color.SetData(viewSize.width, viewSize.height, PixelFormat::RGB_16F, nullptr);
		_normal.SetData(viewSize.width, viewSize.height, PixelFormat::RGB_16F, nullptr);
		_position.SetData(viewSize.width, viewSize.height, PixelFormat::RGB_32F, nullptr);
		_depth.SetData(viewSize.width, viewSize.height, PixelFormat::DEPTH_16, nullptr);
	}
	Size lastSize = Size(-1, -1);

	void OnDraw()
	{
		if (Input::IsKeyDown(SGE_KEY_3))
		{
			UpdateShaders();
		}


		Size viewSize = Display::GetCurrent()->GetSize();
		if (viewSize.width < 1 || viewSize.height < 1)
			return;
		if (viewSize != lastSize)
		{
			UpdateFrameBuffers(viewSize);
			lastSize = viewSize;		
		}
		
		

		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Front;
			passDesc.depth = Gl3dDepth::LessEqual;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::DepthColor;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			Gl3dRenderPas pass(_basicShader.GetPtr(), &_rezult, &passDesc);

			Matrix4 view = Matrix4::LookAt(Vector3(-12.0, 12.0, -12.0), Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));
			Matrix4 projection = Matrix4::Perspective(3.14f / 2.0f, float(viewSize.width) / float(viewSize.height), 0.1, 1000);
			pass.Uniform("projection", projection);
			pass.Uniform("view", view);

			for (size_t i = 0; i < m_renders.Length(); i++)
			{
				pass.Uniform("model", m_renders[i]->m_transform);
				if (m_renders[i]->m_isCube)
					_cube.DrawIndexed(Gl3dFundamentalType::UInteger, _cube_indeces->Length());
				else
					_sphere.DrawIndexed(Gl3dFundamentalType::UInteger, _sphere_indeces->Length());
			}

		}
		
		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Disable;
			passDesc.depth = Gl3dDepth::Disable;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::DepthColor;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			Gl3dRenderPas pass(_blit.GetPtr(), nullptr, &passDesc);
			pass.Uniform("color", &_color);
			pass.Uniform("position", &_position);
			pass.Uniform("normal", &_normal);
			_quad.Draw(_quadVertexes->Length());

		}
	

	}

};

/*

hello world
hello
*/
//
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
			TimeSpan s = Time::GetTotalMicroseconds();
			
			m_renderer->OnDraw();
			m_window->SwapBuffers();
			//printf("fr: %f\n", TimeSpawnToFloatSeconds(Time::GetTotalMicroseconds() - s));
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

class Comp : public Component
{
public:
	void Awake() override
	{	
		SetEnabled(true);


	}

	void FrameUpdate() override
	{
		if (Input::IsKeyDown(SGE_KEY_1))
		{
			GameObject* lookObject = new GameObject();
			lookObject->SetPosition(Vector3(0, 5, 0));
			lookObject->SetRotation(Quaternion(0.5, 0.1, 0.3));
			lookObject->AddComponent<RigidBody>()->SetMass(1.0f);
			lookObject->AddComponent<BoxCollider>()->SetSize(Vector3(1.0f, 1.0f, 1.0f));
			lookObject->AddComponent<SampleSyzaRender>()->m_isCube = true;
		}
		if (Input::IsKeyDown(SGE_KEY_2))
		{
			GameObject::ClearScene();
		}
		if (Input::IsKeyDown(SGE_KEY_3))
		{
			
		}
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


	/*	for(int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
			{
				GameObject* lookObject = new GameObject();
				lookObject->SetPosition(Vector3(i - 5, -1, j + 5));
				lookObject->AddComponent<FixedRotation>();
				lookObject->AddComponent<SampleSyzaRender>();
			}
		*/
		{
			GameObject* lookObject = new GameObject();
			lookObject->SetPosition(Vector3(0, -3, 0));
			lookObject->AddComponent<RigidBody>()->SetMass(0.0f);
			lookObject->AddComponent<BoxCollider>()->SetSize(Vector3(62.0f, 1.0f, 62.0f));
			lookObject->AddComponent<SampleSyzaRender>()->m_isCube = true;
			lookObject->GetComponent<SampleSyzaRender>()->m_scale = Matrix4::Scale(Vector3(62.0f, 1.0f, 62.0f));
		}
		/*
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
			{
				GameObject* lookObject = new GameObject();
				lookObject->SetPosition(Vector3(i - 5, 3, j + 5));
				lookObject->SetRotation(Quaternion(0.5, 0.1, 0.3));
				lookObject->AddComponent<RigidBody>()->SetMass(1.0f);
				lookObject->AddComponent<BoxCollider>()->SetSize(Vector3(1.0f, 1.0f, 1.0f));
				lookObject->AddComponent<SampleSyzaRender>()->m_isCube = true;
			}*/
		
		GameObject* d = new GameObject();
		d->AddComponent<Comp>();

		while (instnace.GameLoop())
		{
			
		}


	}



	return 0;
}

