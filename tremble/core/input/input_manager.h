#pragma once

#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <Windows.h>
#undef _WINSOCK2API_
#undef _WINSOCKAPI_
#include "key.h"
#include "mouse.h"
#include "../networking/peer.h"

namespace tremble
{
	class GameManager;
	class Window;
    class RakNet::BitStream;

	/**
	* @struct tremble::KeyState
	* @brief Keyboard key state
	* @author Anton Gavrilov
	*/
	struct KeyState
	{
		bool pressed = false; //!< Was this key pressed this frame?
		bool down = false; //!< Is key down? (Meaning not pressed this exact frame, but pressed down from this frame or previous ones)
		bool repeated = false; //!< Was this key repeated this frame (when you press and hold the key it gets repeated after a while)
		bool released = false; //!< Was this key released this frame?

        void Combine(const KeyState& combine_with)
        {
            pressed = pressed || combine_with.pressed;
            down = down || combine_with.down;
            repeated = repeated || combine_with.repeated;
            released = released || combine_with.released;
        }

	};
    char KeyToString(KEY_TYPE key_type);

	/**
	* @struct tremble::MouseEvent
	* @brief Keyboard key event, that gets created and queued on windows' callback
	* @author Anton Gavrilov
	*/
	struct KeyEvent
	{
		/**
		* @enum KEY_EVENT_TYPE
		* @brief Type of the key event
		*/
		enum KEY_EVENT_TYPE
		{
			KEY_UP, //!< Key got released
			KEY_DOWN //!< Key got pressed
		};
		KEY_EVENT_TYPE event_type; //!< The key event type
		KEY_TYPE key_type; //!< The key that was pressed 
	};

	/**
	* @struct tremble::MouseButtonState
	* @brief Mouse button state
	* @author Anton Gavrilov
	*/
	struct MouseButtonState
	{
		bool pressed; //<! Was this mouse button pressed this frame?
		bool released; //<! Was this mouse button released this frame
		bool down; //<! Is the mouse button down?
		bool dbl; //<! Is the button double clicked?
        void Combine(const MouseButtonState& combine_with)
        {
            pressed = pressed || combine_with.pressed;
            down = down || combine_with.down;
            dbl = dbl || combine_with.dbl;
            released = released || combine_with.released;
            if (combine_with.released == true)
            {
                std::cout << "miracle" << std::endl;
            }
        }
	};

	/**
	* @struct tremble::MouseEvent
	* @brief mouse event, that gets created and queued on windows' callback
	* @author Anton Gavrilov
	*/
	struct MouseEvent
	{
		/**
		* @enum MOUSE_EVENT_TYPE
		* @brief Type of the mouse event
		*/
		enum MOUSE_EVENT_TYPE
		{
			MOUSE_EVENT_DOWN, //!< Mouse button got pressed
			MOUSE_EVENT_UP, //!< Mouse button got released
			MOUSE_EVENT_DBL, //!< Mouse button doubleclick
			MOUSE_EVENT_MOVE, //!< Mouse was moved
			MOUSE_EVENT_WHEEL, //!< Mouse wheel was moved
		};

		MOUSE_BUTTON_TYPE button_type; //!< The button that came with the event
		MOUSE_EVENT_TYPE event_type; //!< The mouse event type
		WPARAM wparam; //!< wparam that came with the event(mouse position) 
		LPARAM lparam; //!< lparam that came with the event(mouse wheel delta)
	};

    class TextComponent;
    class InputState
    {
    public:
        /**
        * @brief function, returning serialized input state (dereferences maps)
        */
        void Serialize(RakNet::BitStream& serialize_to);
        InputState();
        //copy constructor, that "tightens" the data
        InputState(const InputState& input_state);
        InputState(RakNet::BitStream& deserialize_from);
        void Combine(const InputState& combine_with);
        void Clean(); //!< Clean the input state from the mouse delta, and pressed and released buttons
        void PutOnScreen();

