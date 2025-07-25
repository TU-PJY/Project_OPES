//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#define MAX_LOADSTRING 100

#pragma warning(push)
#pragma warning(disable: 4305)

#include "Config.h"
#include "Objective-D.h"
#include "Framework.h"
#include "GameResource.h"

#include "Scene.h"
#include "CameraUtil.h"
#include "ShaderUtil.h"
#include "MouseUtil.h"
#include "TerrainUtil.h"
#include "OtherPlayerIndicator.h"
#include "Grenade.h"
#include "Turret.h"
#include "PlantMonster.h"

#include <locale>

#include <string>
#include <unordered_set>

//서버
#include <winsock2.h>
//#include <windows.h>
#include <Ws2tcpip.h>  
#include <atlconv.h>
#include <typeinfo>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include<functional>

#include "ModePack.h"


//#include <iostream>
//#include <conio.h>
#include"../Server_OPES/Packet.h"
#include "OtherPlayer.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "msimg32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000


std::atomic<bool> NetRunning{ true };
std::mutex PacketMutex;

SOCKET clientSocket;
bool isRunning = true;
bool enter_room = true;//false;
WSABUF recv_wsabuf[1];
char recv_buffer[MAX_SOCKBUF];
WSAOVERLAPPED recv_over;

struct RecvContext {
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	char buffer[MAX_SOCKBUF];
	std::function<void()> cleanup;
};

bool useServer = true;//클라만 켜서 할땐 false로 바꿔서하기
bool localServer = false; //!useServer;

std::unordered_set<unsigned int> ID_List;

struct SendContext {
	WSAOVERLAPPED overlapped;
	std::function<void()> cleanup;
};

bool IsNewPlayer(unsigned int ID) {
	{
		std::lock_guard<std::mutex> lock(PacketMutex);
		if (!ID_List.contains(ID)) {
			ID_List.insert(ID);
			scene.AddObject(new OtherPlayer(CHARACTER_MG, ID), std::to_string(ID), LAYER_PLAYER);
			auto indicator = scene.Find("otherIndicator");
			if (!indicator)
				GLOBAL.otherIndicator = scene.AddObject(new OtherPlayerIndicator, "otherIndicator", LAYERUI);
			static_cast<GameObject*>(GLOBAL.otherIndicator)->AddPlayer(ID, CHARACTER_MG, std::to_string(ID));
			
			return true;
		}

		return false;
	}
}

void CALLBACK RecvCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
	auto* context = reinterpret_cast<RecvContext*>(p_over);
	if (err != 0 || num_bytes == 0) {
		//std::cout << "[클라이언트] 서버 연결 종료\n";
		isRunning = false;
		if (context && context->cleanup) context->cleanup();
		return;
	}
	//std::cout << "recv\n";
	//std::cout << "[클라이언트] 수신된 데이터 크기: " << num_bytes << " bytes\n";

	PacketType* type = reinterpret_cast<PacketType*>(context->buffer);
