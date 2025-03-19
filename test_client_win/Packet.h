#pragma once

#define MAX_SOCKBUF 1024  

enum class PacketType {
    CHAT,
    MOVE
};

// 채팅 패킷 구조체
struct ChatPacket {
    PacketType type;  // 항상 PacketType::CHAT
    char message[MAX_SOCKBUF];
};

// 이동 패킷 구조체
struct MovePacket {
    PacketType type;  // 항상 PacketType::MOVE
    int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
};
