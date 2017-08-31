#include "start_game_listener.h"
#include "networking_demo.h"

namespace tremble
{
    void StartGameListener::Start()
    {
        if (Get::NetworkManager()->IsHost())
        {
            //TODO: Create press space to start playing UI text.
        }

        //TODO: Create players ready UI text.
    }

    void StartGameListener::Update()
    {
        if (NetworkingDemo::GetNetworkingDemo().GameHasStarted())
        {
            return;
        }

        //TODO: Update players ready UI text.


        if (!Get::NetworkManager()->IsHost())
        {
            return;
        }

        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_ENTER))
        {
            has_pressed_start_ = true;
        }

        if (has_pressed_start_)
        {
            if (NetworkingDemo::GetNetworkingDemo().StartGame())
            {
                // Reset.
                has_pressed_start_ = false;
            }
            else
            {
                std::cout << "Unable to start game, not all player data received yet." << std::endl;
            }
        }
    }
}