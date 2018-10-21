#include "NetworkEntityBase.h"

NetworkEntityBase::NetworkEntityBase() :
	handler([&]()
{
	ts::package_stream<100> s;
	s.write_object(GetGameObject()->GetLocalPosition());
	s.write_object(GetGameObject()->GetLocalRotation());
	s.write_int8(physStateSync ? 1 : 0);
	if (physStateSync)
	{
		RigidBody* r = GetGameObject()->GetComponent<RigidBody>();
		s.write_object(r->GetLinearVelocity());
		s.write_object(r->GetAngularVelocity());
	}
	OnStruct(s);
	call_unsafe(cmdStateHash, s);
}, false)
{}

void NetworkEntityBase::SetNetworkName(const String & id)
{
	uperTypeName = id;
}

String const & NetworkEntityBase::GetNetworkName() const
{
	return uperTypeName;
}

void NetworkEntityBase::RedirectIfServer(uint32_t rcpName, ts::binary_stream & str)
{
	if (Network::IsServer())
	{
		ts::binary_stream bin(str.data(), str.length(), str.length());
		call_unsafe(rcpName, bin);
	}
}
void NetworkEntityBase::RedirectIfServerSafability(uint32_t rcpName, ts::binary_stream & str)
{
	if (Network::IsServer())
	{
		ts::binary_stream bin(str.data(), str.length(), str.length());
		call(rcpName, bin);
	}
}
void NetworkEntityBase::rpc(RPC_ENTITY_ARGS)
{
	if (rpcName == cmdStateHash)
	{
		/*Это нужно чтобы переробсить комманду всем остальным*/

		RedirectIfServer(rpcName, arguments);
		if (!syncEnabled)
		{
			GetGameObject()->SetLocalPosition(arguments.read_object<Vector3>());
			GetGameObject()->SetLocalRotation(arguments.read_object<Quaternion>());
			if (arguments.read_int8() == 1)
			{
				RigidBody* r = GetGameObject()->GetComponent<RigidBody>();
				r->SetLinearVelocity(arguments.read_object<Vector3>());
				r->SetAngularVelocity(arguments.read_object<Vector3>());
			}
			OnDestruct(arguments);
		}

	}
	else if (rpcName == cmdDeleteHash)
	{
		RedirectIfServerSafability(rpcName, arguments);
		if (!syncEnabled)
			delete GetGameObject();

	}

}

void NetworkEntityBase::OnDestruct(ts::binary_stream & s)
{

}

void NetworkEntityBase::OnStruct(ts::binary_stream & s)
{

}

void NetworkEntityBase::Destroy()
{
	GetGameObject()->GetHandlers()->OnTransformChanged -= &handler;
	String gs = GetGameObject()->GetPath();
	if (!gs.IsEmpty() && syncEnabled)
	{
		ts::package_stream<1100> s;
		s.write_string(gs.c_str());
		call(cmdDeleteHash, s);
	}
}

ts::entity_id NetworkEntityBase::AllocateId()
{
	return ts::rpc_entity::allocate_id();
}

/*
-1 если хотите отключить синхронизацию
0 если хотите включить синхронизацию по изменению состояния
всё что больше 0 задаст фиксированый тикрейт
*/

void NetworkEntityBase::PhysicUpdate()
{
	if (!syncEnabled)
		return;
	double delta = Time::GetTotalTime() - lastUpdateTime;
	if (invTickRate > 0.0 && delta >= invTickRate)
	{
		lastUpdateTime = Time::GetTotalTime();
		ts::package_stream<100> s;
		s.write_object(GetGameObject()->GetLocalPosition());
		s.write_object(GetGameObject()->GetLocalRotation());
		s.write_int8(physStateSync ? 1 : 0);
		if (physStateSync)
		{
			RigidBody* r = GetGameObject()->GetComponent<RigidBody>();
			s.write_object(r->GetLinearVelocity());
			s.write_object(r->GetAngularVelocity());
		}
		OnStruct(s);
		call_unsafe(cmdStateHash, s);
	}

}

void NetworkEntityBase::EnableVelocitySync(bool value)
{
	physStateSync = value;
}

void NetworkEntityBase::SetStateSync(int stickRate)
{
	if (stickRate == -1)
	{
		SetEnabled(false);
		GetGameObject()->GetHandlers()->OnTransformChanged -= &handler;
		syncEnabled = false;
		return;
	}
	invTickRate = 1.0 / stickRate;
	if (stickRate == 0)
	{
		SetEnabled(false);
		GetGameObject()->GetHandlers()->OnTransformChanged += &handler;
	}
	else
	{
		SetEnabled(true);
		GetGameObject()->GetHandlers()->OnTransformChanged -= &handler;

	}
	syncEnabled = true;
}
