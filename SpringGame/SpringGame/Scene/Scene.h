#pragma once
class SceneController;
class Input;
class Scene
{
protected:
	SceneController& controller_;
public:
	Scene(SceneController& controller);

	/// <summary>
	/// シーンの初期化
	/// </summary>
	virtual void Init() {}

	/// <summary>
	/// シーンの情報の更新
	/// </summary>
	/// <param name="input"></param>
	virtual void Update(Input& input) = 0;

	/// <summary>
	/// シーンの描画(シーンの持ち物も描画する)
	/// </summary>
	virtual void Draw() = 0;
};

