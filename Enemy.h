#pragma once
#include"Object3d.h"
#include"CollisionPrimitive.h"
#include <DirectXMath.h>

class Enemy {
public:
	Enemy();
	~Enemy();

	void Initialize(Model *model);
	void Reset();

	void Update();

	void Draw();

	bool CheckCollision(Ray ray,float* dis);

	bool Oncollision();

	void Attack(DirectX::XMFLOAT3 playerPos);

	bool CheckBulletCollision();

private:
	DirectX::XMFLOAT3 pos;
	Vector3 translation;
	Vector3 scale;

	Plane frontPlane;
	Plane backPlane;
	Plane leftPlane;
	Plane rightPlane;
	Plane upPlane;
	Plane downPlane;

	Vector3 playerPos;
	bool isBulletAlive;
	const int bulletTIme = 30;
	int bulletTimer = bulletTIme;
	Vector3 bulletSpeed_;
	DirectX::XMFLOAT3 bulletpos;
	float length = 40;

	const int lifeMax = 10;
	int life = lifeMax;
	const int invincibleTime = 60;
	int invincibleTimer = 0;

public:
	Object3d* body = nullptr;
	Object3d* bullet = nullptr;
};