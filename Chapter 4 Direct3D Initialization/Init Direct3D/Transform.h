#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	DirectX::XMFLOAT3 vSca;		// Scaling						

	DirectX::XMFLOAT3 vDir;		// Z : Roll
	DirectX::XMFLOAT3 vRight;	// X : Pitch
	DirectX::XMFLOAT3 vUp;		// Y : Yaw

	DirectX::XMFLOAT3 vPos;		// Position						

	DirectX::XMFLOAT4 qRot;		// Rotation par Quaternion
	DirectX::XMFLOAT4X4 mRot;	// Rotation par Matrix pas nécessaire	

	DirectX::XMFLOAT4X4 mMatrix; // Matrix final

	Transform();
	virtual ~Transform();
	void Identity();
	void UpdateMatrix();
	void RotateWorld(float roll, float pitch, float yaw);
	void Rotate(float roll, float pitch, float yaw);
	void RotateYaw(float angle);
	void RotateRoll(float angle);
	void RotatePitch(float angle);

	void SetScaling(float X, float Y, float Z);
	void GetPitchAndYaw(float& x, float& y);

	void UpdateRotationFromMatrix();

	void Translation(bool roll, bool pitch, bool yaw, float speed, float dir, float dt);
	void TranslationX(float speed, float dir, float dt);
	void TranslationY(float speed, float dir, float dt);
	void TranslationZ(float speed, float dir, float dt);
	DirectX::XMMATRIX GetMatrix() const;


	void SetPosition(float X, float Y, float Z);
};