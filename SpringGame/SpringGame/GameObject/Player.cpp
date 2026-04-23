#include "Player.h"
#include"../System/Input.h"
#include"../Physics/Matrix4x4.h"
#include"../Physics/Vector3.h"
#include "../GameObject/Enemy.h"
#include"../Manager/EffectManager.h"
#include"../Physics/Camera.h"
#include"../System/Application.h"
#include"DxLib.h"
#include<cassert>
#include<algorithm>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f,0.0f,0.0f };

	//最大体力
	constexpr int kMaxHP = 3;

	//プレイヤーの移動速度
	constexpr float kSpeed = 10.0f;

	//カメラの回転速度
	constexpr float kCameraSpeed = 0.03f;
	constexpr float kCameraPitch = 0.3f;

	//当たり判定のサイズ
	constexpr float kColSize = 60.0f;
	constexpr float kAttackColSize = 100.0f;

	//当たり判定位置の調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };

	//モデルのサイズ
	constexpr float kModelScale = 1.5f;

	//回避時間
	constexpr float kDodgeTime = 0.25f;

	//回避速度
	constexpr float kDodgeSpeed = 18.0f;

	//ジャスト回避の猶予フレーム数(16F)
	constexpr int kJustDodgeFrame = 4;

	//攻撃時間
	constexpr float kAttackTime = 0.8f;

	//攻撃の当たり判定が出ている時間
	constexpr float kAttackColEnabletime = 0.15f;

	//被ダメージ時間
	constexpr float kHitTime = 0.3f;

	//ノックバックに加わる力
	constexpr float kKnockbackPower = 8.0f;

	//ノックバックする時間
	constexpr float kKnockbackTime = 0.2f;

	//ノックバックで吹っ飛ぶ距離の倍率
	constexpr float kKnockbackMoveScale = 3.0f;

	//ノックバックした後の減速
	constexpr float kKnockbackDeceration = 0.9f;

	//移動時の線形補間
	constexpr float kMoveLerp = 0.15f;

	//停止する際の線形補間
	constexpr float kStopLerp = 0.2f;

	//攻撃判定の前判定距離の調整
	constexpr float kAttackDistance = 100.0f;

	//壁の制限エフェクトの距離調整
	constexpr float kBarrierEffectDistance = 70.0f;

	//エフェクトの位置(高さ)
	const Vector3 kEffectOffset = { 0.0f,80.0f,0.0f };

	//回避時の残像の表示時間
	constexpr float kAfterImageLife = 0.3f;

	//回避時間
	constexpr float kAfterImageTime = 0.02f;

	constexpr float kRotateLerpKeyboard = 0.2f;
	constexpr float kRotateLerpAnalogStick = 0.3f;

	//移動制限するための定数
	const float kWalkXLimit = 1500.0f;
	const float kWalkZLimit = 10000.0f;

	//壁制限のしきい値(誤差)
	constexpr float kWallHitEpsilon = 0.01f;

	//入力のしきい値(誤差)
	constexpr float kInputEpsilon = 0.01f;

	//移動の入力のしきい値
	constexpr float kRunEpsilon = 0.1f;

	//アナログスティックのデッドゾーン
	constexpr float kStickDeadZone = 0.04f;

	//移動のフレーム補正
	constexpr float kFrameRate = 60.0f;

	//カメラの揺れ時間
	constexpr float kCameraShakeTime = 0.2f;

	//カメラの揺らす力
	constexpr float kCameraShakePower = 10.0f;

	//無敵時間
	constexpr float kInvincibleTime = 0.5f;
}

