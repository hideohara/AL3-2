#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// 乱数シード生成器
	std::random_device seed_gen;
	// メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	// 乱数範囲（回転角用）
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	// 乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	// for (size_t i = 0; i < _countof(worldTransform_); i++) {

	//	// X, Y, Z 方向のスケーリングを設定
	//	worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};

	//	//  X, Y, Z 軸周りの回転角を設定
	//	worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

	//	//  X, Y, Z 軸周りの平行移動を設定
	//	worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

	//	// ワールドトランスフォームの初期化
	//	worldTransform_[i].Initialize();
	//}

	// 大本
	worldTransform_[PartId::Root].Initialize();
	// 背骨
	worldTransform_[PartId::Spine].translation_ = {0, 4.5f, 0};
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();

	// 上半身
	// 胸
	worldTransform_[PartId::Chest].translation_ = {0, 0, 0};
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Chest].Initialize();
	// 頭
	worldTransform_[PartId::Head].translation_ = {0, 4.5f, 0};
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].Initialize();
	// 左手
	worldTransform_[PartId::ArmL].translation_ = {-4.5f, 0, 0};
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].Initialize();
	// 右手
	worldTransform_[PartId::ArmR].translation_ = {4.5f, 0, 0};
	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].Initialize();

	// 下半身
	// 尻
	worldTransform_[PartId::Hip].translation_ = {0, -4.5f, 0};
	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Hip].Initialize();
	// 左足
	worldTransform_[PartId::LegL].translation_ = {-3, -4.5f, 0};
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].Initialize();
	// 右足
	worldTransform_[PartId::LegR].translation_ = {3, -4.5f, 0};
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].Initialize();

	// カメラ視点座標を設定
	// viewProjection_.eye = {0, 0, -10};

	// カメラ注視点座標を設定
	// viewProjection_.target = {10, 0, 0};

	// カメラ上方向ベクトルを設定（右上45度指定)
	// viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};

	// カメラ垂直方向視野角を設定
	// viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	// アスペクト比を設定
	// viewProjection_.aspectRatio = 1.0f;

	//// ニアクリップ距離を設定
	// viewProjection_.nearZ = 52.0f;
	//// ファークリップ距離を設定
	// viewProjection_.farZ = 53.0f;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	// キャラクター移動処理
	{
		// キャラクターの移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		// キャラクターの移動速さ
		const float kCharacterSpeed = 0.2f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = {-kCharacterSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kCharacterSpeed, 0, 0};
		}

		// 親の移動（ベクトルの加算）
		worldTransform_[0].translation_.x += move.x;
		worldTransform_[0].translation_.y += move.y;
		worldTransform_[0].translation_.z += move.z;

		// デバッグ用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
		  "Root:(%f,%f,%f)", worldTransform_[0].translation_.x, worldTransform_[0].translation_.y,
		  worldTransform_[0].translation_.z);
	}

	// 上半身回転処理
	{
		// 上半身の回転速さ[ラジアン/frame]
		const float kChestRotSpeed = 0.05f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_U)) {
			worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;
		} else if (input_->PushKey(DIK_I)) {
			worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;
		}
	}

	// 下半身回転処理
	{
		// 下半身の回転速さ[ラジアン/frame]
		const float kHipRotSpeed = 0.05f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_J)) {
			worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;
		} else if (input_->PushKey(DIK_K)) {
			worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;
		}
	}

	worldTransform_[PartId::Root].UpdateMatrix();
	worldTransform_[PartId::Spine].UpdateMatrix();
	worldTransform_[PartId::Chest].UpdateMatrix();
	worldTransform_[PartId::Head].UpdateMatrix();
	worldTransform_[PartId::ArmL].UpdateMatrix();
	worldTransform_[PartId::ArmR].UpdateMatrix();
	worldTransform_[PartId::Hip].UpdateMatrix();
	worldTransform_[PartId::LegL].UpdateMatrix();
	worldTransform_[PartId::LegR].UpdateMatrix();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// for (size_t i = 0; i < _countof(worldTransform_); i++) {
	//	model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	// }

	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
