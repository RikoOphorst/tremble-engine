#pragma once
#include "tremble/components/audio/audio_source.h"
#include "tremble/core/audio/audio_clip.h"

namespace tremble
{
    class ShootingSound : public Component
    {
    public:
        AudioSource * audio;
        float volume = 1.0f;
        float pitch = 1.0f;

        void Start()
        {
            audio = GetNode()->FindComponent<AudioSource>();

            AudioClip* clip = Get::ResourceManager()->GetAudioClip("laser_gunshot.mp3");
            audio->SetAudioClip(clip);

        }
        void Update()
        {
            if (Get::InputManager()->GetMousePressed(MOUSE_BUTTON_LEFT))
            {
                audio->Play();
            }
        }

        void Shutdown() override
        {
            delete audio->GetAudioClip();
        }
    };
}