Player::Player() :
	GameObject(pos_, vel_),
	state_(PlayerState::Idle),
	moveAngle_(0.0f),
	attackTimer_(0.0f),
	invincibleTimer_(0.0f),
	dodgeTimer_(0.0f),
	hitTimer_(0.0f),
	afterImageTimer_(0.0f),
	isJustDodge_(false),
	isJustDodgeTriggered_(false),
	justDodgeFrame_(0),
	justDodgeBuffTimer_(0.0f),
	isJustDodgeBuff_(false),
	hp_(kMaxHP),
	collider_(0.0f),
	attackCollider_(0.0f),
	isHit_(false),
	knockbackVel_(0.0f, 0.0f, 0.0f),
	knockbackTimer_(0.0f),
	isTouchingWall_(false)
{
	//初期位置
	pos_ = kFirstPos;

	//当たり判定(キャラクター・攻撃)
	collider_ = SphereCollider(kColSize);
	attackCollider_ = SphereCollider(kAttackColSize);

	//当たり判定の所有者
	collider_.SetOwner(this);
	attackCollider_.SetOwner(this);

	//コライダーを登録
	GameObject::pCollider_ = &collider_;
}

Player::~Player()
{
	model_.Release();
}

void Player::Init()
{
	//モデルのロード
	model_.Load("data/Player_Idle.mv1");
	ghostModel_.Load("data/PlayerAfterImage.mv1");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle(), AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);

	//当たり判定の設定
	collider_.SetEnable(true);
	attackCollider_.SetEnable(false);
	//当たり判定の種類の設定
	collider_.SetColliderType(ColliderType::Charactor);
	attackCollider_.SetColliderType(ColliderType::Attack);
}

void Player::Update(Input& input, float dt)
{
	isHit_ = false;

	//タイマーの更新
	UpdateTimers(dt);
	//入力処理
	HandleInput(input);

	//状態別の処理
	UpdateAction(input, dt);

	//ノックバックの更新
	UpdateKnockBack(dt);

	//状態遷移の処理
	UpdateState();

	//アニメーションの更新
	UpdateAnimation(dt);

	//当たり判定の更新
	UpdateCollision();

	//移動の制限 
	//制限されている見えない壁に触れた瞬間にエフェクトを生成する
	Vector3 beforePos = pos_;

	if (pos_.x_ > kWalkXLimit) pos_.x_ = kWalkXLimit;
	if (pos_.x_ < -kWalkXLimit) pos_.x_ = -kWalkXLimit;
	if (pos_.z_ > kWalkZLimit) pos_.z_ = kWalkZLimit;
	if (pos_.z_ < 0.0f) pos_.z_ = 0.0f;

	bool hitWall = fabs(beforePos.x_ - pos_.x_) > kWallHitEpsilon || fabs(beforePos.z_ - pos_.z_) > kWallHitEpsilon;

	if (hitWall && !isTouchingWall_)
	{
		Vector3 forward = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };

		Vector3 effectPos = pos_ + forward * kBarrierEffectDistance + kEffectOffset;

		EffectManager::GetInstance().Play("barrier", effectPos);

		Application::GetInstance().GetSoundManager().PlaySe(SE::Cancel);
	}

	isTouchingWall_ = hitWall;

	//行列の更新 
	UpdateMatrix();

	//回避した時の残像の削除処理
	for (auto it = afterImages_.begin(); it != afterImages_.end(); )
	{
		it->life -= dt;

		if (it->life <= 0.0f)
		{
			MV1DeleteModel(it->modelHandle);
			it = afterImages_.erase(it);
		}
		else
		{
			it++;
		}
	}

	afterImages_.erase(
		std::remove_if(afterImages_.begin(), afterImages_.end(),
			[](const AfterImage& a) { return a.life <= 0.0f; }),
		afterImages_.end()
	);
}

void Player::Draw()
{
	//モデルの描画
	model_.Draw();

	for (auto& img : afterImages_)
	{
		Matrix4x4 mat =
			Matrix4x4::Scale(kModelScale, kModelScale, kModelScale) *
			Matrix4x4::RotateY(img.angle + DX_PI_F) *
			Matrix4x4::Translate(img.pos.x_, img.pos.y_, img.pos.z_);

		MV1SetMatrix(img.modelHandle, mat.ToDxLibMatrix());
		MV1DrawModel(img.modelHandle);
	}


#ifdef _DEBUG
	//当たり判定の描画
	unsigned int color = isHit_ ? 0xff0000 : 0x00ff00;

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(),		//中心
		collider_.GetRadian(),					//半径
		16, color, color, FALSE);

	//攻撃判定の描画
	if (state_ == PlayerState::Attack)
	{
		DrawSphere3D(attackCollider_.GetPos().ToDxlibVector(),
			attackCollider_.GetRadian(), 16, 0xffff00, 0xffff00, FALSE);
	}

	char buf[128];
	sprintf_s(buf, "Pos: %.2f, %.2f, %.2f", pos_.x_, pos_.y_, pos_.z_);

	DrawString(20, 180, buf, 0xffffff);
