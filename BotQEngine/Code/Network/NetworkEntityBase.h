#pragma once

#include "Network.h"
#include "tinyrpc.h"
#include "../Component.h"
#include "../GameObject.h"
#include "../Time.hpp"
#include "../PhysicsComponents/RigidBody.h"

/*
Буду гордится этим классом до пенсии
*/
class SGE_EXPORT NetworkEntityBase : public Component, public ts::rpc_entity
{
	String uperTypeName;
	bool physStateSync = false;
	double lastUpdateTime = 0.0;
	int syncEnabled = false;
	double invTickRate;
/*
По идее ts::cmd_hash долженб быть constexpr.
но вот не задача... MSVC не может проходить по строчкам в режиме с++14 ... 
а GCC может! ХИХИХИ....
поэхтому если хотим быстрый cmd_hash то подрубаем
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
	Вызывается когда приходят данные с tinyrpc 
	соединения для entity id заданного в этом классе
	*/
	virtual void rpc(RPC_ENTITY_ARGS) override;;

	/*
	Вызывается у донора когда он отправляет данные
	в realtime синхронизации.
	*/
	virtual void OnDestruct(ts::binary_stream& s);

	/*
	Вызывается у аццептора данных 
	когда они приходят от донора
	в realtime синхронизации.
	*/
	virtual void OnStruct(ts::binary_stream& s);

	virtual void Destroy();

	static ts::entity_id AllocateId();

	NetworkEntityBase();

	virtual void PhysicUpdate() override;

	/*
	Дополняет realtime синхронизациию 
	данными линеной и угловой скорости
	чтобы улучшить плавность синхронизации
	если есть RigidBody
	*/
	void EnableVelocitySync(bool value = true);

	/*
	-1 если хотите отключить realtime синхронизацию
	0 если хотите включить realtime синхронизацию по изменению состояния
	всё что больше 0 задаст фиксированый тикрейт
	*/
	void SetStateSync(int stickRate = 8);

};