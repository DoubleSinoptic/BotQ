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

struct UpdateCmdDesc : public CommandDesc
{
	class SampleSyzaRender* m_parent;
	virtual void Execute() override;
};

class SampleSyzaRender : public Component 
{
public:
	Matrix4			m_transform;
	UpdateCmdDesc	m_updateCommandDesc;

	void Destroy() override
	{
		GameInstance::GetCurrent()->renderThreadQueue.EnqueueAndWaitDelete(CreateCommand([&]()
		{
			m_renders.Remove(this);
		}));
	}

	void Awake() override
	{
		SetEnabled(true);
		GameInstance::GetCurrent()->renderThreadQueue.EnqueueAndWaitDelete(CreateCommand([&]()
		{
			m_renders.Add(this);
		}));
	}

	void PhysicUpdate() override 
	{	
		m_updateCommandDesc.m_parent = this;
		GameInstance::GetCurrent()->renderThreadQueue.Enqueue(&m_updateCommandDesc);
	}
};

void UpdateCmdDesc::Execute()
{
	m_parent->GetGameObject()->FillMatrix4x4((float*)&m_parent->m_transform);
}



class Renderer
{
public:
	GameObject* lookObject;
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


		lookObject = new GameObject();
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


		_basicShader = new Gl3dShader();
		_basicShader->Build(vertexShader, ShaderType::VertexShader);
		_basicShader->Build(fragmentShader, ShaderType::FragmentShader);
		_basicShader->Relink();
	
	}


	void OnDraw()
	{
		Size viewSize = Display::GetCurrent()->GetSize();
		
	
		{
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




int main()
{




	/*
	Вырубаем сетевой инстансинг. чтобы не жрало при инициализации
	GameInstance::InstindateNetwork(50.0f, false, false);
	GameInstance::SolveProblems(-1);
	*/

	/*if(NV_CheckPipelineCompression())
		MessageBox(nullptr, TEXT("Ошибка видео ускорителя. Ваша видеокарта агресивно использует\npipeline сжатие. это может вызывать падение производительности. Если хотите избежать данной проблему запустите игру с правами администратора"), nullptr, MB_ICONWARNING);
	*/
	Window wnd;


	{
		Guard g([]()
		{
			Network::UnboundDisconent();
		});
		GameInstance instnace;
		instnace.renderTickRate = 24;
		instnace.SetThisCurrent();


		wnd.AttachKeyCallbacks(&instnace);
		Gl3dDevice::LinkAddresses((Gl3dDeviceLinker)glfwGetProcAddress);


		GameObject* camera = new GameObject("Camera");

		camera->SetTransform(Vector3(3.483976, 27.737846, -9.487122), Quaternion(-0.025084, 0.013085, 0.000328, 0.999600));
		
	
		Display::GetCurrent()->SetCamera(camera);

		std::atomic_int physRatio{ 0 };
		std::atomic_int gpuRatio{ 0 };
		Renderer renderer;
		SyncObject o;
		std::thread physicsThread([&]()
		{

			/*Ставим тик рейт в ноль чтобы в delt'у не попадал тик
			таймера*/
			instnace.renderTickRate = 0;
			while (!o.SwapSignal())
			{
				/*функции Update и RenderUpdate возвращяет True если
				произошло обновление и false если для обновления ещё не пришло время*/
				if (instnace.Update())
				{
					if (LevelBase::GetCurrent())
						LevelBase::GetCurrent()->UpdateLevel();
					physRatio++;
					Network::ProcessCommands(64);
				}
				else
					std::this_thread::yield();
			}
		});

		double p = Time::GetTotalTime();

		SyncObject additionalCloseSignal;



		while (wnd.Update() && !additionalCloseSignal.SwapSignal())
		{
			if (instnace.RenderUpdate())
			{
				
				renderer.OnDraw();
				wnd.SwapBuffers();				
				gpuRatio++;
			}
			else
				std::this_thread::yield();

		}

		o.Notify();
		physicsThread.join();


	}



	return 0;
}