#endif
}

void Player::Move(Input& input, float dt)
{
	//移動ベクトルの初期化
	vel_ = { 0.0f, 0.0f, 0.0f };

	//キーボード入力から方向作る 
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input.IsPressed("up")) inputDir.z_ += 1.0f;
	if (input.IsPressed("down")) inputDir.z_ -= 1.0f;
	if (input.IsPressed("left")) inputDir.x_ -= 1.0f;
	if (input.IsPressed("right")) inputDir.x_ += 1.0f;

	//入力があるときだけ 
	if (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > kInputEpsilon)
	{
		//カメラの角度を基準にして移動方向を回転させる
		float cameraYaw = 0.0f;
		if (pCamera_)
		{
			cameraYaw = pCamera_->GetYaw();
		}

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		Vector3 playerDir = rotMat.TransformForVector(inputDir);

		//正規化 
		playerDir.Normalize();

		//移動 
		vel_.x_ = playerDir.x_ * kSpeed;
		vel_.z_ = playerDir.z_ * kSpeed;

		//向き更新 
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);
		float diff = playerAngle - moveAngle_;

		//キャラクターの角度を補間する
		while (diff > DX_PI_F) diff -= DX_PI_F * 2;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2;

		//スムーズに回転させる
		moveAngle_ += diff * kRotateLerpKeyboard;
	}

	//アナログスティックの更新 
	UpdateAnalogStick(input);

	//位置の反映
	pos_ += vel_ * dt * kFrameRate;
}

void Player::StartAttack()
{
	state_ = PlayerState::Attack;
	attackTimer_ = kAttackTime;
}

void Player::StartDodge()
{
	state_ = PlayerState::Dodge;
	dodgeTimer_ = kDodgeTime;

	//ジャスト回避の判定開始
	justDodgeFrame_ = kJustDodgeFrame;
	isJustDodge_ = false;

	//SEの再生
	Application::GetInstance().GetSoundManager().PlaySe(SE::Dodge);
}

void Player::UpdateTimers(float deltaTime)
{
	if (attackTimer_ > 0.0f) attackTimer_ -= deltaTime;
	if (dodgeTimer_ > 0.0f) dodgeTimer_ -= deltaTime;
	if (hitTimer_ > 0.0f)hitTimer_ -= deltaTime;
	if (invincibleTimer_ > 0.0f) invincibleTimer_ -= deltaTime;
	if (justDodgeFrame_ > 0)justDodgeFrame_--;
	if (isJustDodgeBuff_)
	{
		justDodgeBuffTimer_ -= deltaTime;

		if (justDodgeBuffTimer_ <= 0.0f)
		{
			isJustDodgeBuff_ = false;
		}
	}
}

void Player::HandleInput(Input& input)
{
	if (knockbackTimer_ > 0.0f) return;

	//ダメージ受けたときや死亡中は入力を受け付けない
	if (state_ == PlayerState::Hit || state_ == PlayerState::Death)return;

	//攻撃中は新しい攻撃を受け付けない
	if (state_ == PlayerState::Attack)return;

	if (input.IsTriggered("dodge") && state_ != PlayerState::Dodge)
	{
		StartDodge();
		return;
	}

	if (input.IsTriggered("attack"))
	{
		StartAttack();
		return;
	}
}

void Player::UpdateAction(Input& input, float dt)
{
	switch (state_)
	{
	case PlayerState::Idle:
	case PlayerState::Run:
		//ノックバック中は移動処理を行わない
		if (knockbackTimer_ <= 0.0f)
		{
			Move(input, dt);
		}
		break;

	case PlayerState::Attack:
		if (knockbackTimer_ <= 0.0f)
		{
			Move(input, dt);
		}
		UpdateAttack();
		break;

	case PlayerState::Dodge:
		UpdateDodge(dt);
		break;
	case PlayerState::Hit:
		break;
	case PlayerState::Death:
		break;
	}
}

