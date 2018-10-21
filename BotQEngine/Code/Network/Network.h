#pragma once

#include "../Common/String.h"
#include "../Common/StorageFunction.h"
#include "../Component.h"
#include "../GameObject.h"
#include "tinyrpc.h"

enum class InstantType 
{
	Buffered,
	Once
};

class NetworkHandlers 
{
public:
	virtual ~NetworkHandlers() {}

	virtual void OnLoadLevelFinal() = 0;

};

class SGE_EXPORT Network 
{
public:
	static void Connect(const ts::ip_end_point& to);
	static void Disconent();
	static bool IsConnected();
	static void Listen(int port);
	static bool IsServer();
	static void SetHandlers(NetworkHandlers* handlers);
	static void RegisterNetworkComponent(const String& name, Component::ComponentConstructor);
	static GameObject* Instantiate(const String& prefabName, const String& networkEntityTypeName, ts::entity_id id,
		InstantType type = InstantType::Once);
	static GameObject* Instantiate(const String& prefabName, const Vector3& position, const Quaternion& rotation,
		const String& networkEntityTypeName, ts::entity_id id, InstantType type = InstantType::Once);
	static GameObject* Instantiate(ts::user_id userId, const String& prefabName, const String& networkEntityTypeName, ts::entity_id id);
	static GameObject* Instantiate(ts::user_id userId, const String& prefabName, const Vector3& position, const Quaternion& rotation,
		const String& networkEntityTypeName, ts::entity_id id);
	static void Destroy(GameObject* object);

	static Component* GetCompoentFromNetworkId(ts::entity_id id);

	template<typename T>
	static inline T* GetFromNetworkId(ts::entity_id id) 
	{
		return dynamic_cast<T*>(GetCompoentFromNetworkId(id));
	}

	static void ProcessCommands(int maxCmdPercall = 64);

	static void Unbound();

	static void UnboundDisconent();
};