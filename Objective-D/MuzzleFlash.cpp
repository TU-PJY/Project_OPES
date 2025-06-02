#include "MuzzleFlash.h"
#include "CameraUtil.h"
#include <random>

BloodParticle::BloodParticle() {
    positions = CreatePositions(30);
    Math::InitVector(vec);
}

std::vector<XMFLOAT3> BloodParticle::CreatePositions(int count)
{
    std::vector<XMFLOAT3> positions;
    positions.reserve(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, XM_2PI);
    std::uniform_real_distribution<float> coreDist(0.0f, 0.3f); // ���� �߽�
    std::uniform_real_distribution<float> tailDist(0.3f, 0.6f); // ���� ���� ����
    std::uniform_real_distribution<float> heightDist(0.0f, 0.2f);
    std::uniform_real_distribution<float> offsetY(-0.1f, 0.05f); // �߽� ��¦ �Ʒ�

    for (int i = 0; i < count; i++)
    {
        float angle = angleDist(gen);
        float radius = (i < count * 0.7f) ? coreDist(gen) : tailDist(gen); // 70�� �߽�, �������� ����
        float x = cosf(angle) * radius;
        float y = sinf(angle) * radius + offsetY(gen);

        // ���� ������ ���� �ö�
        if (i >= count * 0.7f)
            y += heightDist(gen); // ���� ��ƼŬ�� ���� ���� �� �̵�

        float z = 1.0f; // ���� ���� ���̴� ��� ���� (������� ���ǹ�)

        positions.emplace_back(x, y, z);
    }

    return positions;
}

std::vector<XMFLOAT3> BloodParticle::GetPositions() {
    return positions;
}

Vector BloodParticle::GetVector() {
    return vec;
}