void Player::UpdateState()
{
	if (state_ == PlayerState::Attack && attackTimer_ <= 0.0f)state_ = PlayerState::Idle;

	if (state_ == PlayerState::Dodge && dodgeTimer_ <= 0.0f)state_ = PlayerState::Idle;

	if (state_ == PlayerState::Idle || state_ == PlayerState::Run)
	{
		if (fabs(vel_.x_) > kRunEpsilon || fabs(vel_.z_) > kRunEpsilon)
		{
			state_ = PlayerState::Run;
		}
		else
		{
			state_ = PlayerState::Idle;
		}
	}
	//ダメージ処理から戻す
	if (state_ == PlayerState::Hit && hitTimer_ <= 0.0f)
	{
		state_ = PlayerState::Idle;
	}

	//死亡は戻らない
	if (state_ == PlayerState::Death)return;
}

void Player::UpdateAttack()
{
	//ジャスト回避時は攻撃判定を広げる
	float attackRadius = kAttackColSize;

	//ジャスト回避のバフを受けている場合は攻撃判定を広げる
	if (isJustDodgeBuff_)
	{
		attackRadius *= 5.0f;
	}

	//攻撃判定の半径をセット
	attackCollider_.SetRadian(attackRadius);

	//プレイヤーの前方に攻撃判定を生成する
	Vector3 forward = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };
	Vector3 attackPos = pos_ + forward * kAttackDistance + kColOffset;

	attackCollider_.SetPos(attackPos);

	if (attackTimer_ > kAttackColEnabletime)
	{
		attackCollider_.SetEnable(true);
	}
	else
	{
		attackCollider_.SetEnable(false);
	}

	if (attackTimer_ <= 0.0f)
	{
		attackCollider_.SetEnable(false);
	}
}

void Player::UpdateKnockBack(float dt)
{
	if (knockbackTimer_ > 0.0f)
	{
		pos_ += knockbackVel_ * dt * kKnockbackMoveScale;

		//減速（自然に止まるようにする）
		knockbackVel_ *= kKnockbackDeceration;

		knockbackTimer_ -= dt;
	}
}

void Player::UpdateDodge(float dt)
{
	//前方向に高速移動する
	Vector3 dir = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };

	pos_ += dir * kDodgeSpeed * dt * kFrameRate;

	//回避中は当たり判定を無効
	collider_.SetPos(pos_ + kColOffset);

	//無敵終了後に当たり判定を有効にする
	if (dodgeTimer_ <= 0.0f)
	{
		collider_.SetEnable(true);
		collider_.SetPos(pos_ + kColOffset);
	}

	afterImageTimer_ -= dt;

	if (afterImageTimer_ <= 0.0f)
	{
		afterImages_.push_back({ MV1DuplicateModel(ghostModel_.GetHandle()), pos_, moveAngle_, kAfterImageLife });
		afterImageTimer_ = kAfterImageTime;//速さ
	}
}

void Player::UpdateAnimation(float dt)
{
	AnimationState animState = AnimationState::Idle;

	//プレイヤーの状態とアニメーションの状態を紐づける
	switch (state_)
	{
	case PlayerState::Idle:		animState = AnimationState::Idle; break;
	case PlayerState::Run:		animState = AnimationState::Run; break;
	case PlayerState::Attack:	animState = AnimationState::Attack; break;
	case PlayerState::Hit:		animState = AnimationState::Hit; break;
	case PlayerState::Dodge:	animState = AnimationState::Dodge; break;
	case PlayerState::Death:	animState = AnimationState::Death; break;
	}

	if (animation_.GetState() != animState)
	{
		animation_.ChangeState(animState);
	}

	animation_.Update(dt);
}

void Player::UpdateCollision()
{
	if (state_ != PlayerState::Attack)
	{
		attackCollider_.SetEnable(false);
	}

	collider_.SetPos(pos_ + kColOffset);
}

