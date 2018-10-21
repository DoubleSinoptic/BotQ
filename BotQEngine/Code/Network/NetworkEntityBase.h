#pragma once

#include "Network.h"
#include "tinyrpc.h"
#include "../Component.h"
#include "../GameObject.h"
#include "../Time.hpp"
#include "../PhysicsComponents/RigidBody.h"

/*
���� �������� ���� ������� �� ������
*/
class SGE_EXPORT NetworkEntityBase : public Component, public ts::rpc_entity
{
	String uperTypeName;
	bool physStateSync = false;
	double lastUpdateTime = 0.0;
	int syncEnabled = false;
	double invTickRate;
/*
�� ���� ts::cmd_hash ������� ���� constexpr.
�� ��� �� ������... MSVC �� ����� ��������� �� �������� � ������ �++14 ... 
� GCC �����! ������....
�������� ���� ����� ������� cmd_hash �� ���������
-std=c++17
*/
	const int cmdStateHash = ts::cmd_hash("@sync");
	const int cmdDeleteHash = ts::cmd_hash("@delete");
	EventHandler<> handler;
public:
	void SetNetworkName(const String& id);

	String const & GetNetworkName() const;

	void RedirectIfServer(uint32_t rcpName, ts::binary_stream& str);
	void RedirectIfServerSafability(uint32_t rcpName, ts::binary_stream & str);
	/*
	���������� ����� �������� ������ � tinyrpc 
	���������� ��� entity id ��������� � ���� ������
	*/
	virtual void rpc(RPC_ENTITY_ARGS) override;;

	/*
	���������� � ������ ����� �� ���������� ������
	� realtime �������������.
	*/
	virtual void OnDestruct(ts::binary_stream& s);

	/*
	���������� � ��������� ������ 
	����� ��� �������� �� ������
	� realtime �������������.
	*/
	virtual void OnStruct(ts::binary_stream& s);

	virtual void Destroy();

	static ts::entity_id AllocateId();

	NetworkEntityBase();

	virtual void PhysicUpdate() override;

	/*
	��������� realtime �������������� 
	������� ������� � ������� ��������
	����� �������� ��������� �������������
	���� ���� RigidBody
	*/
	void EnableVelocitySync(bool value = true);

	/*
	-1 ���� ������ ��������� realtime �������������
	0 ���� ������ �������� realtime ������������� �� ��������� ���������
	�� ��� ������ 0 ������ ������������ �������
	*/
	void SetStateSync(int stickRate = 8);

};