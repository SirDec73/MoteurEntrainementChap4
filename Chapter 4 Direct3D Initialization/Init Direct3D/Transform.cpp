#include "Transform.h"
#include <iostream>

float convertRadius = DirectX::XM_PI / 180.0f;

Transform::Transform() {
	Identity();
}

Transform::~Transform() {

}

void Transform::Identity() {

	vSca = { 1.0f, 1.0f, 1.0f };

	vDir = { 0.0f, 0.0f, 1.0f };
	vRight = { 1.0f, 0.0f, 0.0f };
	vUp = { 0.0f, 1.0f, 0.0f };

	vPos = { 0.0f, 0.0f, 0.0f };

	qRot = { 0.0f, 0.0f, 0.0f, 1.0f };

	memset(&mRot, 0, sizeof(DirectX::XMFLOAT4X4));

	memset(&mMatrix, 0, sizeof(DirectX::XMFLOAT4X4));

	for (int i = 0; i < 4; i++) {
		//mRot.m[i][i] = 1.0f;
		mMatrix.m[i][i] = 1.0f;
	}
}

void Transform::UpdateMatrix() {
	DirectX::XMMATRIX tmpMatrix = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX mmRot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qRot));

	DirectX::XMMATRIX mSca = DirectX::XMMatrixScaling(vSca.x, vSca.y, vSca.z);
	DirectX::XMMATRIX mPos = DirectX::XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

	tmpMatrix = DirectX::XMMatrixMultiply(mmRot, mSca);
	tmpMatrix = DirectX::XMMatrixMultiply(tmpMatrix, mPos);

	DirectX::XMStoreFloat4x4(&mMatrix, tmpMatrix);
}

void Transform::RotateWorld(float roll, float pitch, float yaw) {

	float rollRad = roll * convertRadius;
	float pitchRad = pitch * convertRadius;
	float yawRad = yaw * convertRadius;

	//Transform identity;
	//identity.Identity();

	DirectX::XMVECTOR qRotRPY = DirectX::XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);

	DirectX::XMStoreFloat4(&qRot, qRotRPY);
	DirectX::XMStoreFloat4x4(&mRot, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qRot)));

	UpdateRotationFromMatrix();
}

void Transform::Rotate(float roll, float pitch, float yaw) {

	float rollRad = roll * convertRadius;
	float pitchRad = pitch * convertRadius;
	float yawRad = yaw * convertRadius;

	DirectX::XMVECTOR qRotRPY;
	qRotRPY = DirectX::XMQuaternionIdentity();

	DirectX::XMVECTOR qRoll  = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vDir), rollRad);
	DirectX::XMVECTOR qPitch = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vRight), pitchRad);
	DirectX::XMVECTOR qYaw   = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vUp), yawRad);


	qRotRPY = DirectX::XMQuaternionMultiply(qRoll,  qRotRPY);
	qRotRPY = DirectX::XMQuaternionMultiply(qPitch, qRotRPY);
	qRotRPY = DirectX::XMQuaternionMultiply(qYaw,   qRotRPY);
	qRotRPY = DirectX::XMQuaternionNormalize(qRotRPY);

	DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(qRotRPY,DirectX::XMLoadFloat4(&qRot));

	DirectX::XMStoreFloat4(&qRot, result);
	DirectX::XMStoreFloat4x4(&mRot, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qRot)));
	
	UpdateRotationFromMatrix();

}

void Transform::UpdateRotationFromMatrix() {
	vRight = { mRot._11, mRot._12, mRot._13 };
	vUp = { mRot._21, mRot._22, mRot._23 };
	vDir = { mRot._31, mRot._32, mRot._33 };

	/*DirectX::XMStoreFloat3(&vRight, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vRight)));
	DirectX::XMStoreFloat3(&vUp, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vUp)));
	DirectX::XMStoreFloat3(&vDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vDir)));*/
}

//void Transform::UpdateRotationFromMatrix() {
//
//	vRight.x = mRot._11;
//	vRight.y = mRot._12;
//	vRight.z = mRot._13;
//	vUp.x = mRot._21;
//	vUp.y = mRot._22;
//	vUp.z = mRot._23;
//	vDir.x = mRot._31;
//	vDir.y = mRot._32;
//	vDir.z = mRot._33;
//}

void Transform::RotateYaw(float angle) {
	Rotate(0.0f, 0.0f, angle);
}

void Transform::RotateRoll(float angle) {
	Rotate(angle, 0.0f, 0.0f);
}

void Transform::RotatePitch(float angle) {
	Rotate(0.0f, angle, 0.0f);
}

void Transform::SetScaling(float X, float Y, float Z) {
	vSca.x = X;
	vSca.y = Y;
	vSca.z = Z;

	UpdateMatrix();
}

void Transform::GetPitchAndYaw(float& X, float& Y) {
	Y = atan2(vDir.x, vDir.z);

	X = asin(vDir.y);

	Y = DirectX::XMConvertToDegrees(Y);
	X = DirectX::XMConvertToDegrees(X);
}


void Transform::Translation(bool roll, bool pitch, bool yaw, float speed, float dir, float dt) {
	DirectX::XMVECTOR vNewPosition = DirectX::XMLoadFloat3(&vPos);

	if (roll) vNewPosition = DirectX::XMVectorAdd(vNewPosition, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vDir), (speed * dir * dt)));
	if (pitch) vNewPosition = DirectX::XMVectorAdd(vNewPosition, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vRight), (speed * dir * dt)));
	if (yaw) vNewPosition = DirectX::XMVectorAdd(vNewPosition, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vUp), (speed * dir * dt)));

	DirectX::XMStoreFloat3(&vPos, vNewPosition);
}

void Transform::TranslationX(float speed, float dir, float dt) {
	Translation(false, true, false, speed, dir, dt);
}
void Transform::TranslationY(float speed, float dir, float dt) {
	Translation(false, false, true, speed, dir, dt);
}
void Transform::TranslationZ(float speed, float dir, float dt) {
	Translation(true, false, false, speed, dir, dt);
}

void Transform::SetPosition(float X, float Y, float Z)
{
	vPos.x = X;
	vPos.y = Y;
	vPos.z = Z;

	UpdateMatrix();
}

DirectX::XMMATRIX Transform::GetMatrix() const {
	return DirectX::XMLoadFloat4x4(&mMatrix);
}


// Faut-il normaliser le quaternion ??
// Les angles doivent être converti en radian
// Le RotateWorld : Est-ce que c'est bien un objet qui tourne autour du (0,0,0) (centre du monde)

// Pour une rotation sur le monde : on remplace vDir, vRight et vUp par leur valeur initial (celle du monde)
// Puis lors de la multiplication final on inverse qRotRPY et qRot : DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&qRot), qRotRPY);
// Cela permet de se mettre sur le repère du monde pour la rotation