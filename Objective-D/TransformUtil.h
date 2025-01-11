#pragma once
#include "DirectX_3D.h"

namespace Transform {
	void Identity(XMFLOAT4X4& Matrix);
	void Move(XMFLOAT4X4& Matrix, float X, float Y, float Z);
	void Move(XMFLOAT4X4& Matrix, XMFLOAT3& Value);
	void Rotate(XMFLOAT4X4& Matrix, float RotationValue, float X, float Y, float Z);
	void Rotate(XMFLOAT4X4& Matrix, float RotationX, float RotationY, float RotationZ);
	void Rotate(XMFLOAT4X4& Matrix, XMFLOAT3& Rotation);
	void RotateYX(XMFLOAT4X4& Matrix, float RotationX, float RotationY);
	void Scale(XMFLOAT4X4& Matrix, float X, float Y, float Z);
	void Scale(XMFLOAT4X4& Matrix, XMFLOAT3& Size);
	void ImageAspect(XMFLOAT4X4& Matrix, int Width, int Height);
	void Move2D(XMFLOAT4X4& Matrix, float X, float Y);
	void Rotate2D(XMFLOAT4X4& Matrix, float Rotation);
	void Scale2D(XMFLOAT4X4& Matrix, float X, float Y);
}