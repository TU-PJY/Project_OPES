// TimerService.cpp
#include "Timer.h"
#include "IOCompletionPort.h"
#include <chrono>
#include <mutex>

using namespace std::chrono;

TimerService::TimerService(IOCompletionPort& iocp) : iocp(iocp) {}
TimerService::~TimerService() { stop(); }

void TimerService::start() {
    if (running.exchange(true)) return;
    timerThread = std::thread([this] { loop(); });
}
void TimerService::stop() {
    if (!running.exchange(false)) return;
    if (timerThread.joinable()) timerThread.join();
}

void TimerService::resetRoom(unsigned roomID, int seconds) {
    std::lock_guard<std::mutex> g(iocp.RoomMapMutex());
    auto& rooms = iocp.Rooms();
    auto it = rooms.find((int)roomID);
    if (it == rooms.end()) return;

    auto& r = it->second;
    r.timerRemainSec = seconds;
    r.timerLastTick = steady_clock::now();
    r.timerRunning = true;

    iocp.BroadcastStageTimer(roomID, r.stageState, r.timerRemainSec, /*running=*/true);
}

void TimerService::stopRoom(unsigned roomID) {
    std::lock_guard<std::mutex> g(iocp.RoomMapMutex());
    auto& rooms = iocp.Rooms();
    auto it = rooms.find((int)roomID);
    if (it == rooms.end()) return;

    auto& r = it->second;
    if (!r.timerRunning) return;
    r.timerRunning = false;

    iocp.BroadcastStageTimer(roomID, r.stageState, r.timerRemainSec, /*running=*/false);
}

void TimerService::loop() {
    while (running.load() && iocp.RunningFlag().load()) {
        auto now = steady_clock::now() + std::chrono::seconds(1);

        {
            std::lock_guard<std::mutex> g(iocp.RoomMapMutex());
            auto& rooms = iocp.Rooms();

            for (auto& kv : rooms) {
                auto& r = kv.second;
                if (!r.timerRunning) continue;

                while (now - r.timerLastTick >= 1s && r.timerRunning) {
                    r.timerLastTick += 1s;

                    if (r.timerRemainSec > 0) {
                        --r.timerRemainSec;
                        iocp.BroadcastStageTimer(r.roomID, r.stageState, r.timerRemainSec, /*running=*/true);
                    }
                    if (r.timerRemainSec == 0) {
                        r.timerRunning = false;
                        iocp.BroadcastStageTimer(r.roomID, r.stageState, 0, /*running=*/false);
                        break;
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