        std::unordered_map<KEY_TYPE, KeyState> key_states; //!< All the key states
        std::unordered_map<MOUSE_BUTTON_TYPE, MouseButtonState> mouse_button_states; //!< All the mouse button states
        MouseState mouse_state; //!< Current mouse state
    private:
        TextComponent* input_on_screen_ = nullptr;
    };

	/**
	* @brief A class that listens to the input, holds the state of keys, mouse buttons, and mouse, and provides accessor functions for them
	* @class tremble::InputManager
	* @author Anton Gavrilov
	*/
	class InputManager
	{
	public:
		/**
		* Process windows messages and update input manager
		* @param[in] message The message to be processed
		*/
		void ProcessWinMessage(MSG message); 


		/**
		* Input manager's constructor. 
		* @param[in] window Window, that will use this input manager. This constructor sets window's input listener function to this input manager.
		* @param[in] game_manager game manager object, that owns this input manager
		*/
		InputManager(Window* window, GameManager* game_manager);

		bool GetKeyDown(KEY_TYPE key) const; //!< Get if a key is down during this frame
		bool GetKeyPressed(KEY_TYPE key) const; //!< Get if a key was pressed during this frame
		bool GetKeyReleased(KEY_TYPE key) const; //!< Get if a key is released during this frame
		bool GetKeyRepeated(KEY_TYPE key) const; //!< Get if a key is released during this frame

		bool GetMouseDown(MOUSE_BUTTON_TYPE key) const; //!< Get if a key is down during this frame
		bool GetMousePressed(MOUSE_BUTTON_TYPE key) const; //!< Get if a key was pressed during this frame
		bool GetMouseReleased(MOUSE_BUTTON_TYPE key) const; //!< Get if a key is released during this frame

		DirectX::XMINT2 GetMousePos() const; //!< Get mouse position on the screen (top left is 0)
		DirectX::XMINT2 GetMouseDeltaPos() const; //!< Get the movement of the mouse this frame (right and down are positive)
		float GetMouseDeltaWheel() const; //!< Get movement of the mouse wheel (up is positive)

		void Update(); //!< Frame update of the input manager

		void HideCursor();
		void UnHideCursor();
		bool isCursorHidden();
		void LockCursor();
		bool IsCursorLocked();
		void UnlockCursor();

        ///////////////////////////VIRTUAL INPUT
        const std::unordered_map<KEY_TYPE, KeyState>& GetKeyStates();
        const std::unordered_map<MOUSE_BUTTON_TYPE, MouseButtonState>& GetMouseButtonStates();
        const MouseState& GetMouseState();
        const InputState& GetOwnInputState() const;

        /**
        * @brief Set virtual input of a player id
        */
        void UseInputOfPeer(PeerID player_id);
        void UseRealInput();
        void StoreInput(PeerID associate_with, InputState input_to_store); //!< Store input until further use
        void SetVirtualInput(PeerID player_id, const InputState& inputState); //!< Set virtual input of a certain player
        void CombineVirtualInput(PeerID player_id, const InputState& inputState); //!< Combine current existing virtual input of a player with the new input

		void SetCanSendInput(bool can_send_input);
        void SendInput();
	private:
        InputState* current_input_;
		bool can_send_input_ = false;
		bool is_cursor_hidden_ = false;
		bool is_cursor_locked_ = false; //!< Is the cursor locked at current?
		POINT window_center_point_;
		DirectX::XMINT2 window_center_local_;

		std::queue<KeyEvent> key_events_; //!< Key events, queued for processing
		std::queue<MouseEvent> mouse_events_; //!< Mouse events, queued for processing

        InputState own_input_state_;

		Window* window_; //!< window, that uses this input listener
		GameManager* game_manager_; //!< game manager object, that owns this input manager

		void ProcessEvents(); //!< Process queued mouse and key events


        //std::unordered_map<>

        //////////////////////////////////////VIRTUAL INPUT
        std::unordered_map<PeerID, InputState> peers_input_; //Input states associated with peer ids, that are used when running components, associated with them;
        std::unordered_map<PeerID, InputState> peers_input_storage_; //Input states, that come in, are stored here, until activated at the start of the loop
	
    };
}