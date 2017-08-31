#pragma once

namespace tremble
{
	class Shader
	{
	public:
		enum ShaderType
		{
			ShaderTypeVertex,
			ShaderTypePixel,
			ShaderTypeGeometry,
			ShaderTypeUnknown
		};

		Shader();
		~Shader();

		void CompileFromFile(const std::string& shader_path, ShaderType shader_type);
		void CompileFromSource(const std::string& shader_code, ShaderType shader_type);
		void CreateFromByteCode(const BYTE byte_code[], UINT byte_code_size);

		void Reload();
		
		const ShaderType& GetShaderType() const { return shader_type_; }
		const D3D12_SHADER_BYTECODE& GetShaderByteCode() const { return shader_byte_code_; }
		const void* GetShaderBinary() const { return shader_byte_code_.pShaderBytecode; }
		const UINT& GetShaderBinaryLength() const { return static_cast<UINT>(shader_byte_code_.BytecodeLength); }
		const ID3DBlob* GetShaderBlob() const { return shader_blob_; }
		const ID3DBlob* GetErrorBlob() const { return error_blob_; }
		
		static int shaders_created;
		static int shaders_deleted;

	private:
		std::string shader_path_;
		std::string shader_code_;

		ShaderType shader_type_;
		D3D12_SHADER_BYTECODE shader_byte_code_;

		ID3DBlob* shader_blob_;
		ID3DBlob* error_blob_;
	};
}