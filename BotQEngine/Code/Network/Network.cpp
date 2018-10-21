
#include "Network.h"
#include "../GameObject.h"
#include "NetworkEntityBase.h"
#include <map>
NetworkHandlers* handlers = nullptr;
void Network::SetHandlers(NetworkHandlers * handslers)
{
	handlers = handslers;
}

uint32_t flowIndexEnumerator = 0;
Ref<ts::rpc_listener> listener;
Ref<ts::rpc_client> client;
std::map<String, Component::ComponentConstructor> components;

class ClientNetworkInstantor : public ts::rpc_entity
{
public:

	void rpc(RPC_ENTITY_ARGS) override 
	{
		if (rpcName == ts::cmd_hash("@spawn"))
		{
			
			String a0 = arguments.read_string().c_str();
			String a1 = arguments.read_string().c_str();
			Vector3 a2 = arguments.read_object<Vector3>();
			Quaternion a3 = arguments.read_object<Quaternion>();
			GameObject* o = GameObject::SpawnPrefab(
				a0,
				a1,
				a2,
				a3);
			
			String netName = arguments.read_string().c_str();
			auto& cmpcs = components[netName];
			if (cmpcs)
			{
				NetworkEntityBase* xCmp = dynamic_cast<NetworkEntityBase*>(o->AddComponent(cmpcs));
				if (xCmp == nullptr)
					throw std::runtime_error("invalid network component attachment");
				xCmp->set_id(arguments.read_uint32());
				xCmp->SetNetworkName(netName);
			}
			else
				throw std::runtime_error("invalid network compoennt name");
		
		}
		else if (rpcName == ts::cmd_hash("@load_final"))
		{
			if (handlers)
				handlers->OnLoadLevelFinal();
		}
		
	}

	
};

Component* Network::GetCompoentFromNetworkId(ts::entity_id id)
{
	return dynamic_cast<NetworkEntityBase*>(ts::begin_entities()[id]);
}

class ServerNetworkInstantor : public ts::rpc_entity
{
public:

	void rpc(RPC_ENTITY_ARGS) override
	{
		if (rpcName == ts::cmd_hash("@resolve_entiyties"))
		{
			for (size_t i = 2; i < ts::entities_count(); i++) 
			{
				ts::rpc_entity* r = ts::begin_entities()[i];
				if (r == nullptr || r == ((ts::rpc_entity*)-1))
					continue;
				NetworkEntityBase* xCmp = dynamic_cast<NetworkEntityBase*>(r);
				if (!xCmp->GetGameObject()->GetPath().IsEmpty())
				{
					ts::package_stream<1400> s;
					s.write_string(xCmp->GetGameObject()->GetPath().c_str());
					s.write_string("@#$%^");
					s.write_object(xCmp->GetGameObject()->GetLocalPosition());
					s.write_object(xCmp->GetGameObject()->GetLocalRotation());
					s.write_string(xCmp->GetNetworkName().c_str());
					s.write_uint32(i);
					call(ts::cmd_hash("@spawn"), calledUser, s);
				}		
			}
			ts::binary_stream s(nullptr, 0, 0);
			call(ts::cmd_hash("@load_final"), calledUser, s);
		}
	}


};
Ref<ts::rpc_entity> instantor;

void Network::Connect(const ts::ip_end_point & to)
{
	try 
	{
		client = new ts::rpc_client(0);
		client->connect(to);
		ts::insert_rpc_client(client.GetPtr());
		ts::clear_entities();
		instantor = new ClientNetworkInstantor();
		instantor->set_id(1);	
		ts::binary_stream e(nullptr, 0, 0);
		instantor->call(ts::cmd_hash("@resolve_entiyties"), e);
	}
	catch (std::exception& ex) 
	{
		client = nullptr;
		throw ex;
	}
}

void Network::Disconent()
{
	client.Realase();
	instantor.Realase();
}

bool Network::IsConnected()
{
	return client;
}

