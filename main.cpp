#include<d3dcompiler.h>
#include<Windows.h>
#include <tchar.h>
#include<iostream>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<vector>
#include<string>
#include<DirectXMath.h>
#include<DirectXTex.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include"Triangle.h"
#include"object3d.h"
#include"input.h"
#include"WinApp.h"
#include"DirectXCommon.h"

using namespace DirectX;
using namespace std;

#include"Enemy.h"

//ウィンドウプロージャー
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wapram, LPARAM lparam) {
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウインドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wapram, lparam);
}


//windowsアプリのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region 基盤システムの初期化
	WinApp* winApp = nullptr;
	//WindowsAPIの初期化
	winApp = new WinApp;
	winApp->Initialize();

	DirectXCommon* dxCommon = nullptr;
	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	Input* input_ = nullptr;
	//DirectInputの初期化
	input_ = new Input;
	input_->Initialize(winApp);
#pragma endregion 基盤システムの初期化
#pragma region 最初のシーンの初期化


#pragma endregion 最初のシーンの初期化
	MSG msg{};
	HRESULT result;

	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 80, -400);//視点ベクトル
	XMFLOAT3 target(0, 0, 200);//注視点座標
	XMFLOAT3 up(0, 1, 0);	 //上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	float angle = 0.0f;//カメラの回転角


