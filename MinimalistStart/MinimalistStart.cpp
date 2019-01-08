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


//
//struct MeshSmooth
//{
//	std::vector<Vector3> v;
//	std::vector<Vector2> t;
//	std::vector<Vector3> n;
//	std::vector<int>	 tc;
//	std::vector<int>	 nc;
//
//	std::vector<int>  s;
//
//	int add_vertex(
//		const Vector3& va,
//		const Vector2& ta,
//		const Vector3& na)
//	{
//		int foundIndex = -1;
//		for (int i = 0; i < v.size(); i++)
//			if (v[i] == va)
//			{
//				foundIndex = i;
//				break;
//			}
//		if (foundIndex != -1)
//		{
//			t[foundIndex] += ta;
//			n[foundIndex] += na;
//			tc[foundIndex] += 1;
//			nc[foundIndex] += 1;
//			return;
//		}
//
//		int i = v.size();
//		v.push_back(va);
//		t.push_back(ta);
//		n.push_back(na);
//		tc.push_back(1);
//		nc.push_back(1);
//		return i;
//	}
//
//	void post_process() 
//	{
//		for (int i = 0; i < v.size(); i++) 
//		{
//			t[i] /= tc[i];
//			n[i] /= nc[i];
//		}
//	}
//
//	void add_triangle(
//		const Vector3& va, const Vector3& vb, const Vector3& vc,
//		const Vector2& ta, const Vector2& tb, const Vector2& tc,
//		const Vector3& na, const Vector3& nb, const Vector3& nc,
//		bool useOrderFixAlgo = false
//	)
//	{		
//		s.push_back(add_vertex(va, ta, na));
//		s.push_back(add_vertex(vb, tb, nb));
//		s.push_back(add_vertex(vc, tc, nc));		
//	}
//};
//

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi

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
template<typename T, typename X>
const T& PSclae(const X* ptr, size_t index)
{
	return reinterpret_cast<const T*>(ptr)[index];
}

template<typename T, typename X>
constexpr T v3t(const X& v) 
{
	return T(v.x, v.y, v.z);
}


class Renderer
{
public:
	Ref<Gl3dShader>             _blit;
	Ref<Gl3dShader>				_geometry;
	Ref<Gl3dShader>				_geometrySkybox;
	Ref<Gl3dShader>				_ssao;
	Ref<Gl3dShader>				_ssaoBlur;

	Gl3dFrameBufferInstance     _geometryFrame;


	Gl3dTexture					_depth;
	Gl3dTexture					_color;
	Gl3dTexture					_normal;
	Gl3dTexture					_position;
	Gl3dTexture					_ssaoNoise;

	Gl3dTexture					_pipe[2];
	Gl3dFrameBufferInstance     _pipeFrame[2];

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

