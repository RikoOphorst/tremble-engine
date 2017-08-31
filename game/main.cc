#include "components\player.h"
#include "components\flying.h"
#include "components/fps_counter.h"
#include "components/grappling_hook.h"
#include "components/pick_up.h"
#include "components/jump_pad.h"
#include "components/collision_test.h"

#include "networking\network_event_handler.h"
#include "components/death_box.h"

using namespace tremble;
#include "networking_demo.h"

static void AddPickUp(SGNode*, TagData&);

static void AddSpawnpoint(SGNode*, TagData&);

class PointLightController : public Component
{
public:
    void Update()
    {
        GetNode()->SetLocalPosition(
            Vector3(
                GetNode()->GetLocalPosition().GetX(),
                GetNode()->GetLocalPosition().GetY(),
                GetNode()->GetLocalPosition().GetZ() + (Scalar(50.0f) * Scalar(Get::DeltaT()))
            )
        );

        if (GetNode()->GetLocalPosition().GetZ() > 30.0f)
        {
            GetNode()->SetLocalPosition(
                Vector3(
                    GetNode()->GetLocalPosition().GetX(),
                    GetNode()->GetLocalPosition().GetY(),
                    Scalar(-30.0f)
                )
            );
        }
    }
};

class SinMover1 : public Component
{
public:
    void Update()
    {
        GetNode()->SetLocalPosition(
            Vector3(
                Scalar(std::sin(Get::TimeSinceStartup()) * 2.0f),
                GetNode()->GetLocalPosition().GetY(),
                Scalar(std::cos(Get::TimeSinceStartup()) * 2.0f)
            )
        );

        //GetNode()->SetLocalRotationRadians(
        //	Vector3(
        //		Scalar(std::sin(Get::TimeSinceStartup())),
        //		GetNode()->GetLocalRotationRadians().GetY(),
        //		GetNode()->GetLocalRotationRadians().GetZ()
        //	)
        //);
    }
};

class AnimationChanger : public Component
{
public:
    void Start()
    {
        renderable_ = GetNode()->FindComponent<SkinnedRenderable>();
        i = 0;
    }
    void Update()
    {
        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_N))
        {
            renderable_->PlayAnimation(i++);
        }
    }
private:
    int i;
    SkinnedRenderable* renderable_;
};