//----------------------ゲームループ-------------------------
	//シーン管理
	enum class Scene {
		Title,
		Play,
		Clear,
		Gameover,
	};
	Scene scene = Scene::Title;

	//------------オブジェクト-----------
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);
	Model::SetDevice(dxCommon->GetDevice());

	//タイトル
	Model* titleModel = nullptr;
	Object3d* title = nullptr;

	title = Object3d::Create();
	titleModel = Model::LoadFromOBJ("square_mat");
	titleModel->LoadTexture("Resources/square_mat/", "title.png");

	title->SetModel(titleModel);
	title->SetPosition({ 0,0,55 });
	title->SetEye({ 0, 0, -100 });

	//タイトル
	Model* clearModel = nullptr;
	Object3d* clear = nullptr;

	clear = Object3d::Create();
	clearModel = Model::LoadFromOBJ("square_mat");
	clearModel->LoadTexture("Resources/square_mat/", "clear.png");

	clear->SetModel(clearModel);
	clear->SetPosition({ 0,0,55 });
	clear->SetEye({ 0, 0, -100 });

	//ゲームオーバー
	Model* gameOverModel = nullptr;
	Object3d* gameOver = nullptr;

	gameOver = Object3d::Create();
	gameOverModel = Model::LoadFromOBJ("square_mat");
	gameOverModel->LoadTexture("Resources/square_mat/", "gameover.png");

	gameOver->SetModel(gameOverModel);
	gameOver->SetPosition({ 0,0,55 });
	gameOver->SetEye({ 0, 0, -100 });


	Object3d* field = nullptr;
	Model* model3 = nullptr;

	field = Object3d::Create();

	model3 = Model::LoadFromOBJ("box");

	field->SetModel(model3);
	field->SetPosition({ 0,-120,-100 });
	field->SetEye(eye);

	//プレイヤー
	const int lifeMax = 5;
	int life = lifeMax;
	Object3d* player = nullptr;
	Model* model1 = nullptr;

	player = Object3d::Create();

	model1 = Model::LoadFromOBJ("sphere");

	player->SetModel(model1);
	player->SetPosition({ 0,0,-240 });
	player->SetEye(eye);
	player->SetTarget({ 0,0,-200 });

	bool isLaserAlive = false;
	const int laserTime = 60;
	int laserTimer=laserTime;

	Ray laserRay;
	XMFLOAT3 pos = player->GetPosition();
	laserRay.start = { pos.x,pos.y,pos.z };
	laserRay.dir = { 0,0,1 };

	Object3d* laser = nullptr;
	Model* lasserModel = nullptr;
	laser = Object3d::Create();
	lasserModel = Model::LoadFromOBJ("laser");

	laser->SetModel(lasserModel);
	laser->SetPosition(pos);
	laser->SetEye(eye);

	//敵
	Model* model2 = nullptr;
	model2 = Model::LoadFromOBJ("cube");
	Enemy* enemy = new Enemy();
	enemy->Initialize(model2);

	while (true) {
#pragma region 基盤システムの更新
		//×ボタンで終了メッセージがきたらゲームループを抜ける
		if (winApp->ProcessMessage()) {
			break;
		}
		//入力の更新
		input_->Update();

#pragma endregion 基盤システムの更新

#pragma region 最初のシーンの更新
		switch (scene)
		{
		case Scene::Title:
			title->Update();
			Object3d::SetEye({0,0,-100});
			Object3d::SetTarget({ 0,0,0 });

			//スペースでゲームシーンに
			if (input_->TriggerKey(DIK_SPACE)) {
				scene = Scene::Play;
				Object3d::SetEye(eye);
				Object3d::SetTarget({ 0,0,-200 });
			}

			//リセット
			isLaserAlive = false;
			laserTimer = laserTime;
			player->SetPosition({ 0,0,-240 });
			life = lifeMax;
			enemy->Reset();

			break;
		case Scene::Play:

			pos = player->GetPosition();
			
			//移動
			if (input_->PushKey(DIK_W) || input_->PushKey(DIK_A) || input_->PushKey(DIK_S) || input_->PushKey(DIK_D)) {
				const float speed = 4.0f;
				if (input_->PushKey(DIK_W)) {
					pos.y += speed;
				}else if (input_->PushKey(DIK_S)) {
					pos.y -= speed;
				}
				if (input_->PushKey(DIK_D)) {
					pos.x += speed;
				}else if (input_->PushKey(DIK_A)) {
					pos.x -= speed;
				}
				if (pos.x > -150 && pos.x < 150 && pos.y > -100 && pos.y < 100) {
					player->SetPosition(pos);
				}
			}

			//攻撃
			if (input_->TriggerKey(DIK_SPACE)) {
				if (isLaserAlive==false) {
					isLaserAlive = true;
					laserTimer = laserTime;
					laser->SetPosition(pos);
					laserRay.start = { pos.x,pos.y,pos.z };
				}
			}
			
			if (isLaserAlive) {
				laserTimer--;
				if (laserTimer < 0) {
					isLaserAlive = false;
				}
				float dis;
				if (enemy->CheckCollision(laserRay, &dis)) {
					if (enemy->Oncollision()) {
						scene = Scene::Clear;
					}
				}
			}
			
			enemy->Attack(player->GetPosition());

			if (enemy->CheckBulletCollision()) {
				life--;
				if (life < 0) {
					scene = Scene::Gameover;

				}
			}

			player->Update();
			laser->Update();
			enemy->Update();
			field->Update();


			break;
		case Scene::Clear:
			Object3d::SetEye({ 0,0,-100 });
			Object3d::SetTarget({ 0,0,0 });
			clear->Update();


			//スペースでタイトルシーンに
			if (input_->TriggerKey(DIK_SPACE)) {
				scene = Scene::Title;
			}

			break;
		case Scene::Gameover:
			Object3d::SetEye({ 0,0,-100 });
			Object3d::SetTarget({ 0,0,0 });
			gameOver->Update();

			//スペースでタイトルシーンに
			if (input_->TriggerKey(DIK_SPACE)) {
				scene = Scene::Title;
			}

			break;
		}

#pragma endregion 最初のシーンの更新
		//描画前処理
		dxCommon->PreDraw();
		Object3d::PreDraw(dxCommon->GetCommandList());

#pragma region 最初のシーンの描画
		switch (scene)
		{
		case Scene::Title:
			title->Draw();

			break;
		case Scene::Play:
			player->Draw();
			if (isLaserAlive) {
				laser->Draw();
			}

			enemy->Draw();
			field->Draw();

			break;
		case Scene::Clear:
			clear->Draw();

			break;
		case Scene::Gameover:
			gameOver->Draw();

			break;
		}

#pragma endregion 最初のシーンの描画

		//描画後処理
		dxCommon->PostDrow();
		Object3d::PostDraw();
	}

#pragma region 最初のシーンの終了


#pragma endregion 最初のシーンの終了

#pragma region 基盤システムの終了
	delete input_;

	delete dxCommon;

	winApp->Finalize();

	delete winApp;
#pragma endregion 基盤システムの終了
	return 0;
}