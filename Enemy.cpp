#include"Enemy.h"
#include"Collision.h"

Enemy::Enemy() {

}

Enemy::~Enemy() {

}

void Enemy::Initialize(Model *model) {
	body = Object3d::Create();

	body->SetModel(model);
	body->SetPosition({ 0,0,0 });
	body->SetEye({ 0,0 ,-100 });
	pos = body->GetPosition();

	bullet = Object3d::Create();

	Model* bulletModel = nullptr;
	bulletModel = Model::LoadFromOBJ("sphere");
	bulletModel->LoadTexture("Resources/sphere/", "enemyBullet.png");
	bullet->SetModel(bulletModel);
	bullet->SetPosition(pos);
	bullet->SetEye({ 0,0,-100 });
	isBulletAlive = false;

	translation = { pos.x,pos.y,pos.z };
	scale = { 10,10,10 };

	//‘O
	frontPlane.normal = { 0, 0, 1 };

	frontPlane.distance = translation.z + scale.z;

	frontPlane.pos.x = translation.x;
	frontPlane.pos.y = translation.y;
	frontPlane.pos.z = translation.z + scale.z;

	frontPlane.size.x = scale.x;
	frontPlane.size.y = scale.y;
	frontPlane.size.z = 0;


	//Œã‚ë
	backPlane.normal = { 0, 0, -1 };

	backPlane.distance = -(translation.z - scale.z);

	backPlane.pos.x = translation.x;
	backPlane.pos.y = translation.y;
	backPlane.pos.z = translation.z - scale.z;

	backPlane.size.x = scale.x;
	backPlane.size.y = scale.y;
	backPlane.size.z = 0;


	//¶
	leftPlane.normal = { -1, 0, 0 };

	leftPlane.distance = -(translation.x - scale.x);

	leftPlane.pos.x = translation.x - scale.x;
	leftPlane.pos.y = translation.y;
	leftPlane.pos.z = translation.z;

	leftPlane.size.x = 0;
	leftPlane.size.y = scale.y;
	leftPlane.size.z = scale.z;

	//‰E
	rightPlane.normal = { 1, 0, 0 };

	rightPlane.distance = translation.x + scale.x;

	rightPlane.pos.x = translation.x + scale.x;
	rightPlane.pos.y = translation.y;
	rightPlane.pos.z = translation.z;

	rightPlane.size.x = 0;
	rightPlane.size.y = scale.y;
	rightPlane.size.z = scale.z;

	//ã
	upPlane.normal = { 0, 1, 0 };

	upPlane.distance = translation.y + scale.y;

	upPlane.pos.x = translation.x;
	upPlane.pos.y = translation.y + scale.y;
	upPlane.pos.z = translation.z;

	upPlane.size.x = scale.x;
	upPlane.size.y = 0;
	upPlane.size.z = scale.z;

	//‰º
	downPlane.normal = { 0, -1, 0 };

	downPlane.distance = -(translation.y - scale.y);

	downPlane.pos.x = translation.x;
	downPlane.pos.y = translation.y - scale.y;
	downPlane.pos.z = translation.z;

	downPlane.size.x = scale.x;
	downPlane.size.y = 0;
	downPlane.size.z = scale.z;
}

void Enemy::Reset() {
	body->SetPosition({ 0, 0, 0 });
	isBulletAlive = false;
	bulletTimer = bulletTIme;
	life = lifeMax;
}

void Enemy::Update() {
	body->Update();
	bulletpos = bullet->GetPosition();
	DirectX::XMFLOAT3 bulletSpeed = { bulletSpeed_.x,bulletSpeed_.y,bulletSpeed_.z };
	bullet->SetPosition({ bulletpos.x + bulletSpeed.x,bulletpos.y + bulletSpeed.y ,bulletpos.z + bulletSpeed.z });
	bullet->Update();
	if (invincibleTimer > 0) {
		invincibleTimer--;
	}
}

void Enemy::Draw() {
	if (invincibleTimer % 10 == 0) {
		body->Draw();
	}
	if (isBulletAlive == true) {
		bullet->Draw();
	}
}

void Enemy::Attack(DirectX::XMFLOAT3 playerPos) {
	this->playerPos = { playerPos.x,playerPos.y,playerPos.z };

	bulletTimer--;

	if (isBulletAlive == false) {
		bullet->SetPosition(body->GetPosition());
		if (bulletTimer <= 0) {
		isBulletAlive = true;
		bulletTimer = bulletTIme;
		bulletSpeed_ = this->playerPos - translation;
		bulletSpeed_ = bulletSpeed_.nomalize() * 7;
		}
	}
	else if (isBulletAlive) {
		if (bulletTimer < 0) {
			isBulletAlive = false;
		}
	}
}

bool Enemy::Oncollision() {
	if (invincibleTimer <= 0) {
		life--;
		if (life < 0) {
			return true;
		}
		invincibleTimer = invincibleTime;
	}
	return false;
}

bool Enemy::CheckCollision(Ray ray,float* dis) {
	if (Collision::CheckRay2Plane(ray, frontPlane, dis)) { return true; }
	if (Collision::CheckRay2Plane(ray, backPlane, dis)) { return true; }
	if (Collision::CheckRay2Plane(ray, leftPlane, dis)) { return true; }
	if (Collision::CheckRay2Plane(ray, rightPlane, dis)) { return true; }
	if (Collision::CheckRay2Plane(ray, upPlane, dis)) { return true; }
	if (Collision::CheckRay2Plane(ray, downPlane, dis)) { return true; }
	return false;
}

bool Enemy::CheckBulletCollision() {
	float distance;

	if (isBulletAlive) {
		distance = sqrt(pow(playerPos.x - bulletpos.x, 2) +
			pow(playerPos.y - bulletpos.y, 2) +
			pow(playerPos.z - bulletpos.z, 2));

		if (distance <= length) {
			isBulletAlive = false;
			return true;
		}
	}

	return false;
}