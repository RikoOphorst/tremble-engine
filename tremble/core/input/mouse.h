#pragma once

namespace tremble
{
	/**
	* @enum tremble::MOUSE_BUTTON_TYPE
	* @brief Enum of all the mouse button types, compliant with windows input enum
	*/
	enum MOUSE_BUTTON_TYPE
	{
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_NULL
	};

	/**
	* @struct tremble::MouseState
	* @brief Represents the mouse state(position, movement, and wheel movement)
	* @author Anton Gavrilov
	*/
	struct MouseState
	{
		DirectX::XMINT2 position = DirectX::XMINT2(0,0);
		DirectX::XMINT2 delta_position = DirectX::XMINT2(0, 0);
		float delta_wheel = 0.0f;
	};
}