//	std::cout << typeid(type).name() << std::endl;

	if (*type == PacketType::MOVE) {
		MovePacket_StoC* movePacket = reinterpret_cast<MovePacket_StoC*>(context->buffer);
		std::cout << "[서버]이동: " << movePacket->id << ":" << movePacket->x << "," << movePacket->y<<"," << movePacket->z << std::endl;
		
		if (auto Found = scene.SearchLayer(LAYER_PLAYER, std::to_string(movePacket->id)); Found)
			Found->InputPosition(XMFLOAT3(movePacket->x, movePacket->y, movePacket->z));
	}

	else if (*type == PacketType::CLEAR_COUNT) {
		ClearCountPacket* Packet = reinterpret_cast<ClearCountPacket*>(context->buffer);
		std::cout << "[서버]CLEAR: " << Packet->PlayerCount <<  std::endl;

		// 현재 접속한 사람이 모두 도착 지점에 들어가면 다음 스테이지로 넘어간다.
		// 자신의 아이디는 ID_List에 없기 때문에 1을 더한 값으로 비교한다.
		if (Packet->PlayerCount >= ID_List.size() + 1) {
			{
				std::lock_guard<std::mutex> lock(PacketMutex);
				scene.SwitchMode(Level2::Start);
			}
		}
		
	}
	else if (*type == PacketType::VIEW_ANGLE) {
		ViewingAnglePacket_StoC* viewAnglePacket = reinterpret_cast<ViewingAnglePacket_StoC*>(context->buffer);
		//std::cout << "[서버]시선: " << viewAnglePacket->id << ":" << viewAnglePacket->x << "," << viewAnglePacket->y << "," << viewAnglePacket->z << std::endl;

		if (auto Found = scene.SearchLayer(LAYER_PLAYER, std::to_string(viewAnglePacket->id)); Found)
			Found->InputRotation(XMFLOAT3(viewAnglePacket->x, viewAnglePacket->y, viewAnglePacket->z));
	}

	else if (*type == PacketType::ANIMATION) {
		AnimationPacket_StoC* aniPacket = reinterpret_cast<AnimationPacket_StoC*>(context->buffer);
		//std::cout << "[서버] 상태: " << aniPacket->id  << ": " << aniPacket->animationType << std::endl;

		if (auto Found = scene.SearchLayer(LAYER_PLAYER, std::to_string(aniPacket->id)); Found)
			Found->InputState(aniPacket->animationType);
	}

	else if (*type == PacketType::MONSTER_STATE) {
		MonsterStatePacket_StoC* packet = reinterpret_cast<MonsterStatePacket_StoC*>(context->buffer);

		std::cout << "몬스터id:" << packet->id << "state: " << packet->state << std::endl;

		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			if (auto monster = scene.SearchLayer(LAYER_MONSTER, std::to_string(packet->id)); monster)
				monster->InputState(packet->state);

		}
	}

	else if (*type == PacketType::MONSTER_MOVE) {
		MonsterMovePacket* packet = reinterpret_cast<MonsterMovePacket*>(context->buffer);

		//std::cout << "MonsterID:" << packet->monsterId << "pID" << packet->playerId << "(" << packet->x << ", " << packet->x << ", " << packet->y << ", "
		//	<< packet->z << " angle:" << packet->angle_y << std::endl;
		XMFLOAT3 recvPosition = { packet->x, packet->y, packet->z };
		float recvRotation = packet->angle_y;
		
		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			if (auto monster = scene.SearchLayer(LAYER_MONSTER, std::to_string(packet->monsterId)); monster) {
				monster->InputPosition(recvPosition);
				monster->InputRotation(recvRotation);
				monster->InputTargetID(packet->playerId);
			}
		}
	}

	else if (*type == PacketType::PTOM_DAMAGE) {
		PtoMDamagePacket* packet = reinterpret_cast<PtoMDamagePacket*>(context->buffer);
		std::cout << "[PTOM_DAMAGE] monsterID: " << packet->monsterID <<"damage: "<< packet->attackHp << std::endl;

		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			if (auto monster = scene.SearchLayer(LAYER_MONSTER, std::to_string(packet->monsterID)); monster)
				monster->InputHP(packet->attackHp);
		}
		
		//처리부분
	}
	
	else if (*type == PacketType::MTOP_DAMAGE) {
		MtoPDamagePacket* packet = reinterpret_cast<MtoPDamagePacket*>(context->buffer);
		std::cout << "[MTOP_DAMAGE] monsterID: " << packet->monsterID << ", playerID: " << packet->playerID << "damage: " << packet->attackHp << std::endl;

		if (packet->playerID == GLOBAL.myID) {
			{
				std::lock_guard<std::mutex> lock(PacketMutex);
				if (auto me = scene.SearchLayer(LAYER_PLAYER, "player"); me)
					me->InputHP(packet->attackHp);
			}
		}

		else {
			{
				std::lock_guard<std::mutex> lock(PacketMutex);
				if (auto other = scene.SearchLayer(LAYER_PLAYER, std::to_string(packet->playerID)); other) {
					other->InputHP(packet->attackHp);
					static_cast<GameObject*>(GLOBAL.otherIndicator)->InputHP(packet->playerID, packet->attackHp);
				}
			}
		}
		
		//처리부분
	}
	else if (*type == PacketType::RANDOM_POSITION) {
		DefenseRandomPacket* packet = reinterpret_cast<DefenseRandomPacket*>(context->buffer);
		std::cout << "[RANDOM_POSITION] ID: " << packet->monsterID << ", pos: (" << packet->x << ", " << packet->z
			<< "), rotY: " << std::endl;

		/*if (auto defendeGenerator = scene.SearchLayer(LAYER1, "defenseModeMonsterGenerator"); defendeGenerator)
			defendeGenerator->InputCreatePositionAndID(packet->x, packet->z, packet->monsterID);*/
		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			xmfloat3 createPosition = xmfloat3(packet->x, 0.0, packet->z);
			scene.AddObject(new PlantMonster(createPosition, packet->monsterID, true), std::to_string(packet->monsterID), LAYER_MONSTER);
		}
		
		//처리부분
	}
	else if (*type == PacketType::ENGINEER_INSTALL) {
		EngineerInstallPacket* packet = reinterpret_cast<EngineerInstallPacket*>(context->buffer);
		std::cout << "[ENGINEER_INSTALL] ID: " << packet->ID << ", type: " << packet->Etype
			<< ", pos: (" << packet->posX << ", " << packet->posY << ", " << packet->posZ
			<< "), rotY: " << packet->rotY << std::endl;

		xmfloat3 installPosition = xmfloat3(packet->posX, packet->posY, packet->posZ);
		float installRotation = packet->rotY;

		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			scene.AddObject(new Turret(installPosition, installRotation, true), "turret", LAYER3);
		}

		//처리부분
	}
	else if (*type == PacketType::ENGINEER_OBJECT) {
		EngineerObjectPacket* packet = reinterpret_cast<EngineerObjectPacket*>(context->buffer);
		std::cout << "[ENGINEER_OBJECT] ID: " << packet->ID << ", hp: " << packet->hp << std::endl;

		//처리부분
	}
	else if (*type == PacketType::CENTER_HP) {
		CenterBuildingPacket* packet = reinterpret_cast<CenterBuildingPacket*>(context->buffer);
		//std::cout << "[CENTER_HP] hp: " << packet->damage << std::endl;

		if (auto centerBuilding = scene.SearchLayer(LAYER1, "center_building"); centerBuilding)
			centerBuilding->InputHP(packet->damage);
		
		//처리부분
	}
	else if (*type == PacketType::GRENADE) {
		GrenadePacket* packet = reinterpret_cast<GrenadePacket*>(context->buffer);
		std::cout << "[GRENADE] pos: (" << packet->posX << ", " << packet->posY << ", " << packet->posZ
			<< "), rot: (" << packet->rotX << ", " << packet->rotY << ", " << packet->rotZ << ")" << std::endl;

		xmfloat3 createPosition = xmfloat3(packet->posX, packet->posY, packet->posZ);
		xmfloat3 rotation = xmfloat3(packet->rotX, packet->rotY, packet->rotZ);

		{
			std::lock_guard<std::mutex> lock(PacketMutex);
			scene.AddObject(new Grenade(createPosition, rotation, true), "grenade", LAYER3);
		}

		//처리부분
	}
	else if (*type == PacketType::PLAYER_ARRIVAL) {
		PlayerArrivalPacket* packet = reinterpret_cast<PlayerArrivalPacket*>(context->buffer);
		std::cout << "[PLAYER_ARRIVAL] playerID: " << packet->playerID << std::endl;

		//처리부분
	}

	else if (*type == PacketType::CHOOSE_JOB) {
		ChooseJobPacket* packet = reinterpret_cast<ChooseJobPacket*>(context->buffer);
		std::cout << "[CHOOSE_JOB] playerID: " << packet->playerID << std::endl;

		//처리부분
	}


	else if (*type == PacketType::ENTER) {
		EnterRoomPacket* EnterPacket = reinterpret_cast<EnterRoomPacket*>(context->buffer);
		std::cout <<"MYID-"<< EnterPacket->myID << " / roomID: " << EnterPacket->roomID << std::endl;///룸 id가 0일시 만들어 진것이 아님 대기중인 상태임
		
		// 전역 변수에 내 ID 저장
		GLOBAL.myID = EnterPacket->myID;
		std::cout << "접속 아이디: " << GLOBAL.myID << std::endl;

		//if (0 != EnterPacket->roomID) {
		//	enter_room = true;
		//}
		//else {
		//	std::cout << "대기중.." << std::endl;
		//}
	}

	else if (*type == PacketType::NEW_CLIENT) {
		NewClientPacket* newClientPacket = reinterpret_cast<NewClientPacket*>(context->buffer);
		std::cout << "[접속] 새로운 클라들어옴! ID: " << newClientPacket->id <<std::endl;
		IsNewPlayer(newClientPacket->id);

	//	std::cout << "ID: " << newClientPacket->id << std::endl;
	
		//player_enter = true;
		//enter_player_id = newClientPacket->id;
	}

	else if (*type == PacketType::EXISTING_CLIENTS) { 
		ExistingClientsDataPacket* pkt = reinterpret_cast<ExistingClientsDataPacket*>(context->buffer);
		for (unsigned int i = 0; i < pkt->count; ++i) {
			auto& info = pkt->clients[i];
			std::cout << "[초기화] 현재 접속한 클라이언트 ID: " << info.id << std::endl;
			IsNewPlayer(info.id);

			// TODO: ID에 해당하는 게임 객체 생성 또는 초기화
		}
	}

	// 다음 수신 요청
	auto* newContext = new RecvContext{};
	ZeroMemory(newContext, sizeof(RecvContext));
	newContext->wsabuf.buf = newContext->buffer;
	newContext->wsabuf.len = sizeof(newContext->buffer);
	newContext->cleanup = [newContext]() { delete newContext; };

	DWORD flags = 0;
	int result = WSARecv(clientSocket, &newContext->wsabuf, 1, NULL, &flags, &newContext->overlapped, RecvCallback);
	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		std::cerr << "[클라이언트] 데이터 수신 오류\n";
		isRunning = false;
		newContext->cleanup();
	}
}