	Renderer()
	{
		UpdateShaders();
		UpdateFrameBuffers(Size(1, 1));

		{
			Gl3dFrameBufferDesc buffDesc = {};
			buffDesc.depthAttachment = &_depth;
			buffDesc.colorAttachments[0] = &_color;
			buffDesc.colorAttachments[1] = &_position;
			buffDesc.colorAttachments[2] = &_normal;
			_geometryFrame.Create(&buffDesc);
		}

		{
			Gl3dFrameBufferDesc buffDesc = {};
			buffDesc.colorAttachments[0] = &_pipe[0];
			_pipeFrame[0].Create(&buffDesc);
			buffDesc.colorAttachments[0] = &_pipe[1];
			_pipeFrame[1].Create(&buffDesc);
		}


	
		_quadVertexes = new Gl3dArray<Vector2>(Gl3dArrayTarget::Array);
		_quadVertexes->Add((Vector2*)quadVertexes, 6);

		_cube_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_cube_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_vertexes->Add((Vector3*)model_cube::vertexes, model_cube::vertexesCount);
		_cube_indeces->Add((int*)model_cube::indeces, model_cube::indecesCount);
		_cube_normals->Add((Vector3*)model_cube::normals, model_cube::vertexesCount);

		/*
		MeshSmooth syza;
		for (int i = 0; i < model_monkey::indecesCount; i+=3)
		{
			syza.add_triangle(
				PSclae<Vector3>(model_monkey::vertexes, i), PSclae<Vector3>(model_monkey::vertexes, i + 1), PSclae<Vector3>(model_monkey::vertexes, i + 2),
				PSclae<Vector2>(model_monkey::texcoords, i), PSclae<Vector2>(model_monkey::texcoords, i + 1), PSclae<Vector2>(model_monkey::texcoords, i + 2),
				PSclae<Vector3>(model_monkey::normals, i), PSclae<Vector3>(model_monkey::normals, i + 1), PSclae<Vector3>(model_monkey::normals, i + 2));
		}
		syza.post_process();

		_sphere_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_sphere_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_vertexes->Add(syza.v.data(), syza.v.size());
		_sphere_indeces->Add(syza.s.data(), syza.s.size());
		_sphere_normals->Add(syza.n.data(), syza.v.size());
		*/

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
		_geometry = New<Gl3dShader>();
		_ssao = New<Gl3dShader>();
		_geometrySkybox = New<Gl3dShader>();
		_ssaoBlur = New<Gl3dShader>();

		_blit->LoadFiles("./blit.vert", "./blit.frag", nullptr, 0);
		_geometry->LoadFiles("./geometry.vert", "./geometry.frag", nullptr, 0);
		_ssao->LoadFiles("./ssao.vert", "./ssao.frag", nullptr, 0);
		_ssaoBlur->LoadFiles("./ssaoBlur.vert", "./ssaoBlur.frag", nullptr, 0);
		const char* mcDefines[] = {"SKYBOX"};
		_geometrySkybox->LoadFiles("./geometry.vert", "./geometry.frag", mcDefines, 1);
		{
			Gl3dRenderPas _ssaoUpdate(_ssao.GetPtr(), nullptr, nullptr);

			DynamicArray<Vector3> ssaoNoise;
			for (unsigned int i = 0; i < 64; ++i)
			{
				Vector3 sample(Mathf::Random() * 2.0 - 1.0, Mathf::Random() * 2.0 - 1.0, Mathf::Random());
				printf("sample: %f %f %f\n", Mathf::Random(), Mathf::Random(), Mathf::Random());
				sample.Normalize();
				sample *= Mathf::Random();
				float scale = float(i) / 64;

				scale = Mathf::Lerp(0.1f, 1.0f, scale * scale);
				sample *= scale;
				_ssaoUpdate.Uniform(*String::Format("kernel[%d]", i), sample);
			}

			for (unsigned int i = 0; i < 16; i++)
			{
				Vector3 noise(Mathf::Random() * 2.0 - 1.0, Mathf::Random() * 2.0 - 1.0, 0.0f);
				ssaoNoise.Add(noise);
			}
			Gl3dSubImageDesc image;
			image.width = 4;
			image.height = 4;
			image.data = (void*)ssaoNoise.GetData();
			image.type = Gl3dFundamentalType::Float;
			image.changelsCount = 3;

			_ssaoNoise.SetData(Gl3dPixelFormat::RGB_32F, &image);
		}
		


	}

