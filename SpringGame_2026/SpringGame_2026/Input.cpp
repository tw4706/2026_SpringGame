#include "Input.h"
#include<Dxlib.h>

Input::Input() :inputData_{}, lastInputData_{}, inputTable_{}
{
	inputTable_["up"] = { {PeripheralType::keyboard,KEY_INPUT_UP},
						{PeripheralType::pad1,PAD_INPUT_UP} };
	inputTable_["down"] = { {PeripheralType::keyboard,KEY_INPUT_DOWN},
						{PeripheralType::pad1,PAD_INPUT_DOWN} };
	inputTable_["left"] = { {PeripheralType::keyboard,KEY_INPUT_LEFT},
						{PeripheralType::pad1,PAD_INPUT_LEFT} };
	inputTable_["right"] = { {PeripheralType::keyboard,KEY_INPUT_RIGHT},
						{PeripheralType::pad1,PAD_INPUT_RIGHT} };

	inputTable_["attack"] = { {PeripheralType::keyboard,KEY_INPUT_Z},
						{PeripheralType::pad1,PAD_INPUT_B} };
}

void Input::Update()
{

}

bool Input::IsPressed(const char* name) const
{
	return false;
}

bool Input::IsTriggered(const char* name) const
{
	return false;
}
