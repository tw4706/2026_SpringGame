#include "Model.h"

std::unordered_map<std::string, int> Model::sModelMap_;

Model::Model():
	handle_(-1)
{
}

void Model::Load(const std::string& path)
{
    //‰‰ñ‚¾‚¯ƒ[ƒh
    if (sModelMap_.count(path) == 0)
    {
        int h = MV1LoadModel(path.c_str());
        sModelMap_[path] = h;
    }

    //•¡»‚ğì¬‚µ‚Ä‚»‚ê‚ğg‚¢‚Ü‚í‚·
    handle_ = MV1DuplicateModel(sModelMap_[path]);
}

void Model::Draw()
{
    MV1DrawModel(handle_);
}

void Model::SetPosition(const Vector3& pos)
{
    MV1SetPosition(handle_, pos.ToDxlibVector());
}

void Model::SetScale(const Vector3& scale)
{
    MV1SetScale(handle_, scale.ToDxlibVector());
}

void Model::SetRotationY(float angle)
{
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
