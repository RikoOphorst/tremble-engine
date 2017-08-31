#pragma once

#define TREMBLE_RESOURCES_NUM_MAX_TEXTURES 1024
#define TREMBLE_RESOURCES_NUM_MAX_MODELS 1024
#define TREMBLE_RESOURCES_NUM_MAX_SHADERS 1024
#define TREMBLE_RESOURCES_NUM_MAX_AUDIO_CLIPS 1024

namespace tremble
{
	class FreeListAllocator;
	struct Material;
	class Model;
	class Texture;
	class Shader;
	class AudioClip;

	class ResourceManager
	{
	public:
		ResourceManager(); //!< Default constructor
		~ResourceManager(); //!< Default destructor

	public:
		//------------------------------------------------------------------------------------------------------
		//------------------------------------------ TEXTURE LOADING -------------------------------------------
		//------------------------------------------------------------------------------------------------------

		/**
		* @brief Sets the texture directory. This is prefixed to all texture file locations.
		* @param[in] texture_directory_location The location of the texture directory
		*/
		void SetTextureDirectory(const std::string& texture_directory_location);

		/**
		* @brief Query for the current texture directory
		*/
		const std::string& GetTextureDirectory();

		/**
		* @brief Force (re-)loads a texture into the resource manager.
		* @param[in] texture_location The location of the texture file on disk
		* @param[in] use_texture_directory Whether the texture directory should be prefixed to the texture location
		*/
		Texture* LoadTexture(const std::string& texture_location, bool use_texture_directory = true);

		/**
		* @brief Force unloads a texture from the resource manager. NOTE: make sure there are no other systems using the texture's memory!
		* @param[in] texture_location The location of the texture file on disk
		* @param[in] use_texture_directory Whether the texture directory should be prefixed to the texture location
		*/
		void UnloadTexture(const std::string& texture_location, bool use_texture_directory = true);

		/**
		* @brief Queries the internal texture-map for the given texture. If it hasn't been loaded yet, the resource manager will attempt to load the texture.
		* @param[in] texture_location The location of the texture file on disk
		* @param[in] use_texture_directory Whether the texture directory should be prefixed to the texture location
		*/
		Texture* GetTexture(const std::string& texture_location, bool use_texture_directory = true);

		/**
		* @brief Use this to check whether a certain texture has already been loaded into memory
		* @param[in] texture_location The location of the texture file on disk
		* @param[in] use_texture_directory Whether the texture directory should be prefixed to the texture location
		*/
		bool IsTextureLoaded(const std::string& texture_location, bool use_texture_directory = true);

	public:
		//------------------------------------------------------------------------------------------------------
		//-------------------------------------------- MODEL LOADING -------------------------------------------
		//------------------------------------------------------------------------------------------------------

		/**
		* @brief Sets the model directory. This is prefixed to all model file locations.
		* @param[in] model_directory_location The location of the model directory
		*/
		void SetModelDirectory(const std::string& model_directory_location);

		/**
		* @brief Query for the current model directory
		*/
		const std::string& GetModelDirectory();

		/**
		* @brief Force (re-)loads a model into the resource manager.
		* @param[in] model_location The location of the model file on disk
		* @param[in] use_model_directory Whether the model directory should be prefixed to the model location
		*/
		Model* LoadModel(const std::string& model_location, bool use_model_directory = true);

		/**
		* @brief Force unloads a model from the resource manager. NOTE: make sure there are no other systems using the model's memory!
		* @param[in] model_location The location of the model file on disk
		* @param[in] use_model_directory Whether the model directory should be prefixed to the model location
		*/
		void UnloadModel(const std::string& model_location, bool use_model_directory = true);

		/**
		* @brief Queries the internal model-map for the given model. If it hasn't been loaded yet, the resource manager will attempt to load the model.
		* @param[in] model_location The location of the model file on disk
		* @param[in] use_model_directory Whether the model directory should be prefixed to the model location
		*/
		Model* GetModel(const std::string& model_location, bool use_model_directory = true);

		/**
		* @brief Use this to check whether a certain model has already been loaded into memory
		* @param[in] model_location The location of the model file on disk
		* @param[in] use_model_directory Whether the model directory should be prefixed to the model location
		*/
		bool IsModelLoaded(const std::string& model_location, bool use_model_directory = true);

	public:
		//------------------------------------------------------------------------------------------------------
		//-------------------------------------------- SHADER LOADING ------------------------------------------
		//------------------------------------------------------------------------------------------------------

		/**
		* @brief Sets the shader directory. This is prefixed to all shader file locations.
		* @param[in] shader_directory_location The location of the shader directory
		*/
		void SetShaderDirectory(const std::string& shader_directory_location);

