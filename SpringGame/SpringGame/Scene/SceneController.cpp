#include "SceneController.h"
#include "Scene.h"
#include"../Manager/EffectManager.h"

SceneController::SceneController()
{
}

SceneController::~SceneController()
{
}

void SceneController::ResetScene(std::shared_ptr<Scene> scene)
{
	//シーンを一つにするために一旦消す
	scenes_.clear();
	scenes_.push_back(scene);
	scene->Init();
}

void SceneController::ChangeScene(std::shared_ptr<Scene> scene)
{
	scenes_.clear();
	//もし、リストが空の場合、普通に代入でChangeSceneしようとすると
	//当然、emptyの箱に対してChangeしようとするので、クラッシュします。
	//このため、最初にemptyかどうかチェックします。
	if (scenes_.empty())
	{
		//最初は要素がないため
		//空っぽの場合が指定の要素をpushします。
		//少なくとも1つは積まれている状態にする
		scenes_.push_back(scene);
	}
	else
	{

		scenes_.back() = scene;// この行の時点で元のシーンは自動的に削除されています。
	}
	scene->Init();
}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{	
	//シーンを切り替える前に全エフェクトの停止を行う
	EffectManager::GetInstance().StopAll();

	scenes_.push_back(scene);
}

void SceneController::PopScene()
{
	scenes_.pop_back();
}

void SceneController::Update(Input& input)
{
	//末尾の要素に対してのみUpdateする
	scenes_.back()->Update(input);
}
void SceneController::Draw()
{
	//末尾の要素に対してのみDrawする
	for (auto& scene : scenes_)
	{
		scene->Draw();
	}
}