// 데이터 전송 콜백 함수
void CALLBACK SendCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
	if (err != 0 || num_bytes == 0) {
		std::cerr << "[클라이언트] 데이터 전송 실패\n";
		isRunning = false;
	}
	// 동적으로 할당한 오버랩드 해제
	SendContext* context = reinterpret_cast<SendContext*>(p_over);
	if (context && context->cleanup)
		context->cleanup();

//	std::cout << "send\n";
	recv_wsabuf[0].buf = recv_buffer;
	recv_wsabuf[0].len = sizeof(recv_buffer);
	DWORD recv_flag = 0;
	ZeroMemory(&recv_over, sizeof(recv_over));
	WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
}
void NetworkThread(bool localServer, const wchar_t* cmdLine)
{
	char ipStr[64] = { 0 };

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, cmdLine, -1, ipStr, sizeof(ipStr), NULL, NULL);
#else
	strncpy(ipStr, cmdLine, sizeof(ipStr) - 1);
#endif

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	BOOL bNoDelay = TRUE;
	setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(BOOL));

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	if (!localServer)
		inet_pton(AF_INET, ipStr, &serverAddr.sin_addr);
	else
		inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	if (WSAConnect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		std::cerr << "[클라이언트] 서버 연결 실패\n";
		return;
	}

	std::cout << "[클라이언트] 서버에 연결됨\n";

	auto* initialRecv = new RecvContext{};
	ZeroMemory(initialRecv, sizeof(RecvContext));
	initialRecv->wsabuf.buf = initialRecv->buffer;
	initialRecv->wsabuf.len = sizeof(initialRecv->buffer);
	initialRecv->cleanup = [initialRecv]() { delete initialRecv; };

	DWORD flags = 0;
	int result = WSARecv(clientSocket, &initialRecv->wsabuf, 1, NULL, &flags, &initialRecv->overlapped, RecvCallback);
	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		std::cerr << "[클라이언트] 첫 번째 데이터 수신 오류\n";
		initialRecv->cleanup();
		return;
	}


	// 콜백 실행 루프
	while (NetRunning.load()) {
		SleepEx(INFINITE, TRUE); // RecvCallback, SendCallback 실행됨
	}
}
void SendChooseJobPacket(unsigned int playerID,int job) {
	if (enter_room) {
		auto* pkt = new ChooseJobPacket{ PacketType::CHOOSE_JOB, playerID, job};

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(ChooseJobPacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 전송 실패\n";
			context->cleanup(); // 실패 시 즉시 해제
		}
	}
}
// 이동 패킷 전송 함수
void SendMovePacket(float x, float y, float z) {
	if (enter_room) {
		auto* pkt = new MovePacket_CtoS{ PacketType::MOVE, x, y, z };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(MovePacket_CtoS);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 전송 실패\n";
			context->cleanup(); // 실패 시 즉시 해제
		}

		//else
			//std::cout << "전송: " << x << " " << y << " " << z << std::endl;
	}
}
void SendViewingAnglePacket(float x, float y, float z) {
	if (enter_room) {
		auto* pkt = new ViewingAnglePacket_CtoS{ PacketType::VIEW_ANGLE, x, y, z };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(ViewingAnglePacket_CtoS);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 회전 패킷 전송 오류\n";
			context->cleanup();
		}
	}
}

