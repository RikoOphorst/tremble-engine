#include "particle_system.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../components/rendering/camera.h"

namespace tremble {

	float Lerp(float a, float b, float t)
	{
		return (1 - t)*a + t*b;
	}

	//------------------------------------------------------------------------------------------------------
	Vector4 Lerp(Vector4 a, Vector4 b, float t)
	{
		return a * (1 - t) + b * t;
	}

	//------------------------------------------------------------------------------------------------------
	ParticleSystem::ParticleSystem()
	{
		texture_ = nullptr;
		renderables_ = nullptr;

		particle_barrier_ = 200;
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleSystem::Spawn(ParticleDescription& p)
	{
		Particle particle;
		particle.age = 0;
		particle.position = (space_ == World) ? p.position : (p.position - GetNode()->GetPosition());
		particle.velocity = p.velocity;
		particle.lifetime = p.lifetime;
		particle.color = p.color;
		particle.colorEnd = p.colorEnd;
		particle.size = p.size;
		particle.sizeEnd = p.sizeEnd;

		particles_.push_back(particle);
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleSystem::Update(Camera* camera)
	{
		float deltaTime = Get::DeltaT();

		Vector3 pos = GetNode()->GetPosition();

		for (int i = 0; i < emitters_.size(); i++) {
			emitters_[i]->Update();
		}

		if (renderables_ == nullptr || buffer_size_ < particles_.size() || (buffer_size_ - particle_barrier_) > particles_.size())
		{
			CreateBuffer();
		}

		float time;
		bool sort = false;

		// Update particles
		for (int i = 0; i < particles_.size(); i++) {
			particles_[i].age += deltaTime;
			if (particles_[i].lifetime == -1) particles_[i].age = 0;
			if (particles_[i].age > particles_[i].lifetime && particles_[i].lifetime != -1) { particles_.erase(particles_.begin() + i); continue; }

			particles_[i].position += particles_[i].velocity * deltaTime;

			if (renderables_[i].Color.w != 1 && renderables_[i].Color.w != 0) sort = true;
		}

		// Set renderable data
		for (int i = 0; i < particles_.size(); i++) {
			time = particles_[i].age / particles_[i].lifetime;

			renderables_[i].Position = particles_[i].position + ((space_ == System) ? pos : Vector3(0, 0, 0));
			renderables_[i].Color = Lerp(particles_[i].color, particles_[i].colorEnd, time);
			renderables_[i].Size = Lerp(particles_[i].size, particles_[i].sizeEnd, time);
		}

		// Depth sort for transparency
		if (sort) 
		{
			DirectX::XMMATRIX viewProj;
			viewProj = DirectX::XMMatrixMultiply(camera->GetView(), camera->GetProjection());
			Texture* tex = texture_;
			std::sort(renderables_, renderables_ + particles_.size(), [viewProj, tex](const ParticleRenderable& a, const ParticleRenderable& b) {
				// Ignore solids without mask
				if (tex==nullptr && (a.Color.w == 1 || b.Color.w==1)) { 
					return a.Color.w > b.Color.w;
				}

				DirectX::XMVECTOR ac, bc;
				ac = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&a.Position), viewProj);
				bc = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&b.Position), viewProj);
				float z1, z2;
				z1 = ac.m128_f32[2] / ac.m128_f32[3];
				z2 = bc.m128_f32[2] / bc.m128_f32[3];
				return z2 < z1;
			});
		}
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleSystem::CreateBuffer()
	{
		if (renderables_ != nullptr) {
			free(renderables_);
			particle_buffer_.Destroy();
		}

		buffer_size_ = std::max(1.0f, std::ceil(particles_.size() / (float)particle_barrier_)) * particle_barrier_;

		renderables_ = (ParticleRenderable*)malloc(buffer_size_ * sizeof(ParticleRenderable));
		particle_buffer_.Create(L"ParticleBuffer", buffer_size_, sizeof(ParticleRenderable));
	}

	//------------------------------------------------------------------------------------------------------
	ParticleEmitter::ParticleEmitter()
	{
		system_ = nullptr;
		mode_ = Passive;
		time_ = 0;
		amount_ = 1;
		spawn_size_ = { 1, 1, 1 };

		type_.velocity = {};
		type_.velocity_random = {};

		type_.lifetime=1;
		type_.lifetime_random=0;

		type_.color = {1, 1, 1, 1};
		type_.colorEnd = { 1, 1, 1, 1 };
		type_.color_random = { 0, 0, 0, 0 };
		type_.colorEnd_random = { 0, 0, 0, 0 };

		type_.size = 1;
		type_.sizeEnd = 1;
		type_.size_random = 0;
		type_.sizeEnd_random = 0;
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleEmitter::AssignToSystem(ParticleSystem* system)
	{
		system_ = system;
		system_->GetEmitters().push_back(this);
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleEmitter::Update()
	{
		// Check if assigned
		assert(system_ != nullptr);

		time_ += Get::DeltaT();

		switch (mode_)
		{
		case tremble::Constant:
		{
			float timePerParticle = (1000.0f / amount_) / 1000.0f;
			if (time_ >= timePerParticle) {
				int toSpawn = std::floor(time_ / timePerParticle);
				Burst(toSpawn);
				time_ -= toSpawn * timePerParticle;
			}
			break;
		}

		case tremble::Fill:
		{
			if (system_->GetParticleCount() < amount_) {
				Burst(amount_ - system_->GetParticleCount());
			}
			break;
		}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleEmitter::Burst(int amount)
	{
		// Check if assigned
		assert(system_ != nullptr);

		Vector3 pos = GetNode()->GetPosition() - spawn_size_ / 2.0f;

		ParticleDescription p;

		for (int i = 0; i < amount; i++) {
			p.position = pos + Vector3(((rand() % 1000) / 1000.0f)*spawn_size_.GetX(), ((rand() % 1000) / 1000.0f)*spawn_size_.GetY(), ((rand() % 1000) / 1000.0f)*spawn_size_.GetZ());
			p.color = type_.color + Vector4(((rand() % 1000) / 1000.0f)*type_.color_random.GetX(), ((rand() % 1000) / 1000.0f)*type_.color_random.GetY(), ((rand() % 1000) / 1000.0f)*type_.color_random.GetZ(), ((rand() % 1000) / 1000.0f)*type_.color_random.GetW());
			p.colorEnd = type_.colorEnd + Vector4(((rand() % 1000) / 1000.0f)*type_.colorEnd_random.GetX(), ((rand() % 1000) / 1000.0f)*type_.colorEnd_random.GetY(), ((rand() % 1000) / 1000.0f)*type_.colorEnd_random.GetZ(), ((rand() % 1000) / 1000.0f)*type_.colorEnd_random.GetW());
			p.velocity = type_.velocity + Vector3(((rand() % 1000) / 1000.0f)*type_.velocity_random.GetX(), ((rand() % 1000) / 1000.0f)*type_.velocity_random.GetY(), ((rand() % 1000) / 1000.0f)*type_.velocity_random.GetZ());
			p.lifetime = type_.lifetime + ((rand() % 1000) / 1000.0f)*type_.lifetime_random;
			p.size = type_.size + ((rand() % 1000) / 1000.0f)*type_.size;
			p.sizeEnd = type_.sizeEnd + ((rand() % 1000) / 1000.0f)*type_.sizeEnd;
			system_->Spawn(p);
		}
	}
}