void Player::UpdateAnalogStick(Input& input)
{
	//左アナログスティックの取得
	Vector3 stickL = input.GetStickLeft();
	Vector3 stickR = input.GetStickRight();

	Vector3 playerDir = { 0.0f, 0.0f, 0.0f };

	if (stickL.LengthSq() > kStickDeadZone)
	{
		float cameraYaw = pCamera_ ? pCamera_->GetYaw() : 0.0f;

		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraYaw);
		playerDir = rotMat.TransformForVector(stickL).Normalize();

		//速度の線形補間
		Vector3 targetVel = playerDir * kSpeed;
		vel_.x_ = Vector3::Lerp(vel_.x_, targetVel.x_, kMoveLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, targetVel.z_, kMoveLerp);

		//プレイヤーの向いている方向の更新
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);
		float diff = playerAngle - moveAngle_;

		while (diff > DX_PI_F) diff -= DX_PI_F * 2.0f;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

		moveAngle_ += diff * kRotateLerpAnalogStick;
	}
	else
	{
		//入力がない場合は減速させる
		vel_.x_ = Vector3::Lerp(vel_.x_, 0.0f, kStopLerp);
		vel_.z_ = Vector3::Lerp(vel_.z_, 0.0f, kStopLerp);
	}

	//カメラ回転
	if (pCamera_)
	{
		pCamera_->AddRotation(stickR.x_ * kCameraSpeed, stickR.z_ * kCameraSpeed);
	}
}

void Player::UpdateMatrix()
{
	//回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_ + DX_PI_F);

	//移動
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale(kModelScale, kModelScale, kModelScale);

	//行列の合成
	Matrix4x4 mat = scaleMat * rotMat * transMat;
	MV1SetMatrix(model_.GetHandle(), mat.ToDxLibMatrix());
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}

int Player::GetMaxHP() const
{
	return kMaxHP;
}

void Player::OnCollision(GameObject* other)
{

	if (dynamic_cast<Enemy*>(other))
	{
		//ジャスト回避判定
		if (state_ == PlayerState::Dodge && justDodgeFrame_ > 0)
		{
			if (!isJustDodge_)
			{
				EffectManager::GetInstance().Play("dodge", pos_);
				isJustDodge_ = true;
				isJustDodgeTriggered_ = true;

				invincibleTimer_ = kInvincibleTime;

				isJustDodgeBuff_ = true;
				justDodgeBuffTimer_ = 0.5f;

				//SE再生
				Application::GetInstance().GetSoundManager().PlaySe(SE::JustDodge);
			}
			return;
		}
		OnHit(other);
	}
}

bool Player::IsJustDodge()
{
	if (isJustDodgeTriggered_)
	{
		isJustDodgeTriggered_ = false;
		return true;
	}
	return false;
}

void Player::OnHit(GameObject* attacker)
{
	if (IsInvincible()) return;

	hp_--;

	//SEの再生
	Application::GetInstance().GetSoundManager().PlaySe(SE::Hit);

	isHit_ = true;

	//ノックバック方向（敵から離れるように飛ばす）
	Vector3 knockDir = pos_ - attacker->GetPos();
	knockDir.y_ = 0.0f;
	knockDir.Normalize();

	knockbackVel_ = knockDir * kKnockbackPower;
	knockbackTimer_ = kKnockbackTime;

	//向きは敵の方向を見る
	Vector3 lookDir = attacker->GetPos() - pos_;
	lookDir.y_ = 0.0f;
	lookDir.Normalize();

	moveAngle_ = atan2f(-lookDir.x_, lookDir.z_);

	if (hp_ <= 0)
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::PlayerDeath);
		hp_ = 0;
		state_ = PlayerState::Death;
	}
	else
	{
		state_ = PlayerState::Hit;
		hitTimer_ = kHitTime;
	}

	//カメラの揺れ
	if (pCamera_)
	{
		pCamera_->Shake(kCameraShakeTime, kCameraShakePower);
	}

	//無敵時間
	invincibleTimer_ = kInvincibleTime;
}
