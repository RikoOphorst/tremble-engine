#include "shader.h"

#include "../utilities/debug.h"

namespace tremble
{
	int Shader::shaders_created = 0;
	int Shader::shaders_deleted = 0;

	//------------------------------------------------------------------------------------------------------
	Shader::Shader() :
		shader_path_(""),
		shader_code_(""),
		shader_type_(ShaderTypeUnknown),
		shader_blob_(nullptr),
		error_blob_(nullptr)
	{
		ZeroMemory(&shader_byte_code_, sizeof(D3D12_SHADER_BYTECODE));
	}
	
	//------------------------------------------------------------------------------------------------------
	Shader::~Shader()
	{
		SAFE_RELEASE(shader_blob_);
		SAFE_RELEASE(error_blob_);

		shaders_deleted++;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Shader::CompileFromFile(const std::string& shader_path, ShaderType shader_type)
	{
		std::string entry_point;
		std::string shader_model;
		switch (shader_type)
		{
		case ShaderTypeVertex:
			entry_point = "VS";
			shader_model = "vs_5_0";
			break;
		case ShaderTypeGeometry:
			entry_point = "GS";
			shader_model = "gs_5_0";
			break;
		case ShaderTypePixel:
			entry_point = "PS";
			shader_model = "ps_5_0";
			break;
		default:
			DLOG("Invalid shader type given, not compiling..");
			return;
			break;
		}

		shader_code_ = "";

		shader_path_ = shader_path;
		shader_type_ = shader_type;

		std::wstring shader_path_intermediate = std::wstring(shader_path_.begin(), shader_path_.end());

		UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* shader_blob_intermediate = nullptr;
		ID3DBlob* error_blob_intermediate = nullptr;
		HRESULT hr = D3DCompileFromFile(shader_path_intermediate.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point.c_str(), shader_model.c_str(), flags, NULL, &shader_blob_intermediate, &error_blob_intermediate);

		if (hr != S_OK || error_blob_intermediate != nullptr)
		{
			DLOG("Shader compile failed: ");
			DLOG(static_cast<const char*>(error_blob_intermediate->GetBufferPointer()));

			return;
		}
		else
		{
			SAFE_RELEASE(shader_blob_);
			SAFE_RELEASE(error_blob_);

			shader_blob_ = shader_blob_intermediate;
			error_blob_ = error_blob_intermediate;

			shader_byte_code_.BytecodeLength = shader_blob_->GetBufferSize();
			shader_byte_code_.pShaderBytecode = shader_blob_->GetBufferPointer();

			shaders_created++;
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	void Shader::CompileFromSource(const std::string& shader_code, ShaderType shader_type)
	{
		std::string entry_point;
		std::string shader_model;
		switch (shader_type_)
		{
		case ShaderTypeVertex:
			entry_point = "VS";
			shader_model = "vs_5_0";
			break;
		case ShaderTypeGeometry:
			entry_point = "GS";
			shader_model = "gs_5_0";
			break;
		case ShaderTypePixel:
			entry_point = "PS";
			shader_model = "ps_5_0";
			break;
		default:
			DLOG("Invalid shader type given, not compiling..");
			return;
			break;
		}

		shader_path_ = "";

		shader_code_ = shader_code;
		shader_type_ = shader_type;

		std::wstring shader_code_intermediate = std::wstring(shader_code_.begin(), shader_code_.end());

		UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined(_DEBUG) || defined(DEBUG)
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* shader_blob_intermediate = nullptr;
		ID3DBlob* error_blob_intermediate = nullptr;
		HRESULT hr = D3DCompile(shader_code_intermediate.c_str(), shader_code_intermediate.size(), NULL, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point.c_str(), shader_model.c_str(), flags, NULL, &shader_blob_intermediate, &error_blob_intermediate);

		if (hr != S_OK || error_blob_intermediate != nullptr)
		{
			DLOG("Shader compile failed:");
			DLOG(static_cast<const char*>(error_blob_->GetBufferPointer()));

			return;
		}
		else
		{
			SAFE_RELEASE(shader_blob_);
			SAFE_RELEASE(error_blob_);

			shader_blob_ = shader_blob_intermediate;
			error_blob_ = error_blob_intermediate;

			shader_byte_code_.BytecodeLength = shader_blob_->GetBufferSize();
			shader_byte_code_.pShaderBytecode = shader_blob_->GetBufferPointer();

			shaders_created++;
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Shader::CreateFromByteCode(const BYTE byte_code[], UINT byte_code_size)
	{
		shader_byte_code_.pShaderBytecode = byte_code;
		shader_byte_code_.BytecodeLength = byte_code_size;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Shader::Reload()
	{
		if (shader_path_ != "")
		{
			CompileFromFile(shader_path_, shader_type_);
		}
		
		if (shader_code_ != "")
		{
			CompileFromSource(shader_code_, shader_type_);
		}
	}
}