void SendAnimaionPacket(unsigned short playerState) {
	if (enter_room) {
		auto* pkt = new AnimationPacket_CtoS{ PacketType::ANIMATION, playerState };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(AnimationPacket_CtoS);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 애니메이션 패킷 전송 오류\n";
			context->cleanup();
		}
	}
}
void SendMonstertypePacket(unsigned int monsterType, unsigned int monsterState, unsigned int id) {
	if (enter_room) {
		auto* pkt = new MonsterStatePacket_CtoS{ PacketType::MONSTER_STATE, monsterType, monsterState, id };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(MonsterStatePacket_CtoS);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 몬스터 상태 패킷 전송 오류\n";
			context->cleanup();
		}
	}
}

void SendMonsterMovePacket(float x, float y, float z, float angle, unsigned int monsterId, unsigned int playerId) {
	if (enter_room) {
		auto* pkt = new MonsterMovePacket{ PacketType::MONSTER_MOVE, monsterId,playerId,x,y,z,angle };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(MonsterMovePacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 몬스터 이동 패킷 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendPtoMDamagePacket(unsigned int monsterID, int attackHp) {
	if (enter_room) {
		auto* pkt = new PtoMDamagePacket{ PacketType::PTOM_DAMAGE, monsterID, attackHp };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(PtoMDamagePacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] PTOM_DAMAGE 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendMtoPDamagePacket(unsigned int playerID, unsigned int monsterID, int attackHp) {
	if (enter_room) {
		auto* pkt = new MtoPDamagePacket{ PacketType::MTOP_DAMAGE, playerID, monsterID, attackHp };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(MtoPDamagePacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] MTOP_DAMAGE 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendEngineerInstallPacket(int type, unsigned int ID, float rotY, float posX, float posY, float posZ) {
	if (enter_room) {
		auto* pkt = new EngineerInstallPacket{ PacketType::ENGINEER_INSTALL, type, ID,  rotY,posX, posY, posZ };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(EngineerInstallPacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 설치 패킷 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendEngineerObjectPacket(unsigned int ID, int hp) {
	if (enter_room) {
		auto* pkt = new EngineerObjectPacket{ PacketType::ENGINEER_OBJECT, ID, hp };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(EngineerObjectPacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 엔지니어 객체 패킷 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendCenterBuildingPacket(int hp) {
	if (enter_room) {
		auto* pkt = new CenterBuildingPacket{ PacketType::CENTER_HP, hp };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(CenterBuildingPacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 센터 체력 패킷 전송 실패\n";
			context->cleanup();
		}
	}
}

void SendGrenadePacket(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) {
	if (enter_room) {
		auto* pkt = new GrenadePacket{ PacketType::GRENADE, posX, posY, posZ, rotX, rotY, rotZ };

		auto* context = new SendContext{};
		ZeroMemory(context, sizeof(SendContext));

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(pkt);
		wsaBuf.len = sizeof(GrenadePacket);

		context->cleanup = [pkt, context]() {
			delete pkt;
			delete context;
			};

		DWORD bytesSent = 0;
		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, &context->overlapped, SendCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 수류탄 패킷 전송 실패\n";
			context->cleanup();
		}
	}
}



// 채팅 패킷 전송 함수
//void SendChatPacket(const char* message) {
//	if (enter_room) {
//		ChatPacket_CtoS chatPacket = {};
//		chatPacket.type = PacketType::CHAT;
//		int msg_size = strlen(message);
//		memcpy(chatPacket.message, message, msg_size);
//
//		WSABUF wsaBuf[1];
//		wsaBuf[0].buf = reinterpret_cast<char*>(&chatPacket);
//		wsaBuf[0].len = sizeof(PacketType) + msg_size;
//
//		WSAOVERLAPPED send_over = { 0 };
//
//		int result = WSASend(clientSocket, wsaBuf, 1, NULL, 0, &send_over, SendCallback);//동기 io
//		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
//			std::cerr << "[클라이언트] 채팅 패킷 전송 오류\n";
//		}
//	}
//}


int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

Framework framework;

// 프레임워크, 카메라, 마우스유틸, 터레인 유틸은 전역 객체이다. 즉, 해당되는 헤더파일만 포함하면 어디서든지 사용 가능하다
Scene scene;
Camera camera;
MouseUtil mouse;

// 전역 시스템
DeviceSystem GlobalSystem;

// 전역 커맨드 리스트
ID3D12GraphicsCommandList* GlobalCommandList;

// 오브젝트 쉐이더 루트 시그니처
ID3D12RootSignature* ObjectShaderRootSignature;

// 이미지 쉐이더 루트 시그니처
ID3D12RootSignature* ImageShaderRootSignature;

// 선 쉐이더 루트 시그니처
ID3D12RootSignature* LineShaderRootSignature;

// 바운드박스 루트 시그니처
ID3D12RootSignature* BoundboxShaderRootSignature;

// 전역 HWND
HWND GlobalHWND;

HINSTANCE						AppInstance;
TCHAR							Title[MAX_LOADSTRING];
TCHAR							WindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG Messege;
	HACCEL AccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, Title, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT045, WindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return(FALSE);

	AccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT045));
	std::thread netThread;

	// 전역 서버 사용 여부 저장

	if (useServer) {
		netThread = std::thread(NetworkThread, localServer, lpCmdLine);
		GLOBAL.useServer = true;
	}
	else
		GLOBAL.useServer = false;
//	if (useServer) {
//
//		//if (lpCmdLine == NULL || _tcslen(lpCmdLine) == 0) {
//		//	MessageBox(NULL, _T("서버 IP 주소를 인자로 입력하세요.\n예: program.exe 127.0.0.1"), _T("오류"), MB_OK);
//		//	return -1;
//		//}
//
//		char ipStr[64] = { 0 };
//
//#ifdef UNICODE
//		// 유니코드 → 멀티바이트 변환
//		WideCharToMultiByte(CP_ACP, 0, lpCmdLine, -1, ipStr, sizeof(ipStr), NULL, NULL);
//#else
//		// 멀티바이트는 바로 복사
//		strncpy(ipStr, lpCmdLine, sizeof(ipStr) - 1);
//#endif
//		// 윈속 초기화
//		WSADATA wsaData;
//		WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//		// 소켓 생성 및 서버 연결
//		clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
//
//		// NAGLE 비활성화
//		BOOL bNoDelay = TRUE;
//		int result = setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(BOOL));
//		if (result == SOCKET_ERROR) {
//			std::cerr << "[클라이언트] setsockopt(TCP_NODELAY) 실패: " << WSAGetLastError() << std::endl;
//		}
//		///
//
//		SOCKADDR_IN serverAddr;
//		serverAddr.sin_family = AF_INET;
//		serverAddr.sin_port = htons(SERVER_PORT);
//
//		if(!localServer)
//			inet_pton(AF_INET, ipStr, &serverAddr.sin_addr);//cmd에서 ip입력할때 
//		else
//			inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);//한 컴퓨터에서 실행할때
//
//		//std::cout << ipStr << std::endl;
//		if (WSAConnect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
//			std::cerr << "[클라이언트] 서버 연결 실패\n";
//			return -1;
//		}
//		std::cout << "[클라이언트] 서버에 연결됨\n";
//
//		// 첫 번째 데이터 수신 요청
//		recv_wsabuf[0].buf = recv_buffer;
//		recv_wsabuf[0].len = sizeof(recv_buffer);
//		DWORD recv_flag = 0;
//		ZeroMemory(&recv_over, sizeof(recv_over));
//
//		int __result = WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
//		if (__result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
//			std::cerr << "[클라이언트] 첫 번째 데이터 수신 오류\n";
//			return -1;
//		}
//	}

	while (true) {
		if (::PeekMessage(&Messege, NULL, 0, 0, PM_REMOVE)) {
			if (Messege.message == WM_QUIT)
				break;

			if (!::TranslateAccelerator(Messege.hwnd, AccelTable, &Messege)) {
				::TranslateMessage(&Messege);
				::DispatchMessage(&Messege);
			}
		}
		else {
			framework.Update();
			//SleepEx(0, TRUE);
			//Sleep(0);
		}
		// 비동기 I/O 콜백 실행
		
	}

	// _tWinMain() 루프를 빠져나온 직후 ---------
	NetRunning = false;                 // ① 네트워크 루프 정지 플래그

	shutdown(clientSocket, SD_BOTH);    // ② 진행 중 I/O 모두 취소
	closesocket(clientSocket);          //    → SleepEx(INFINITE,TRUE) 깨움

	if (netThread.joinable())           // ③ 즉시 조인 가능
		netThread.join();

	WSACleanup();                       // ④ Winsock 해제
	framework.Destroy();                // ⑤ DirectX·리소스 정리
	return static_cast<int>(Messege.wParam);


	//return((int)Messege.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX Wcex;

	Wcex.cbSize = sizeof(WNDCLASSEX);

	Wcex.style = CS_HREDRAW | CS_VREDRAW;
	Wcex.lpfnWndProc = WndProc;
	Wcex.cbClsExtra = 0;
	Wcex.cbWndExtra = 0;
	Wcex.hInstance = hInstance;
	Wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT045));
	Wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	Wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	Wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_LABPROJECT045);
	Wcex.lpszClassName = WindowClass;
	Wcex.hIconSm = ::LoadIcon(Wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&Wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	AppInstance = hInstance;
	DWORD WindowStyle{};
	RECT Rect{};

	Rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	WindowStyle = WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	AdjustWindowRect(&Rect, WindowStyle, FALSE);

	HWND MainWnd = CreateWindow(
		WindowClass,
		Title,
		WindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!MainWnd)
		return(FALSE);

	framework.Create(hInstance, MainWnd);

	::ShowWindow(MainWnd, nCmdShow);
	::UpdateWindow(MainWnd);

	// Config.h에서 전체화면 모드를 활성화 했을 경우 바로 전체화면으로 전환된다
	if (!START_WITH_FULL_SCREEN)
		framework.SwitchToWindowMode(MainWnd);

	return(TRUE);
}

// F11키를 누를 시 창모드<->전체화면 모드 전환이 가능하다. 조작키는 비활성화 하거나 바꿀 수 있다.
// ChangeSwapChainState()를 사용하지 않는 이유는 해당 함수 사용 시 뷰포트의 선명도가 떨어지는 문제가 있기 때문이다.
// 창 크기를 늘려 제목표시줄을 없애는 방식으로 전체화면을 구현한다.
void DisplayStateChanger(HWND hWnd, UINT nMessageID, WPARAM wParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_F11:
			if (framework.FullScreenState)
				framework.SwitchToWindowMode(hWnd);
			else
				framework.SwitchToFullscreenMode(hWnd);
			break;
		}
		break;
	}
}

// 이 함수의 내용들은 수정할 필요 없다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	PAINTSTRUCT PaintStruct;
	HDC Hdc;
	GlobalHWND = hWnd;

	DisplayStateChanger(hWnd, nMessageID, wParam);

	switch (nMessageID) {
		// 윈도우 사이즈 변경이 감지되면 카메라 행렬을 새로 업데이트한다.
	case WM_SIZE:
		SCREEN_WIDTH = LOWORD(lParam);
		SCREEN_HEIGHT = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
		scene.InputMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN: case WM_KEYUP:
		scene.InputKeyMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		scene.InputMouseMotionMessage(hWnd);
		break;

	case WM_ACTIVATE:
		framework.WindowsMessageFunc(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId) {
		case IDM_ABOUT:
			::DialogBox(AppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;

		default:
			return(::DefWindowProc(hWnd, nMessageID, wParam, lParam));
		}

		break;

	case WM_PAINT:
		Hdc = ::BeginPaint(hWnd, &PaintStruct);
		EndPaint(hWnd, &PaintStruct);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	default:
		return(::DefWindowProc(hWnd, nMessageID, wParam, lParam));
	}

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (nMessageID) {
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}

	return((INT_PTR)FALSE);
}

// 뷰포트 배경색을 변경한다. flaot 값 또는 rgb값을 사용할 수 있다.
void SetBackgroundColorRGB(int R, int G, int B) {
	framework.BackgroundColor.x = 1.0 / 255.0 * float(R);
	framework.BackgroundColor.y = 1.0 / 255.0 * float(G);
	framework.BackgroundColor.z = 1.0 / 255.0 * float(B);
}

void SetBackgroundColor(float R, float G, float B) {
	framework.BackgroundColor.x = float(R);
	framework.BackgroundColor.y = float(G);
	framework.BackgroundColor.z = float(B);
}