int main(int argc, char* argv)
{
    Stopwatch startup_watch("startup in main");
    startup_watch.Reset();

    GameManager* game_manager = GameManager::Create(1000000000);
    startup_watch.PrintAndReset("GameManager::Create");

    game_manager->Startup("Duty Calls: The FPS", 1280, 720);
    startup_watch.PrintAndReset("GameManager::Startup");

    if (Get::Config().spawn_clients)
    {
        for (UINT i = 0; i < Get::Config().num_clients; i++)
        {
            TCHAR pBuf[255];
            GetModuleFileName(NULL, pBuf, 255);

            STARTUPINFO info = { sizeof(info) };
            PROCESS_INFORMATION processInfo;
            if (CreateProcess(pBuf, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
            {
                std::cout << "spawned " << i << std::endl;
            }
        }
    }

    // Set up the default camera
    SGNode* default_camera = Get::Scene()->AddChild(false, Vector3(0.0f, 0.4f, 0.0f));
    Camera* default_camera_component = default_camera->AddComponent<Camera>();
    Get::Renderer()->SetCamera(default_camera_component);

    if (!Get::Config().god_mode)
    {
        //player->AddComponent<CharacterController>();
        //player->AddComponent<Player>();
        //player->AddComponent<GrapplingHook>();
    }

    if (Get::Config().load_scene)
    {
        Get::SceneLoader()->RegisterTag(GrapplingHook::AddGrapplePoints, "grapple");
        Get::SceneLoader()->RegisterTag(AddPickUp, "PickUp");
        Get::SceneLoader()->RegisterTag(AddSpawnpoint, "Spawnpoint");
        Get::SceneLoader()->RegisterTag(JumpPad::AddJumpPads, "jumpPad");
        Get::SceneLoader()->RegisterTag(DeathBox::AddDeaathBox, "deathBox");
        if (Get::Config().master_volume != 0)
        {
            Get::ResourceManager()->LoadAudioClip("footsteps.mp3");
            Get::ResourceManager()->LoadAudioClip("grapple_sound.mp3");
            Get::ResourceManager()->LoadAudioClip("grapple_sound_fin.mp3");
            Get::ResourceManager()->LoadAudioClip("grenade_explosion.mp3");
            Get::ResourceManager()->LoadAudioClip("grenade_hit.mp3");

            Get::ResourceManager()->LoadAudioClip("GunSounds/laser_end_sound.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/laser_firing_sound.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/laser_reload.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/machinegun_fire.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/pistol_fire.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/pistol_reload.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/shotgun_fire.mp3");
            Get::ResourceManager()->LoadAudioClip("GunSounds/shotgun_reload.mp3");
        }
        if (Get::Config().host) //otherwise, the networking demo should load the host's scene
        {
            Get::SceneLoader()->LoadScene(Get::ConfigManager()->GetScenes()[Get::Config().scene]);
        }

        //for (int x = 0; x < 2; x++)
        //{
        //	for (int z = 0; z < 2; z++)
        //	{
        //		SGNode* node = Get::Scene()->AddChild();
        //		node->SetLocalPosition(Vector3(10.0f * x + 50.0f, 0.0f, 10.0f * z + 50.0f));
        //		node->SetLocalScale(Vector3(0.1f, 0.1f, 0.1f));
        //		node->AddComponent<AnimationChanger>();
        //		SkinnedRenderable* node_renderable = node->AddComponent<SkinnedRenderable>();
        //		node_renderable->SetModel(Get::ResourceManager()->GetModel("ArmyPilot/ArmyPilotRetake.fbx"));
        //		node_renderable->PlayAnimation(0);
        //	}
        //}
    }
    else
    {
        SGNode* n = Get::Scene()->AddChild();
        n->SetLocalScale(Vector3(0.05f, 0.05f, 0.05f));
        ////Renderable* n_renderable = n->AddComponent<Renderable>();
        ////n_renderable->SetModel(Get::ResourceManager()->GetModel("crytek-sponza-obj/sponza.obj"));
		//
        Light* l = n->AddComponent<Light>();
        l->SetLightType(LightTypeDirectional);
        l->SetDirection(Vector3(0.0f, -1.0f, -1.0f));
        l->SetColor(Vector4(1.0f, 1.0f, 1.0f));
		
        //l = n->AddComponent<Light>();
        //l->SetLightType(LightTypeDirectional);
        //l->SetDirection(Vector3(0.5f, -1.0f, 0.05f));
        //l->SetColor(Vector4(0.5f, 0.5f, 0.0f));
        //l->SetShadowCasting(true);

        for (int x = 0; x < 15; x++)
        {
            for (int z = 0; z < 10; z++)
            {
                SGNode* node = Get::Scene()->AddChild();
                node->SetLocalPosition(Vector3(5.0f * x - 5.0f, 0.0f, 5.0f * z - 5.0f));
                node->SetLocalScale(Vector3(0.05f, 0.05f, 0.05f));
                node->AddComponent<AnimationChanger>();
                SkinnedRenderable* node_renderable = node->AddComponent<SkinnedRenderable>();
                node_renderable->SetModel(Get::ResourceManager()->GetModel("ArmyPilot/ArmyPilotNoGun.fbx"));
                node_renderable->PlayAnimation(0);
            }
        }
    }

    if (!Get::Config().god_mode)
    {
        //player->FindComponent<Player>()->Spawn(); 
        Get::InputManager()->LockCursor();
    }
    else
    {
        //player->SetLocalPosition(Vector3(-7.664f, 16.7168f, -23.4085f));
        //player->SetLocalRotationDegrees(Vector3(29.2163f, 11.425f, -0.000011057f));
        //Flying* flying = player->AddComponent<Flying>();
    }

    startup_watch.PrintAndReset("Scene loading");

    Get::Scene()->AddChild()->AddComponent<FPSCounter>();

    if (Get::Config().physics_base_plane)
    {
        SGNode* plane = Get::Scene()->AddChild();
        plane->AddComponent<PhysicsStaticPlane>(Vector3(0, 1, 0));
    }

    startup_watch.PrintAndReset("Player setup");

    SGNode* light_group = Get::Scene()->AddChild();

    if (Get::Config().light_demo)
    {
        srand(time(NULL));
        for (int i = 0; i < Get::Config().num_lights; i++)
        {
            SGNode*					point_light_node = light_group->AddChild();
            point_light_node->SetLocalPosition(Vector3((rand() % 120) - 60.0f, (rand() % 40), (rand() % 60) - 30));

            Light*					point_light_component1 = point_light_node->AddComponent<Light>();
            PointLightController*	point_light_controller = point_light_node->AddComponent<PointLightController>();

            point_light_component1->SetLightType(LightTypePoint);
            point_light_component1->SetColor(
                Vector4(
                    static_cast<float>(rand() % 100) / 200.0f,
                    static_cast<float>(rand() % 100) / 200.0f,
                    static_cast<float>(rand() % 100) / 200.0f,
                    1.0f
                )
            );
            point_light_component1->SetFalloffEnd(Scalar(25.0f));
        }
    }
    startup_watch.PrintAndReset("Lights setup");

    // Setup particle system for bullet holes
    SGNode* bullet_hole_ps = Get::Scene()->AddChild();
    bullet_hole_ps->SetPosition({ 0, 0, 0 });
    BaseGun::bullet_hole_ = bullet_hole_ps->AddComponent<ParticleSystem>();
    Texture* tex = Get::ResourceManager()->GetTexture("hole.png");
    BaseGun::bullet_hole_->SetTexture(tex);
    BaseGun::bullet_hole_->SetSpace(World);

    NetworkingDemo* demo = new NetworkingDemo;

    startup_watch.PrintAndReset("Network demo setup");

    demo->CreateStartGameListenerNode();

    game_manager->MainLoop();

    delete demo;

    game_manager->ShutDown();

    GameManager::Delete(game_manager);

    return 0;
}

static void AddPickUp(SGNode* node, TagData& tagData)
{
    PickUp* temp = node->AddComponent<PickUp>();
    node->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(1, 1, 1)));
    temp->pick_up_type_ = tagData.Strings["type"];

    if (tagData.Floats["RespawnTime"])
    {
        temp->respawn_time_ = tagData.Floats["RespawnTime"];
    }

    temp->amount = tagData.Integers["value"];
}

static void AddSpawnpoint(SGNode* node, TagData& tagData)
{
    Player::spawnpoints.push_back(node);
}