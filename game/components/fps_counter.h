namespace tremble
{
    class FPSCounter : public Component
    {
    public:
        void Start()
        {
            text_ = GetNode()->AddComponent<TextComponent>();
            text_->SetFont("../../font/5thagent.ttf");
            text_->SetSize(50.0f);
            text_->SetCenter({ 0.0f, 0.0f, 0 });
            text_->SetPosition(Vector3(0.0f, 0.0f, 0));
            text_->SetColor({ 1, 1, 0 ,1 });
            text_->SetLayer(1);
            fps_index_ = 0;

            for (int i = 0; i < 10; i++)
            {
                fpses_[i] = 0.0;
            }
        }

        void Update()
        {
            fps_index_ = (fps_index_ + 1) % 10;

            fpses_[fps_index_] = Get::DeltaT();

            double average = 0.0f;
            double accum = 0;
            for (int i = 0; i < 10; i++)
            {
                accum += fpses_[i];
            }

            average = accum / 10;

            time_elapsed_ += Get::DeltaT();
            if (time_elapsed_ > 0.1)
            {
                text_->SetText(std::to_string(1.0 / average));
                time_elapsed_ = 0;
            }
        }

    private:
        TextComponent* text_;
        int fps_index_ = 0;
        double fpses_[10];
        double time_elapsed_ = 0;
    };
}