void Network::Listen(int port)
{
	try
	{
		listener = new ts::rpc_listener(0, 100);
		listener->bind(ts::ip_end_point(ts::ip_address_any, port));
		ts::insert_rpc_server(listener.GetPtr());
		ts::clear_entities();
		instantor = new ServerNetworkInstantor();
		instantor->set_id(1);

	}
	catch (std::exception& ex)
	{
		listener = nullptr;
		throw ex;
	}
}

bool Network::IsServer()
{
	return listener;
}

void Network::RegisterNetworkComponent(const String & name, Component::ComponentConstructor constructor)
{
	components[name] = constructor;
}



GameObject* Network::Instantiate(const String & prefabName, const String & networkEntityTypeName, ts::entity_id id, InstantType type)
{
	return Instantiate( prefabName, Vector3::Zero(), Quaternion::Identity(), networkEntityTypeName, id);
}

GameObject* Network::Instantiate(const String& prefabName, const Vector3& position, const Quaternion& rotation,
	const String& networkEntityTypeName, ts::entity_id id, InstantType type)
{
	if (IsServer())
	{
		ts::package_stream<1400> s;
		s.write_string(prefabName.c_str());
		s.write_string("@#$%^");
		s.write_object(position);
		s.write_object(rotation);
		s.write_string(networkEntityTypeName.c_str());
		s.write_uint32(id);
		instantor->call(ts::cmd_hash("@spawn"), s);

		GameObject* o = GameObject::SpawnPrefab(prefabName, "@#$%^", position, rotation);
		auto& cmpcs = components[networkEntityTypeName];
		if (cmpcs)
		{
			NetworkEntityBase* xCmp = dynamic_cast<NetworkEntityBase*>(o->AddComponent(cmpcs));
			if (xCmp == nullptr)
				throw std::runtime_error("invalid network component attachment");
			xCmp->set_id(id);
			xCmp->SetNetworkName(networkEntityTypeName);
		}
		else
			throw std::runtime_error("invalid network compoennt name");
		
		
		return o;
	}
	return nullptr;
}

GameObject* Network::Instantiate(ts::user_id userId, const String& prefabName,
	const String& networkEntityTypeName, ts::entity_id id)
{
	return Instantiate(userId, prefabName, Vector3::Zero(), Quaternion::Identity(), networkEntityTypeName, id);
}

GameObject* Network::Instantiate(ts::user_id userId, const String& prefabName, const Vector3& position, const Quaternion& rotation,
	const String& networkEntityTypeName, ts::entity_id id)
{
	if (IsServer())
	{
		ts::package_stream<1400> s;
		s.write_string(prefabName.c_str());
		s.write_string("@#$%^");
		s.write_object(position);
		s.write_object(rotation);
		s.write_string(networkEntityTypeName.c_str());
		s.write_uint32(id);
		instantor->call(ts::cmd_hash("@spawn"), userId, s);

		GameObject* o = GameObject::SpawnPrefab(prefabName, "@#$%^", position, rotation);
		auto& cmpcs = components[networkEntityTypeName];
		if (cmpcs)
		{
			NetworkEntityBase* xCmp = dynamic_cast<NetworkEntityBase*>(o->AddComponent(cmpcs));
			if (xCmp == nullptr)
				throw std::runtime_error("invalid network component attachment");
			xCmp->set_id(id);
			xCmp->SetNetworkName(networkEntityTypeName);
		}
		return o;
	}
	return nullptr;
}

void Network::Destroy(GameObject * object)
{
}

void Network::ProcessCommands(int maxCmdPercall)
{
	if (listener)
	{
		listener->process_commands(maxCmdPercall);
	}
	else if (client)
	{
		client->process_commands(maxCmdPercall);
	}
}

void Network::Unbound()
{
	listener.Realase();
	instantor.Realase();
}

void Network::UnboundDisconent()
{
	Unbound();
	Disconent();
}
