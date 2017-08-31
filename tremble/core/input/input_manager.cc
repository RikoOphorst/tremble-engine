#include "input_manager.h"
#include "../../core/game_manager.h"
#include "../utilities/utilities.h"
#include "../dependencies/raknet/include/BitStream.h"
#include "../networking/network_manager.h"
#include "../networking/packet_factory.h"
#include "../networking/peer_factory.h"
#include "../networking/packet_handlers/input_packet_handler.h"
#include "../networking/packet_identifiers.h"
#include "../win32/window.h"
#include "../get.h"

#include "../components/rendering/text_component.h"
#include "../scene_graph/scene_graph.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	InputManager::InputManager(Window* window, GameManager* game_manager)
		:window_(window), game_manager_(game_manager), current_input_(&own_input_state_)
	{
		std::function<void(MSG message)> message = std::bind(&InputManager::ProcessWinMessage, this, std::placeholders::_1);
		window_->SetWindowInputListener(message);
	}

	//------------------------------------------------------------------------------------------------------
	void InputManager::ProcessWinMessage(MSG message)
	{
		switch (message.message)
		{
		case WM_KEYDOWN:
		{
			if (message.wParam == VK_ESCAPE)
			{
				game_manager_->StopRunning();
			}
			key_events_.push(KeyEvent{ KeyEvent::KEY_DOWN, static_cast<KEY_TYPE>(message.wParam) });
			break;
		}
		case WM_KEYUP:
		{
			key_events_.push(KeyEvent{ KeyEvent::KEY_UP, static_cast<KEY_TYPE>(message.wParam) });
			break;
		}
		case WM_LBUTTONDOWN:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_LEFT, MouseEvent::MOUSE_EVENT_DOWN, message.wParam, message.lParam });
			break;
		case WM_LBUTTONUP:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_LEFT, MouseEvent::MOUSE_EVENT_UP, message.wParam, message.lParam });
			break;
		case WM_LBUTTONDBLCLK:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_LEFT, MouseEvent::MOUSE_EVENT_DBL, message.wParam, message.lParam });
			break;
		case WM_RBUTTONDOWN:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_RIGHT, MouseEvent::MOUSE_EVENT_DOWN, message.wParam, message.lParam });
			break;
		case WM_RBUTTONUP:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_RIGHT, MouseEvent::MOUSE_EVENT_UP, message.wParam, message.lParam });
			break;
		case WM_RBUTTONDBLCLK:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_RIGHT, MouseEvent::MOUSE_EVENT_DBL, message.wParam, message.lParam });
			break;
		case WM_MBUTTONDOWN:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_MIDDLE, MouseEvent::MOUSE_EVENT_DOWN, message.wParam, message.lParam });
			break;
		case WM_MBUTTONUP:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_MIDDLE, MouseEvent::MOUSE_EVENT_UP, message.wParam, message.lParam });
			break;
		case WM_MBUTTONDBLCLK:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_MIDDLE, MouseEvent::MOUSE_EVENT_DBL, message.wParam, message.lParam });
			break;
		case WM_MOUSEMOVE:
			if (is_cursor_locked_)
			{
				DirectX::XMINT2 new_mouse_position = DirectX::XMINT2(static_cast<float>(((int)(short)LOWORD(message.lParam))), static_cast<float>(((int)(short)HIWORD(message.lParam))));
				if (new_mouse_position.x != window_center_local_.x || new_mouse_position.y != window_center_local_.y)
				{
					POINT new_center = window_center_point_;
					ClientToScreen(Get::Window()->GetWindowHandle(), &new_center);
					SetCursorPos(new_center.x, new_center.y);
				}
			}
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_NULL, MouseEvent::MOUSE_EVENT_MOVE, message.wParam, message.lParam });
			break;
		case WM_MOUSEWHEEL:
			mouse_events_.push(MouseEvent{ MOUSE_BUTTON_NULL, MouseEvent::MOUSE_EVENT_WHEEL, message.wParam, message.lParam });
			break;
		}
	}

    //------------------------------------------------------------------------------------------------------
    void InputManager::SetVirtualInput(PeerID player_id, const InputState& input_state)
    {
        peers_input_[player_id] = input_state;
    }

    //------------------------------------------------------------------------------------------------------
    void InputManager::CombineVirtualInput(PeerID player_id, const InputState& inputState)
    {
        peers_input_[player_id].Combine(inputState);
    }

    //------------------------------------------------------------------------------------------------------
	void InputManager::SetCanSendInput(bool can_send_input)
	{
		can_send_input_ = can_send_input;
	}

	//------------------------------------------------------------------------------------------------------
    void InputManager::SendInput()
    {
        if (!can_send_input_ || Get::NetworkManager()->IsHost())
        {
            return;
        }

        RakNet::BitStream* packet_stream = Get::PacketFactory()->CreatePacket(ID_INPUT_PACKET);
        
        InputState condensed_input(GetOwnInputState());
        condensed_input.Serialize(*packet_stream);

        Get::NetworkManager()->SendPacket(Get::NetworkManager()->GetPeerFactory()->GetHostingPeer(), packet_stream);

        Get::PacketFactory()->DeletePacket(packet_stream);
    }

    void InputState::PutOnScreen()
    {
        if (input_on_screen_ == nullptr)
        {
            input_on_screen_ = Get::Scene()->AddComponent<TextComponent>();
        }
        input_on_screen_->SetFont("../../font/5thagent.ttf");
        input_on_screen_->SetSize(50.0f);
        input_on_screen_->SetCenter({ 0.0f, 0.0f, 0 });
        input_on_screen_->SetPosition(Vector3(1000.0f, 500.0f, 0));
        input_on_screen_->SetColor({ 1, 1, 0 ,1 });
        input_on_screen_->SetLayer(1);

        std::string input_text;


        for each (std::pair<KEY_TYPE, KeyState> i in key_states)
        {
            if (i.second.down == true)
            {
                input_text += KeyToString(i.first);
            }
        }
        input_on_screen_->SetText(input_text);
    }

    //------------------------------------------------------------------------------------------------------
    void InputManager::UseInputOfPeer(PeerID player_id)
    {
        current_input_ = &peers_input_[player_id];
        int number_down = 0;
        for each (auto i in current_input_->key_states)
        {
            if (i.second.down == true)
            {
                number_down++;
            }
        }
        /*if (number_down > 0)
        {
            //std::cout << Get::DeltaT() << "Using input of active peer" << number_down << std::endl;
        }*/
    }

	//------------------------------------------------------------------------------------------------------
	void InputManager::Update()
	{
        //OWN INPUT

		//Clear the per frame pressed and released values
        own_input_state_.Clean();
		//Process input events
		ProcessEvents();

        //OTHERS INPUT

        //Put the received input to use. 
        for each (std::pair<PeerID, InputState> received_input in peers_input_storage_)
        {
            peers_input_[received_input.first] = received_input.second;
        }
        //If not received, clean up input, that is already there (set pressed and released to false. Keep the pressed keys)
        for each (std::pair<PeerID, InputState> existing_input in peers_input_storage_)
        {
            if (peers_input_storage_.find(existing_input.first) == peers_input_storage_.end())
            {
                existing_input.second.Clean();
            }
        }
        peers_input_storage_.clear();
	}

	//------------------------------------------------------------------------------------------------------
	void InputManager::HideCursor()
	{
		if (!is_cursor_hidden_)
		{
			ShowCursor(false);
		}
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::isCursorHidden()
	{
		return is_cursor_hidden_;
	}

	//------------------------------------------------------------------------------------------------------
	void InputManager::UnHideCursor()
	{
		ShowCursor(true);
	}

	//------------------------------------------------------------------------------------------------------
	void InputManager::LockCursor()
	{
		int vertical_center = (Get::Window()->GetHeight() / 2);
		int horizontal_center = (Get::Window()->GetWidth() / 2);
		window_center_point_ = { horizontal_center, vertical_center };
		window_center_local_ = DirectX::XMINT2(horizontal_center, vertical_center);
		is_cursor_locked_ = true;
		int window_x, window_y;
		Get::Window()->GetPosition(window_x, window_y);
		RECT window_rect = RECT{ (LONG)window_x, (LONG)window_y, (LONG)window_x + (LONG)Get::Window()->GetWidth() - 1, (LONG)window_y + (LONG)Get::Window()->GetHeight() - 1 };
		ClipCursor(&window_rect);
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::IsCursorLocked()
	{
		return is_cursor_locked_;
	}

	//------------------------------------------------------------------------------------------------------
	void InputManager::UnlockCursor()
	{
		is_cursor_locked_ = false;
		ClipCursor(NULL);
	}

    //------------------------------------------------------------------------------------------------------
    const std::unordered_map<KEY_TYPE, KeyState>& InputManager::GetKeyStates()
    {
        return current_input_->key_states;
    }

    //------------------------------------------------------------------------------------------------------
    const std::unordered_map<MOUSE_BUTTON_TYPE, MouseButtonState>& InputManager::GetMouseButtonStates()
    {
        return current_input_->mouse_button_states;
    }

    //------------------------------------------------------------------------------------------------------
    const MouseState& InputManager::GetMouseState()
    {
        return current_input_->mouse_state;
    }

    //------------------------------------------------------------------------------------------------------
    const InputState& InputManager::GetOwnInputState() const 
    {
        return own_input_state_;
    }

    //------------------------------------------------------------------------------------------------------
    void InputManager::UseRealInput()
    {
        current_input_ = &own_input_state_;
    }

    //------------------------------------------------------------------------------------------------------
    void InputManager::StoreInput(PeerID associate_with, InputState input_to_store)
    {
        if (peers_input_storage_.find(associate_with) != peers_input_storage_.end())
        {
            peers_input_storage_[associate_with].Combine(input_to_store);
            return;
        }
        peers_input_storage_[associate_with] = input_to_store;
    }

	//------------------------------------------------------------------------------------------------------
	void InputManager::ProcessEvents()
	{
		//Process Key events
		uint size = static_cast<uint>(key_events_.size());
		for (int i = 0; i < static_cast<int>(size); i++)
		{
			KeyEvent& key_event = key_events_.front();
			if (key_event.event_type == KeyEvent::KEY_DOWN)
			{
				if (own_input_state_.key_states[key_event.key_type].down == true)
				{
                    own_input_state_.key_states[key_event.key_type].repeated = true;
				}
				else
				{
                    own_input_state_.key_states[key_event.key_type].pressed = true;
                    own_input_state_.key_states[key_event.key_type].down = true;
				}
			}
			else //KEY_UP
			{
                own_input_state_.key_states[key_event.key_type].released = true;
                own_input_state_.key_states[key_event.key_type].down = false;
                own_input_state_.key_states[key_event.key_type].repeated = false;
			}
			key_events_.pop();
		}

		//Process Mouse Events
		size = static_cast<uint>(mouse_events_.size());
		for (int i = 0; i < static_cast<int>(size); i++)
		{
			MouseEvent& mouse_event = mouse_events_.front();

			switch (mouse_event.event_type)
			{
			case MouseEvent::MOUSE_EVENT_DOWN:
			{
				MouseButtonState& mouse_button_state = own_input_state_.mouse_button_states[mouse_event.button_type];
				mouse_button_state.pressed = true;
				mouse_button_state.down = true;
			}
			break;
			case MouseEvent::MOUSE_EVENT_UP:
			{
				MouseButtonState& mouse_button_state = own_input_state_.mouse_button_states[mouse_event.button_type];
				mouse_button_state.released = true;
				mouse_button_state.down = false;
			}
			break;
			case MouseEvent::MOUSE_EVENT_DBL:
			{
				MouseButtonState& mouse_button_state = own_input_state_.mouse_button_states[mouse_event.button_type];
				mouse_button_state.dbl = true;
			}
			break;
			case MouseEvent::MOUSE_EVENT_MOVE:
			{
				window_->GetWindowHandle();

				//The LOWORD HIWORD stuff is actually copied from <windowsx.h> just to not include the whole thing here
				DirectX::XMINT2 new_mouse_position = DirectX::XMINT2(static_cast<float>(((int)(short)LOWORD(mouse_event.lparam))), static_cast<float>(((int)(short)HIWORD(mouse_event.lparam))));

                own_input_state_.mouse_state.delta_position.x += new_mouse_position.x - own_input_state_.mouse_state.position.x;
                own_input_state_.mouse_state.delta_position.y += new_mouse_position.y - own_input_state_.mouse_state.position.y;

				if (is_cursor_locked_ == false)
				{
                    own_input_state_.mouse_state.position = new_mouse_position;
				}
				else
				{
                    own_input_state_.mouse_state.position = window_center_local_;
				}
			}
			break;
			case MouseEvent::MOUSE_EVENT_WHEEL:
			{
                own_input_state_.mouse_state.delta_wheel = static_cast<float>(GET_WHEEL_DELTA_WPARAM(mouse_event.wparam)) / WHEEL_DELTA;
			}
			break;
			}
			mouse_events_.pop();
		}
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetKeyDown(KEY_TYPE key) const
	{
		return current_input_->key_states[key].down;
		//return key_states_[key].down;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetKeyPressed(KEY_TYPE key) const
	{
		return current_input_->key_states[key].pressed;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetKeyReleased(KEY_TYPE key) const
	{
		return current_input_->key_states[key].released;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetKeyRepeated(KEY_TYPE key) const
	{
		return current_input_->key_states[key].repeated;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetMouseDown(MOUSE_BUTTON_TYPE key) const
	{
		return current_input_->mouse_button_states[key].down;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetMousePressed(MOUSE_BUTTON_TYPE key) const
	{
		return current_input_->mouse_button_states[key].pressed;
	}

	//------------------------------------------------------------------------------------------------------
	bool InputManager::GetMouseReleased(MOUSE_BUTTON_TYPE key) const
	{
		return current_input_->mouse_button_states[key].released;
	}

	//------------------------------------------------------------------------------------------------------
	DirectX::XMINT2 InputManager::GetMousePos() const
	{
		return current_input_->mouse_state.position;
	}

	//------------------------------------------------------------------------------------------------------
	DirectX::XMINT2 InputManager::GetMouseDeltaPos() const
	{
		return current_input_->mouse_state.delta_position;
	}

	//------------------------------------------------------------------------------------------------------
	float InputManager::GetMouseDeltaWheel() const
	{
		return current_input_->mouse_state.delta_wheel;
	}
    //------------------------------------------------------------------------------------------------------
    void InputState::Serialize(RakNet::BitStream& serialize_to)
    {
        serialize_to.Write(mouse_state);
        serialize_to.Write(key_states.size());
        for (auto i : key_states)
        {
            serialize_to.Write((unsigned char)i.first);
            serialize_to.Write(i.second);
        }
        serialize_to.Write(mouse_button_states.size());
        for (auto i : mouse_button_states)
        {
            serialize_to.Write((unsigned char)i.first);
            serialize_to.Write(i.second);
        }
    }

    InputState::InputState()
    {

    }

    //------------------------------------------------------------------------------------------------------
    InputState::InputState(const InputState& input_state)
        :mouse_state(input_state.mouse_state)
    {
        for (auto i : input_state.key_states)
        {
            if (i.second.down || i.second.pressed || i.second.released || i.second.repeated)
            {
                key_states[i.first] = i.second;
            }
        }
        for (auto i : input_state.mouse_button_states)
        {
            if (i.second.down || i.second.pressed || i.second.released || i.second.dbl)
            {
                mouse_button_states[i.first] = i.second;
            }
        }
    }

    //------------------------------------------------------------------------------------------------------
    InputState::InputState(RakNet::BitStream& deserialize_from)
    {
        size_t amount_serialized_buttons;
        deserialize_from.Read(mouse_state);
        deserialize_from.Read(amount_serialized_buttons);
        //std::cout << "keys : " << amount_serialized_buttons << " ";
        unsigned char key_type;
        KeyState key_state;
        for (int i = 0; i < amount_serialized_buttons; i++)
        {
            deserialize_from.Read(key_type);
            deserialize_from.Read(key_state);
            key_states[(KEY_TYPE)key_type] = key_state;
        }
        deserialize_from.Read(amount_serialized_buttons);
        //std::cout << "mouse buttons : " << amount_serialized_buttons << std::endl;
        unsigned char button_type;
        MouseButtonState button_state;
        for (int i = 0; i < amount_serialized_buttons; i++)
        {
            deserialize_from.Read(button_type);
            deserialize_from.Read(button_state);
            mouse_button_states[(MOUSE_BUTTON_TYPE)button_type] = button_state;
        }
    }

    void InputState::Combine(const InputState& combine_with)
    {
        for each (std::pair<KEY_TYPE, KeyState> i in key_states)
        {
            //if the key, that local machine has, is not in the received input, remove the continuus states from it
            //(down for example). This is needed, because if the other machine didn't send over that key, no statuses
            //are true in it. We don't remove the momentous statuses(pressed or released) because we clean them up
            //at the start of the game loop (input manager update)
            if (combine_with.key_states.find(i.first) == combine_with.key_states.end())
            {
                i.second.down = false;
                i.second.repeated = false;
            }
        }

        for each (std::pair<MOUSE_BUTTON_TYPE, MouseButtonState> i in mouse_button_states)
        {
            if (combine_with.mouse_button_states.find(i.first) == combine_with.mouse_button_states.end())
            {
                i.second.down = false;
            }
        }

        for each (std::pair<KEY_TYPE, KeyState> i in combine_with.key_states)
        {
            key_states[i.first].Combine(i.second);
        }
        for each (std::pair<MOUSE_BUTTON_TYPE, MouseButtonState> i in combine_with.mouse_button_states)
        {
            mouse_button_states[i.first].Combine(i.second);
        }
        mouse_state.delta_position.x += combine_with.mouse_state.delta_position.x;
        mouse_state.delta_position.y += combine_with.mouse_state.delta_position.y;
        mouse_state.delta_wheel += combine_with.mouse_state.delta_wheel;
    }

    void InputState::Clean()
    {
        uint size = static_cast<uint>(key_states.size());
        for (auto iter = key_states.begin(); iter != key_states.end(); iter++)
        {
            iter->second.pressed = false;
            iter->second.released = false;
        }

        size = static_cast<uint>(mouse_button_states.size());
        for (auto iter = mouse_button_states.begin(); iter != mouse_button_states.end(); iter++)
        {
            iter->second.dbl = false;
            iter->second.pressed = false;
            iter->second.released = false;
        }

        //Clear mouse delta values
        mouse_state.delta_position = DirectX::XMINT2(0.0f, 0.0f);
        mouse_state.delta_wheel = 0.0f;
    }

    char KeyToString(KEY_TYPE key_type)
    {
        return key_names[key_type];
    }

}