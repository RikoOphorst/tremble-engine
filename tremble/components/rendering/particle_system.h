#pragma once
#include "../../core/scene_graph/component.h"
#include "../../core/math/math.h"
#include "../../core/rendering/structured_buffer.h"
#include "../../core/rendering/particle_renderer.h"

namespace tremble 
{

	class Texture;
	class ParticleEmitter;

	enum ParticleSpace{
		World,
		System
	};

	struct Particle {
		Vector3 position;
		Vector3 velocity;

		float age;
		float lifetime;

		Vector4 color;
		Vector4 colorEnd;

		float size;
		float sizeEnd;
	};

	struct ParticleDescription {
		Vector3 position;
		Vector3 velocity;
		float lifetime;

		Vector4 color;
		Vector4 colorEnd;

		float size;
		float sizeEnd;
	};

	/**
	* @class tremble::ParticleSystem
	* @author Sander Brouwers
	* @brief Handels the lifecycle of particles
	*/
	class ParticleSystem : public Component
	{
	public:
		ParticleSystem();

		/// Assigns texture to particle system
		void SetTexture(Texture* texture) { texture_ = texture; }
		/// Returns texture assigned to particle system
		Texture* GetTexture() { return texture_; }

		/// Returns GPU buffer for renderable particles
		StructuredBuffer& GetBuffer() { return particle_buffer_; }

		/// Returns CPU buffer with renderable particles
		ParticleRenderable* GetParticles() { return renderables_; }

		/// Returns amount of particles in system
		int GetParticleCount() { return (int)particles_.size(); }

		/// Sets alignment space
		void SetSpace(ParticleSpace space) { space_ = space; }

		/// Spawn particle with specified description
		void Spawn(ParticleDescription&);

		/// Update particle system
		void Update(Camera*);

		/// Returns emitters attached to system
		std::vector<ParticleEmitter*>& GetEmitters() { return emitters_; }

	private:
		void CreateBuffer();

		Texture* texture_;

		StructuredBuffer particle_buffer_;

		std::vector<Particle> particles_;

		ParticleRenderable* renderables_;

		int buffer_size_;

		int particle_barrier_;

		ParticleSpace space_ = System;

		std::vector<ParticleEmitter*> emitters_;
	};

	enum EmitterMode {
		Constant,
		Fill,
		Passive
	};

	struct ParticleType {
		Vector3 velocity;
		Vector3 velocity_random;

		float lifetime;
		float lifetime_random;

		Vector4 color;
		Vector4 colorEnd;
		Vector4 color_random;
		Vector4 colorEnd_random;

		float size;
		float sizeEnd;
		float size_random;
		float sizeEnd_random;
	};

	/**
	* @class tremble::ParticleEmitter
	* @author Sander Brouwers
	* @brief Spawns particles in defined area
	*/
	class ParticleEmitter : public Component {
	public:

		ParticleEmitter();

		/// Updates emitter ands spawns particles if needed
		void Update();

		/// Assign emitter to specified system
		void AssignToSystem(ParticleSystem*);

		/// Sets spawning mode
		void SetMode(EmitterMode m) { mode_ = m; time_ = 0; }

		/// Set spaning rate (particles / second) for constant mode
		void SetRate(float rate) { amount_ = rate; time_ = 0; }

		/// Sets particle amount target for fill mode
		void SetTarget(float target) { amount_ = target; }

		/// Sets Spawning area
		void SetSpawnArea(Vector3 size) { spawn_size_ = size; }

		/// Sets particle type used for spawning
		void SetParticle(ParticleType type) { type_ = type; }

		/// Spawns specified amount of particles
		void Burst(int);

	private:
		EmitterMode mode_;

		ParticleSystem* system_;

		float amount_;
		float time_;
		Vector3 spawn_size_;

		ParticleType type_;
	};
}