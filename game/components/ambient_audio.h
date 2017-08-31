#pragma once
#include "tremble/components/audio/audio_source.h"
#include "tremble/core/audio/audio_clip.h"

namespace tremble
{
    class AmbientAudio : public Component
    {
    public:
        AudioSource * audio;
        float volume = 1.0f;
        float pitch = 1.0f;
        bool paused = false;

        void Start()
        {
            audio = GetNode()->FindComponent<AudioSource>();
            AudioClip* clip = Get::ResourceManager()->GetAudioClip("CANON.mp3");
            audio->SetAudioClip(clip);
            audio->Play();
        }

        void Update()
        {
            if (Get::InputManager()->GetKeyPressed(TYPE_KEY_P))
            {
                if (paused)
                {
                    paused = false;
                }
                else
                {
                    paused = true;
                }
                audio->TogglePaused(paused);
            }

            if (Get::InputManager()->GetKeyPressed(TYPE_KEY_V))
            {
                audio->Enable3DAudio(Vector3(0, 0, 0), Vector3(0, 0, 0));
            }
        }

        void Set3DAttributes()
        {

        }
    };

}