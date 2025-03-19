#pragma once

#define MAX_SOCKBUF 1024  

enum class PacketType {
    CHAT,
    MOVE
};

// ä�� ��Ŷ ����ü
struct ChatPacket {
    PacketType type;  // �׻� PacketType::CHAT
    char message[MAX_SOCKBUF];
};

// �̵� ��Ŷ ����ü
struct MovePacket {
    PacketType type;  // �׻� PacketType::MOVE
    int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
};
