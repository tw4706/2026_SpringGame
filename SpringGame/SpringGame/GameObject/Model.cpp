#include "Model.h"

std::unordered_map<std::string, int> Model::sModelMap_;

Model::Model():
	handle_(-1)
{
}

void Model::Load(const std::string& path)
{
    //初回だけロード
    if (sModelMap_.count(path) == 0)
    {
        //モデルのロード
        int h = MV1LoadModel(path.c_str());
        //ハンドルの保存
        sModelMap_[path] = h;
    }

    //複製を作成してそれを使いまわす
    handle_ = MV1DuplicateModel(sModelMap_[path]);
}

void Model::Draw()
{
    //モデルの描画
    MV1DrawModel(handle_);
}

void Model::SetPosition(const Vector3& pos)
{
    //モデルの位置
    MV1SetPosition(handle_, pos.ToDxlibVector());
}

void Model::SetScale(const Vector3& scale)
{
    //モデルの拡縮
    MV1SetScale(handle_, scale.ToDxlibVector());
}

void Model::SetRotationY(float angle)
{
    //モデルの回転
    MV1SetRotationXYZ(handle_, VGet(0.0f, angle, 0.0f));
}

void Model::Release()
{
    if (handle_ != -1)
    {
        MV1DeleteModel(handle_);
        handle_ = -1;
    }
}