	void UpdateFrameBuffers(Size viewSize)
	{
		_color.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
		_normal.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_32F);
		_position.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_32F);
		_position.SetWrapMode(Gl3dWrapMode::ClampToEdge, Gl3dWrapMode::ClampToEdge, Gl3dWrapMode::ClampToEdge);
		_depth.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::DEPTH_16);
		_pipe[0].AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
		_pipe[1].AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
	}
	Size lastSize = Size(-1, -1);

	void OnDraw()
	{
		if (Input::IsKeyDown(SGE_KEY_3))
		{
			UpdateShaders();
		}

		//ssaoInput
		Size viewSize = Display::GetCurrent()->GetSize();
		if (viewSize.width < 1 || viewSize.height < 1)
			return;
		if (viewSize != lastSize)
		{
			UpdateFrameBuffers(viewSize);
			lastSize = viewSize;		
		}
		
		GameObject* camera = Display::GetCurrent()->GetCamera();
		Vector3 lookPosition = camera->GetPosition();
		auto view =  Matrix4::LookAt(v3t<Vector3>(lookPosition), v3t<Vector3>(lookPosition + camera->GetForward()), v3t<Vector3>(camera->GetUp()));
		auto projection = Matrix4::Perspective(3.14f / 2.0f, float(viewSize.width) / float(viewSize.height), 0.1f, 1400.0f);
		
	/*	glm::cross(glm::vec3(1.0f), glm::vec4(1.0f));*/

		Gl3dRenderPassDesc postDesc = {};
		postDesc.cullFace = Gl3dCullFace::Disable;
		postDesc.depth = Gl3dDepth::Disable;
		postDesc.blending = Gl3dBlending::Disable;
		postDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		postDesc.clearDepth = 1.0f;
		postDesc.clear = Gl3dClear::None;
		postDesc.viewport = { 0, 0, viewSize.width, viewSize.height };
	
	
		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Front;
			passDesc.depth = Gl3dDepth::LessEqual;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::DepthColor;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			Gl3dRenderPas pass(_geometrySkybox.GetPtr(), &_geometryFrame, &passDesc);

			pass.Uniform("projection", projection);
			pass.Uniform("view", view);
			pass.Uniform("model", Matrix4::Scale(Vector3(1000.0f, 1000.0f, 1000.0f)));
			_cube.DrawIndexed(Gl3dPrimitive::Triangles, Gl3dFundamentalType::UInteger, _cube_indeces->Length());
				
		}

		Gl3dDevice::ThrowIfError();

	
		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Back;
			passDesc.depth = Gl3dDepth::LessEqual;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::None;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			
			Gl3dRenderPas pass(_geometry.GetPtr(), &_geometryFrame, &passDesc);

			pass.Uniform("projection", projection);
			pass.Uniform("view", view);

			for (size_t i = 0; i < m_renders.Length(); i++)
			{
				pass.Uniform("model", m_renders[i]->m_transform);
				if (m_renders[i]->m_isCube )
					_cube.DrawIndexed(Gl3dPrimitive::Triangles, Gl3dFundamentalType::UInteger, _cube_indeces->Length());
				else
					_sphere.DrawIndexed(Gl3dPrimitive::Triangles, Gl3dFundamentalType::UInteger, _sphere_indeces->Length());
			}

		}
	
	

		{			
			Gl3dRenderPas pass(_ssao.GetPtr(), &_pipeFrame[0], &postDesc);
			pass.Uniform("position", &_position);
			pass.Uniform("normal", &_normal);
			pass.Uniform("texNoise", &_ssaoNoise);
			pass.Uniform("renderSize", Vector2(viewSize.width, viewSize.height));
			pass.Uniform("sampleCount", 64);
			pass.Uniform("projection", projection);
			pass.Uniform("view", view);			
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}

		{
			Gl3dRenderPas pass(_ssaoBlur.GetPtr(), &_pipeFrame[1], &postDesc);
			pass.Uniform("ssaoInput", &_pipe[0]);
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}

		{
			Gl3dRenderPas pass(_blit.GetPtr(), nullptr, &postDesc);
			pass.Uniform("ssao", &_pipe[1]);
			pass.Uniform("color", &_color);
			pass.Uniform("position", &_position);
			pass.Uniform("normal", &_normal);
			pass.Uniform("lookPosition", lookPosition);
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}
		
	
		Gl3dDevice::ThrowIfError();
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
			//Vector3 size = Vector3(0.5f + Mathf::Random() * 2, 0.5f + Mathf::Random() * 2, 0.5f + Mathf::Random() * 2);
			Vector3 size = Vector3(4, 4, 4);
			GameObject* lookObject = new GameObject();
			lookObject->SetPosition(Vector3(0, 5, 0));
			lookObject->SetRotation(Quaternion(0.5, 0.1, 0.3));
			lookObject->AddComponent<RigidBody>()->SetMass(1.0f);
			lookObject->AddComponent<BoxCollider>()->SetSize(size);
			lookObject->AddComponent<SampleSyzaRender>()->m_isCube = false;
			lookObject->GetComponent<SampleSyzaRender>()->m_scale = Matrix4::Scale(size);
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
		camera->SetTransform(Vector3(-7, 7, -7), Quaternion(-45.0f, 45.0f, 0.0f));
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
		camera->AddComponent<DebugFlyCamera>();
		while (instnace.GameLoop())
		{
			
		}


	}



	return 0;
}
