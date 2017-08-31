#pragma once
namespace tremble
{
    class InvokerTimer : public Component
    {
    public:
        float timer = 0;
        float duration = 1;

        const Component* other_component_;

        bool completed = false;

        /// Assigns function for on completion event
        void SetOnComplete(const Component& other_component, std::function<void(const Component&)> on_complete)
        {
            on_complete_ = on_complete;
            other_component_ = &other_component;
        }

        void Start()
        {

        }
        void Update()
        {
            if (completed == false)
            {
                timer += Get::DeltaT();

                if (timer >= duration)
                {
                    completed = true;
                    on_complete_(*other_component_);
                }
            }
        }

        std::function<void(const Component& other_component)> on_complete_;
    };
}