		/**
		* @brief Query for the current shader directory
		*/
		const std::string& GetShaderDirectory();

		/**
		* @brief Force (re-)loads a shader into the resource manager.
		* @param[in] shader_location The location of the shader file on disk
		* @param[in] use_shader_directory Whether the shader directory should be prefixed to the shader location
		*/
		Shader* LoadShader(const std::string& shader_location, bool use_shader_directory = true);

		/**
		* @brief Force unloads a shader from the resource manager. NOTE: make sure there are no other systems using the shader's memory!
		* @param[in] shader_location The location of the shader file on disk
		* @param[in] use_shader_directory Whether the shader directory should be prefixed to the shader location
		*/
		void UnloadShader(const std::string& shader_location, bool use_shader_directory = true);

		/**
		* @brief Queries the internal shader-map for the given shader. If it hasn't been loaded yet, the resource manager will attempt to load the shader.
		* @param[in] shader_location The location of the shader file on disk
		* @param[in] use_shader_directory Whether the shader directory should be prefixed to the shader location
		*/
		Shader* GetShader(const std::string& shader_location, bool use_shader_directory = true);

		/**
		* @brief Use this to check whether a certain shader has already been loaded into memory
		* @param[in] shader_location The location of the shader file on disk
		* @param[in] use_shader_directory Whether the shader directory should be prefixed to the shader location
		*/
		bool IsShaderLoaded(const std::string& shader_location, bool use_shader_directory = true);

	public:
		//------------------------------------------------------------------------------------------------------
		//-------------------------------------------- AUDIO CLIP LOADING --------------------------------------
		//------------------------------------------------------------------------------------------------------

		/**
		* @brief Sets the audio_clip directory. This is prefixed to all audio_clip file locations.
		* @param[in] audio_clip_directory_location The location of the audio_clip directory
		*/
		void SetAudioClipDirectory(const std::string& audio_clip_directory_location);

		/**
		* @brief Query for the current audio_clip directory
		*/
		const std::string& GetAudioClipDirectory();

		/**
		* @brief Force (re-)loads a audio_clip into the resource manager.
		* @param[in] audio_clip_location The location of the audio_clip file on disk
		* @param[in] use_audio_clip_directory Whether the audio_clip directory should be prefixed to the audio_clip location
		*/
		AudioClip* LoadAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory = true);

		/**
		* @brief Force unloads a audio_clip from the resource manager. NOTE: make sure there are no other systems using the audio_clip's memory!
		* @param[in] audio_clip_location The location of the audio_clip file on disk
		* @param[in] use_audio_clip_directory Whether the audio_clip directory should be prefixed to the audio_clip location
		*/
		void UnloadAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory = true);

		/**
		* @brief Queries the internal audio_clip-map for the given audio_clip. If it hasn't been loaded yet, the resource manager will attempt to load the audio_clip.
		* @param[in] audio_clip_location The location of the audio_clip file on disk
		* @param[in] use_audio_clip_directory Whether the audio_clip directory should be prefixed to the audio_clip location
		*/
		AudioClip* GetAudioClip(const std::string& audio_clip_location, bool use_audio_clip_directory = true);

		/**
		* @brief Use this to check whether a certain audio_clip has already been loaded into memory
		* @param[in] audio_clip_location The location of the audio_clip file on disk
		* @param[in] use_audio_clip_directory Whether the audio_clip directory should be prefixed to the audio_clip location
		*/
		bool IsAudioClipLoaded(const std::string& audio_clip_location, bool use_audio_clip_directory = true);

	private:
		std::string texture_directory_; //!< The directory in which all texture files are located
		FreeListAllocator* texture_allocator_; //!< Allocator used for texture allocation
		std::unordered_map<std::string, Texture*> textures_; //!< Stores all texture pointers based on their string-identifier

		std::string model_directory_; //!< The directory in which all model files are located
		FreeListAllocator* model_allocator_; //!< Allocator used for model allocation
		std::unordered_map<std::string, Model*> models_; //!< Stores all model pointers based on their string-identifier

		std::string shader_directory_; //!< The directory in which all compiled shader files are located
		FreeListAllocator* shader_allocator_; //!< Allocator used for shader allocation
		std::unordered_map<std::string, Shader*> shaders_; //!< Stores all shader pointers based on their string-identifier

		std::string audio_clip_directory_; //!< The directory in which all Audio clips are located
		FreeListAllocator* audio_clip_allocator_; //!< Allocator used for audio clip allocation
		std::unordered_map<std::string, AudioClip*> audio_clips_; //!< Stores all audio clip pointers based on their string-identifier
	};
}