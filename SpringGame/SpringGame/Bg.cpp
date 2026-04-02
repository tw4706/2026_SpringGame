#include "Bg.h"
#include<Dxlib.h>

Bg::Bg()
{
}

Bg::~Bg()
{
}

void Bg::Init()
{
	//スカイボックス用のテクスチャのロード
	skyTex_[0] = LoadGraph("data/backGround_rt.png");
	skyTex_[1] = LoadGraph("data/backGround_lf.png");
	skyTex_[2] = LoadGraph("data/backGround_up.png");
	skyTex_[3] = LoadGraph("data/backGround_dn.png");
	skyTex_[4] = LoadGraph("data/backGround_ft.png");
	skyTex_[5] = LoadGraph("data/backGround_bk.png");
}

void Bg::Draw(const Vector3& cameraPos)
{
    VECTOR c = VGet(cameraPos.x_, cameraPos.y_, cameraPos.z_);
    const float s = 3000.0f;

    SetUseZBuffer3D(FALSE);
    SetWriteZBuffer3D(FALSE);

    VERTEX3D v[6];

    auto SetV = [&](int i,
        float x, float y, float z,
        float u, float vuv)
        {
            v[i].pos = VGet(x, y, z);
            v[i].norm = VGet(0.0f, 0.0f, 1.0f);
            v[i].dif = GetColorU8(255, 255, 255, 255);
            v[i].spc = GetColorU8(255, 255, 255, 255);
            v[i].u = u;
            v[i].v = vuv;
        };

    // =========================
    // 前（ft）
    // =========================
    SetV(0, c.x - s, c.y - s, c.z - s, 0, 1);
    SetV(1, c.x + s, c.y - s, c.z - s, 1, 1);
    SetV(2, c.x - s, c.y + s, c.z - s, 0, 0);

    SetV(3, c.x + s, c.y - s, c.z - s, 1, 1);
    SetV(4, c.x + s, c.y + s, c.z - s, 1, 0);
    SetV(5, c.x - s, c.y + s, c.z - s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[4], TRUE);

    // =========================
    // 後（bk）
    // =========================
    SetV(0, c.x + s, c.y - s, c.z + s, 0, 1);
    SetV(1, c.x - s, c.y - s, c.z + s, 1, 1);
    SetV(2, c.x + s, c.y + s, c.z + s, 0, 0);

    SetV(3, c.x - s, c.y - s, c.z + s, 1, 1);
    SetV(4, c.x - s, c.y + s, c.z + s, 1, 0);
    SetV(5, c.x + s, c.y + s, c.z + s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[5], TRUE);

    // =========================
    // 左（lf）
    // =========================
    SetV(0, c.x + s, c.y - s, c.z - s, 0, 1);
    SetV(1, c.x + s, c.y - s, c.z + s, 1, 1);
    SetV(2, c.x + s, c.y + s, c.z - s, 0, 0);

    SetV(3, c.x + s, c.y - s, c.z + s, 1, 1);
    SetV(4, c.x + s, c.y + s, c.z + s, 1, 0);
    SetV(5, c.x + s, c.y + s, c.z - s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[1], TRUE);

    // =========================
    // 右（rt）
    // =========================
    SetV(0, c.x - s, c.y - s, c.z + s, 0, 1);
    SetV(1, c.x - s, c.y - s, c.z - s, 1, 1);
    SetV(2, c.x - s, c.y + s, c.z + s, 0, 0);

    SetV(3, c.x - s, c.y - s, c.z - s, 1, 1);
    SetV(4, c.x - s, c.y + s, c.z - s, 1, 0);
    SetV(5, c.x - s, c.y + s, c.z + s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[0], TRUE);

    // =========================
    // 上（up）
    // =========================
    SetV(0, c.x - s, c.y + s, c.z - s, 0, 1);
    SetV(1, c.x + s, c.y + s, c.z - s, 1, 1);
    SetV(2, c.x - s, c.y + s, c.z + s, 0, 0);

    SetV(3, c.x + s, c.y + s, c.z - s, 1, 1);
    SetV(4, c.x + s, c.y + s, c.z + s, 1, 0);
    SetV(5, c.x - s, c.y + s, c.z + s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[2], TRUE);

    // =========================
    // 下（dn）
    // =========================
    SetV(0, c.x - s, c.y - s, c.z + s, 0, 1);
    SetV(1, c.x + s, c.y - s, c.z + s, 1, 1);
    SetV(2, c.x - s, c.y - s, c.z - s, 0, 0);

    SetV(3, c.x + s, c.y - s, c.z + s, 1, 1);
    SetV(4, c.x + s, c.y - s, c.z - s, 1, 0);
    SetV(5, c.x - s, c.y - s, c.z - s, 0, 0);

    DrawPolygon3D(v, 2, skyTex_[3], TRUE);

    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
}
