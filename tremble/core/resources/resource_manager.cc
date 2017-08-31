#include "resource_manager.h"

#include "../game_manager.h"
#include "../../core/resources/mesh.h"
#include "../../core/resources/model_loader.h"
#include "../../core/rendering/texture.h"
#include "../../core/rendering/material.h"
#include "../../core/rendering/shader.h"
#include "../../core/memory/memory_includes.h"
#include "../../core/audio/audio_clip.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	ResourceManager::ResourceManager()
	{
		shader_directory_ = "../../shaders/";
		texture_directory_ = "../../textures/";
		model_directory_ = "../../models/";
		audio_clip_directory_ = "../../audio/";
		

		texture_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Texture), TREMBLE_RESOURCES_NUM_MAX_TEXTURES));
		model_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Model), TREMBLE_RESOURCES_NUM_MAX_MODELS));
		shader_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Shader), TREMBLE_RESOURCES_NUM_MAX_SHADERS));
		audio_clip_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(AudioClip), TREMBLE_RESOURCES_NUM_MAX_AUDIO_CLIPS));
	}

	//------------------------------------------------------------------------------------------------------
	ResourceManager::~ResourceManager()
	{
		for (auto texture = textures_.begin(); texture != textures_.end(); texture++)
		{
			texture_allocator_->Delete(texture->second);
		}

		for (auto model = models_.begin(); model != models_.end(); model++)
		{
			model_allocator_->Delete(model->second);
		}

		for (auto shader = shaders_.begin(); shader != shaders_.end(); shader++)
		{
			shader_allocator_->Delete(shader->second);
		}

		for (auto audio_clip = audio_clips_.begin(); audio_clip != audio_clips_.end(); audio_clip++)
		{
			audio_clip_allocator_->Delete(audio_clip->second);
		}

		Get::MemoryManager()->DeleteAllocator(texture_allocator_);
		Get::MemoryManager()->DeleteAllocator(model_allocator_);
		Get::MemoryManager()->DeleteAllocator(shader_allocator_);
		Get::MemoryManager()->DeleteAllocator(audio_clip_allocator_);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//------------------------------------------ TEXTURE LOADING ---------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ResourceManager::SetTextureDirectory(const std::string& texture_directory_location)
	{
		texture_directory_ = texture_directory_location;
	}

	//------------------------------------------------------------------------------------------------------
	const std::string& ResourceManager::GetTextureDirectory()
	{
		return texture_directory_;
	}
	
	//------------------------------------------------------------------------------------------------------
	Texture* ResourceManager::LoadTexture(const std::string& texture_location, bool use_texture_directory)
	{
		std::string location = use_texture_directory ? texture_directory_ + texture_location : texture_location;

		auto result = textures_.find(location);
		if (result != textures_.end() && result->second != nullptr)
		{
			texture_allocator_->Delete<Texture>(result->second);
		}

		// cache the loaded texture in a local variable to avoid an unnecessary & costly unordered_map find-operation
		Texture* loaded_texture = texture_allocator_->New<Texture>(location);
		textures_[location] = loaded_texture;
		return loaded_texture;
	}

	//------------------------------------------------------------------------------------------------------
	void ResourceManager::UnloadTexture(const std::string& texture_location, bool use_texture_directory)
	{
		std::string location = use_texture_directory ? texture_directory_ + texture_location : texture_location;

		auto result = textures_.find(location);
		if (result != textures_.end())
		{
			texture_allocator_->Delete<Texture>(result->second);
			textures_[location] = nullptr;
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	Texture* ResourceManager::GetTexture(const std::string& texture_location, bool use_texture_directory)
	{
		std::string location = use_texture_directory ? texture_directory_ + texture_location : texture_location;

		auto result = textures_.find(location);
		if (result != textures_.end() && result->second != nullptr)
		{
			return result->second;
		}
		
		return LoadTexture(texture_location, use_texture_directory);
	}
	
	//------------------------------------------------------------------------------------------------------
	bool ResourceManager::IsTextureLoaded(const std::string& texture_location, bool use_texture_directory)
	{
		std::string location = use_texture_directory ? texture_directory_ + texture_location : texture_location;

		auto result = textures_.find(location);
		return result != textures_.end() && result->second != nullptr;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------- MODEL LOADING ---------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ResourceManager::SetModelDirectory(const std::string& model_directory_location)
	{
		model_directory_ = model_directory_location;
	}

	//------------------------------------------------------------------------------------------------------
	const std::string& ResourceManager::GetModelDirectory()
	{
		return model_directory_;
	}

	//------------------------------------------------------------------------------------------------------
	Model* ResourceManager::LoadModel(const std::string& model_location, bool use_model_directory)
	{
		std::string location = use_model_directory ? model_directory_ + model_location : model_location;

		auto result = models_.find(location);
		if (result != models_.end() && result->second != nullptr)
		{
			model_allocator_->Delete<Model>(result->second);
		}

		// cache the loaded model in a local variable to avoid an unnecessary & costly unordered_map find-operation
		Model* loaded_model = ModelLoader::LoadModel(location, model_allocator_);
		models_[location] = loaded_model;
		return loaded_model;
	}

	//------------------------------------------------------------------------------------------------------
	void ResourceManager::UnloadModel(const std::string& model_location, bool use_model_directory)
	{
		std::string location = use_model_directory ? model_directory_ + model_location : model_location;

		auto result = models_.find(location);
		if (result != models_.end())
		{
			model_allocator_->Delete<Model>(result->second);
			models_[location] = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	Model* ResourceManager::GetModel(const std::string& model_location, bool use_model_directory)
	{
		std::string location = use_model_directory ? model_directory_ + model_location : model_location;

		auto result = models_.find(location);
		if (result != models_.end() && result->second != nullptr)
		{
			return result->second;
		}

		return LoadModel(model_location, use_model_directory);
	}

	//------------------------------------------------------------------------------------------------------
	bool ResourceManager::IsModelLoaded(const std::string& model_location, bool use_model_directory)
	{
		std::string location = use_model_directory ? model_directory_ + model_location : model_location;

		auto result = models_.find(location);
		return result != models_.end() && result->second != nullptr;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------- SHADER LOADING ---------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ResourceManager::SetShaderDirectory(const std::string& shader_directory_location)
	{
		shader_directory_ = shader_directory_location;
	}

	//------------------------------------------------------------------------------------------------------
	const std::string& ResourceManager::GetShaderDirectory()
	{
		return shader_directory_;
	}

	//------------------------------------------------------------------------------------------------------
	Shader* ResourceManager::LoadShader(const std::string& shader_location, bool use_shader_directory)
	{
		std::string location = use_shader_directory ? shader_directory_ + shader_location : shader_location;

		auto result = shaders_.find(location);
		if (result != shaders_.end() && result->second != nullptr)
		{
			shader_allocator_->Delete<Shader>(result->second);
		}

		std::ifstream input(location, std::ios::in | std::ios::ate | std::ios::binary);

		char* shader_byte_code = nullptr;
		std::streampos shader_byte_size;

		if (input.is_open())
		{
			shader_byte_size = input.tellg();
			shader_byte_code = new char[shader_byte_size];
			input.seekg(0, std::ios::beg);
			input.read(shader_byte_code, shader_byte_size);
			input.close();
		}
		else
		{
			std::cout << "Failed to load a shader (" << location << "). Exiting." << std::endl;
			system("PAUSE");
			exit(-1);
		}

		// cache the loaded shader in a local variable to avoid an unnecessary & costly unordered_map find-operation
		Shader* loaded_shader = shader_allocator_->New<Shader>();
		loaded_shader->CreateFromByteCode(reinterpret_cast<BYTE*>(shader_byte_code), shader_byte_size);
		shaders_[location] = loaded_shader;
		return loaded_shader;
	}

	//------------------------------------------------------------------------------------------------------
	void ResourceManager::UnloadShader(const std::string& shader_location, bool use_shader_directory)
	{
		std::string location = use_shader_directory ? shader_directory_ + shader_location : shader_location;

		auto result = shaders_.find(location);
		if (result != shaders_.end())
		{
			shader_allocator_->Delete<Shader>(result->second);
			shaders_[location] = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	Shader* ResourceManager::GetShader(const std::string& shader_location, bool use_shader_directory)
	{
		std::string location = use_shader_directory ? shader_directory_ + shader_location : shader_location;

		auto result = shaders_.find(location);
		if (result != shaders_.end() && result->second != nullptr)
		{
			return result->second;
		}

		return LoadShader(shader_location, use_shader_directory);
	}

	//------------------------------------------------------------------------------------------------------
	bool ResourceManager::IsShaderLoaded(const std::string& shader_location, bool use_shader_directory)
	{
		std::string location = use_shader_directory ? shader_directory_ + shader_location : shader_location;

		auto result = shaders_.find(location);
		return result != shaders_.end() && result->second != nullptr;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------- AudioClip LOADING ---------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ResourceManager::SetAudioClipDirectory(const std::string& audio_clip_directory_location)
	{
		audio_clip_directory_ = audio_clip_directory_location;
	}

	//------------------------------------------------------------------------------------------------------
	const std::string& ResourceManager::GetAudioClipDirectory()
	{
		return audio_clip_directory_;
	}

	//------------------------------------------------------------------------------------------------------
	AudioClip* ResourceManager::LoadAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory)
	{
		std::string location = use_audio_clip_directory ? audio_clip_directory_ + audio_clip_location : audio_clip_location;

		auto result = audio_clips_.find(location);
		if (result != audio_clips_.end() && result->second != nullptr)
		{
			audio_clip_allocator_->Delete<AudioClip>(result->second);
		}

		// cache the loaded audio_clip in a local variable to avoid an unnecessary & costly unordered_map find-operation
		AudioClip* loaded_audio_clip = audio_clip_allocator_->New<AudioClip>(location);
		audio_clips_[location] = loaded_audio_clip;
		return loaded_audio_clip;
	}

	//------------------------------------------------------------------------------------------------------
	void ResourceManager::UnloadAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory)
	{
		std::string location = use_audio_clip_directory ? audio_clip_directory_ + audio_clip_location : audio_clip_location;

		auto result = audio_clips_.find(location);
		if (result != audio_clips_.end())
		{
			audio_clip_allocator_->Delete<AudioClip>(result->second);
			audio_clips_[location] = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	AudioClip* ResourceManager::GetAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory)
	{
		std::string location = use_audio_clip_directory ? audio_clip_directory_ + audio_clip_location : audio_clip_location;

		auto result = audio_clips_.find(location);
		if (result != audio_clips_.end() && result->second != nullptr)
		{
			return result->second;
		}

		return LoadAudioClip(audio_clip_location, use_audio_clip_directory);
	}

	//------------------------------------------------------------------------------------------------------
	bool ResourceManager::IsAudioClipLoaded(const std::string& audio_clip_location, bool use_audio_clip_directory)
	{
		std::string location = use_audio_clip_directory ? audio_clip_directory_ + audio_clip_location : audio_clip_location;

		auto result = audio_clips_.find(location);
		return result != audio_clips_.end() && result->second != nullptr;
	}
}