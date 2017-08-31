#include "cube_movement.h"
#include "tremble/core/get.h"

using namespace tremble;

void CubeMovement::Awake()
{

}

void CubeMovement::Update()
{
    if (Get::InputManager()->GetKeyDown(TYPE_KEY_W)) //up
    {
        GetNode()->Move(Vector3(0, 1 * Get::DeltaT(), 0));
    }
    else if (Get::InputManager()->GetKeyDown(TYPE_KEY_S)) //down
    {
        GetNode()->Move(Vector3(0, -1 * Get::DeltaT(), 0));
    }

    if (Get::InputManager()->GetKeyDown(TYPE_KEY_A)) //left
    {
        GetNode()->Move(Vector3(-1 * Get::DeltaT(), 0, 0));
    }
    else if (Get::InputManager()->GetKeyDown(TYPE_KEY_D)) //right
    {
        GetNode()->Move(Vector3(1 * Get::DeltaT(), 0, 0));
    }
}
