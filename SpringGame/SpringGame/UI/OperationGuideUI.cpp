#include "OperationGuideUI.h"
#include"../Game.h"
#include<Dxlib.h>
#include<cassert>

OperationGuideUI::OperationGuideUI() :
	aButtonHandle_(-1),
	bButtonHandle_(-1)
{
}

OperationGuideUI::~OperationGuideUI()
{
	//ƒnƒ“ƒhƒ‹‚Ì‰ğ•ú
	DeleteGraph(aButtonHandle_);
	DeleteGraph(bButtonHandle_);
}

void OperationGuideUI::Init()
{
	//‰æ‘œ‚Ì“Ç‚İ‚İ
	aButtonHandle_ = LoadGraph("data/AButton.png");
	bButtonHandle_ = LoadGraph("data/BButton.png");
}


void OperationGuideUI::Draw()
{
	//ƒ{ƒ^ƒ“‚Ì•`‰æ
	int x = Game::kScreenWidth - 230;
	int y = 50;
	float scale = 0.1f;
	float angle = 0.0f;

	//•`‰æ
	DrawRotaGraph(x, y, scale, angle, aButtonHandle_, true);
	DrawRotaGraph(x, y + 70, scale, angle, bButtonHandle_, true);

	//‘€ì‚Ì•¶š‚Ì•`‰æ
	const char* AButtonText = "‰ñ”ğ";
	const char* bButtonText = "UŒ‚";

	int buttonSizeX, buttonSizeY;
	GetGraphSize(aButtonHandle_, &buttonSizeX, &buttonSizeY);

	int buttonATextX = static_cast<int>(x + (buttonSizeX * scale / 2) + 60);
	int buttonATextY = y - 20;

	int buttonBTextX = static_cast<int>(x + (buttonSizeX * scale / 2) +10);
	int buttonBTextY = y + 45;

	DrawStringToHandle(buttonATextX + 4 - 50, buttonATextY + 4, AButtonText, GetColor(0, 0, 0), Game::kFontUIHandle);
	DrawStringToHandle(buttonATextX - 50, buttonATextY, AButtonText, GetColor(255, 255, 255), Game::kFontUIHandle);

	DrawStringToHandle(buttonBTextX + 4, buttonBTextY + 4, bButtonText, GetColor(0, 0, 0), Game::kFontUIHandle);
	DrawStringToHandle(buttonBTextX, buttonBTextY, bButtonText, GetColor(255, 255, 255), Game::kFontUIHandle);
}


