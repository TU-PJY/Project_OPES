#include "CrossHair.h"


CrossHair::CrossHair() {
	crosshair.SetColor(1.0, 0.0, 0.0);
}

// �÷��̾� �� �߻� �� �ݵ� �� �ο�
void CrossHair::InputRecoil(float Value) {
	recoil += Value;
}

void CrossHair::Update(float Delta) {
	// �ݵ��� �׻� 0.0�� ���� ���� �����Ѵ�
	recoil = std::lerp(recoil, 0.0, Delta * 5.0);
}

void CrossHair::Render() {
	// ũ�ν���� ������
	// �ݵ� ���� ���� ���� ũ�ν� ����� ������ �о����� �ȴ� -> ��Ȯ���� �������� �ȴ�
	//  ���� ���� ��
	crosshair.Draw(-0.1 - recoil, 0.0, -0.03 - recoil, 0.0, 0.01);

	// ������ ���� ��
	crosshair.Draw(0.03 + recoil, 0.0, 0.1 + recoil, 0.0, 0.01);

	// �Ʒ� ���� ��
	crosshair.Draw(0.0, -0.1 - recoil, 0.0, -0.03 - recoil, 0.01);

	// �� ���� ��
	crosshair.Draw(0.0, 0.03 + recoil, 0.0, 0.1 + recoil, 0.01);
}