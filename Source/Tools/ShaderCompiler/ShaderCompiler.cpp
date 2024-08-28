// Copyright 2020, Nathan Blane

//WALL_WRN_PUSH
#include <sstream>
//WALL_WRN_POP

#include "BasicTypes/Uncopyable.hpp"
#include "ShaderCompiler.h"
#include "ShaderCompiledOutput.hpp"
#include "ShaderExternal.h"
#include "McppWrapper.hpp"
#include "spirv/disassemble.h"

#include "File/DirectoryLocations.hpp"
#include "Debugging/Assertion.hpp"
#include "Path/Path.hpp"
#include "Utilities/HashFuncs.hpp"
#include "Serialization/MemorySerializer.hpp"

#include "Graphics/Vulkan/VulkanShaderHeader.hpp"
#include "Graphics/Vulkan/VulkanUtilities.h"

namespace
{

class SpvReadBuf : public std::stringbuf, private Uncopyable
{
public:
	SpvReadBuf() = default;

	virtual i32 sync() override
	{
		char* p = pbase();
		char* end = pptr();
		u32 size = (u32)(end - p);
		spvBuffer.Clear();
		spvBuffer.Reserve(size);
		for (; p != end; ++p)
		{
			spvBuffer.Add(*p);
		}

		return 0;
	}

	inline DynamicArray<ansichar> GetSPVText()
	{
		return spvBuffer;
	}

private:
	DynamicArray<ansichar> spvBuffer;
};

// TODO - Make this in accordance with the Vulkan Device properties
static const TBuiltInResource DefaultTBuiltInResource =
{
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .limits = */{
	/* .nonInductiveForLoops = */ 1,
	/* .whileLoops = */ 1,
	/* .doWhileLoops = */ 1,
	/* .generalUniformIndexing = */ 1,
	/* .generalAttributeMatrixVectorIndexing = */ 1,
	/* .generalVaryingIndexing = */ 1,
	/* .generalSamplerIndexing = */ 1,
	/* .generalVariableIndexing = */ 1,
	/* .generalConstantMatrixVectorIndexing = */ 1,
} };

static EShLanguage GetGlslangStage(ShaderStage::Type stage)
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return EShLangVertex;
		case ShaderStage::Fragment:
			return EShLangFragment;
		case ShaderStage::Geometry:
			return EShLangGeometry;
		case ShaderStage::TessalationEval:
			return EShLangTessEvaluation;
		case ShaderStage::TessalationControl:
			return EShLangTessControl;
		case ShaderStage::Compute:
			return EShLangCompute;
		default:
		{
			Assert(false);
			return static_cast<EShLanguage>(-1);
		}
	}
}

#pragma warning(push)
#pragma warning(disable:4062)
static ShaderIntrinsics GetShaderIntrinsic(SpvOp operation)
{
	switch (operation)
	{
		case SpvOpTypeBool:
			return ShaderIntrinsics::Boolean;
		case SpvOpTypeInt:
			return ShaderIntrinsics::Integer;
		case SpvOpTypeFloat:
			return ShaderIntrinsics::Float;
		case SpvOpTypeVector:
			return ShaderIntrinsics::Vector;
		case SpvOpTypeMatrix:
			return ShaderIntrinsics::Matrix;
		case SpvOpTypeStruct:
			return ShaderIntrinsics::Struct;
	}

	Assert(false);
	return static_cast<ShaderIntrinsics>(-1);
}

static ShaderResourceType GetShaderConstantType(SpvReflectDescriptorType type)
{
	switch (type)
	{
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return ShaderResourceType::TextureSampler;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return ShaderResourceType::UniformBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return ShaderResourceType::StorageBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return ShaderResourceType::UniformDynamicBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return ShaderResourceType::StorageDynamicBuffer;
	}

	return static_cast<ShaderResourceType>(-1);
}
#pragma warning(pop)

static bool glslangInitialized = false;

static void RemoveLinePreprocessorDirectives(String& preprocessedShader)
{
	do
	{
		i32 index = preprocessedShader.FindFirst("#line");
		if (index >= 0)
		{
			u32 usIndex = (u32)index;
			i32 newLineIndex = preprocessedShader.FindFrom(usIndex, "\n");
			u32 removeCount = (u32)(newLineIndex - index + 1); // Remove the newline as well
			preprocessedShader.Remove(usIndex, removeCount);
		}

	} while (preprocessedShader.Contains("#line"));
}

}

void InitializeCompiler()
{
	glslang::InitializeProcess();
	glslangInitialized = true;
}

void DeinitializeCompiler()
{
	Assert(glslangInitialized);
	glslang::FinalizeProcess();
}

bool Preprocess(const tchar* pathToShader, const ShaderCompilerDefinitions& inputs, PreprocessedShaderOutput& preprocessedOutput)
{
	ShaderPreprocessor preprocessor(inputs.shaderStage, inputs.definitions);
	preprocessor.Preprocess(Path(pathToShader));

	String extensions = "#extension GL_ARB_separate_shader_objects : enable\n#extension GL_ARB_shading_language_420pack : enable\n\n";
	String shaderHeader = "#version 450 core\n";
	shaderHeader += extensions;

	String output = preprocessor.PreprocessedOutput();
	RemoveLinePreprocessorDirectives(output);

	preprocessedOutput.outputGlsl = shaderHeader + output;
	preprocessedOutput.errors = preprocessor.ErrorString();

	return preprocessor.Success();

}

bool Compile(const tchar* pathToFile, const char* entryPoint, const ShaderCompilerDefinitions& inputs, ShaderStructure& output)
{
	Assert(glslangInitialized);

	PreprocessedShaderOutput preprocessedShader;
	if (!Preprocess(pathToFile, inputs, preprocessedShader))
	{
		// Log
		return false;
	}

	if (!glslangInitialized)
	{
		// TODO - glslang is initialized, however, it's not finalized at any point. This must be fixed somehow

	}
	bool result = false;
	EShLanguage stage = GetGlslangStage(inputs.shaderStage);
	const tchar* shaderCode = *preprocessedShader.outputGlsl;
	glslang::TShader shader(stage);
	shader.setStringsWithLengths(&shaderCode, nullptr, 1);
	shader.setEntryPoint(entryPoint);
	TBuiltInResource resources = DefaultTBuiltInResource;
	EShMessages messages = static_cast<EShMessages>(EShMsgVulkanRules | EShMsgSpvRules | EShMsgDefault);
	if (!shader.parse(&resources, 110, true, messages))
	{
		fmt::print("Error(s) in {}\n", pathToFile);
		const char* errors = shader.getInfoLog();
		fmt::print("{}\n", errors);
		// Log the errors....
		Assert(false);
	}
	else
	{
		glslang::TProgram program;
		program.addShader(&shader);
		if (!program.link(messages))
		{
			const char* errors = program.getInfoLog();
			fmt::print("{}\n", errors);
		}
		else
		{
			Assert(program.getIntermediate(stage));
			program.buildReflection();
			std::vector<u32> spirv;
			spv::SpvBuildLogger logger;
			glslang::GlslangToSpv(*program.getIntermediate(stage), spirv, &logger);
			// Log the messages from logger...

			// Reflection
			SpvReflectShaderModule module = {};
			NOT_USED SpvReflectResult spvResult = spvReflectCreateShaderModule(
				spirv.size() * sizeof(u32), spirv.data(), &module);
			Assert(spvResult == SPV_REFLECT_RESULT_SUCCESS);

			VulkanShaderHeader shaderHeader;
			shaderHeader.shaderStage = MusaStageToVkStage(inputs.shaderStage);
			shaderHeader.entryPoint = entryPoint;
			
			output.compiledOutput.stage = inputs.shaderStage;
			output.compiledOutput.shaderEntryPoint = entryPoint;

			// Inputs
			{
				u32 count;
				result = spvReflectEnumerateInputVariables(&module, &count, nullptr);
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				DynamicArray<SpvReflectInterfaceVariable*> shaderInputs(count);
				result = spvReflectEnumerateInputVariables(&module, &count, shaderInputs.GetData());
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				for (const auto& input : shaderInputs)
				{
					if (input->format != SPV_REFLECT_FORMAT_UNDEFINED)
					{
						ShaderVariable var = {};
						var.name = input->name;
						var.variableType = GetShaderIntrinsic(input->type_description->op);
						var.location = input->location;
						output.compiledOutput.locationToInputs.Add(var.location, var);
					}
				}
			}

			// Outputs
			{
				u32 count;
				result = spvReflectEnumerateOutputVariables(&module, &count, nullptr);
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				DynamicArray<SpvReflectInterfaceVariable*> shaderOutputs(count);
				result = spvReflectEnumerateOutputVariables(&module, &count, shaderOutputs.GetData());
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				for (const auto& out : shaderOutputs)
				{
					if (out->format != SPV_REFLECT_FORMAT_UNDEFINED)
					{
						ShaderVariable var = {};
						var.name = out->name;
						var.variableType = GetShaderIntrinsic(out->type_description->op);
						var.location = out->location;
						output.compiledOutput.locationToOutputs.Add(var.location, var);
					}
				}
			}


			// Constants
			{
				u32 count;
				result = spvReflectEnumerateDescriptorBindings(&module, &count, nullptr);
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				DynamicArray<SpvReflectDescriptorBinding*> shaderDescriptors(count);
				result = spvReflectEnumerateDescriptorBindings(&module, &count, shaderDescriptors.GetData());
				Assert(result == SPV_REFLECT_RESULT_SUCCESS);

				for (const auto& binding : shaderDescriptors)
				{
					ShaderConstant constant = {};
					// TODO - If a uniform is unnamed, it currently isn't supported...
					constant.name = binding->name;
					constant.bindingType = GetShaderConstantType(binding->descriptor_type);
					constant.binding = binding->binding;
					constant.size = binding->block.size;
					output.compiledOutput.bindingToConstants.Add(constant.binding, constant);

					switch (constant.bindingType)
					{
						case ShaderResourceType::UniformBuffer:
						case ShaderResourceType::StorageBuffer:
						case ShaderResourceType::UniformDynamicBuffer:
						case ShaderResourceType::StorageDynamicBuffer:
						{
							SpirvBuffer buffer = {};
							buffer.name = binding->name;
							buffer.bufferType = MusaConstantToDescriptorType(constant);
							buffer.bindIndex = binding->binding;
							shaderHeader.buffers.Add(buffer);
						}break;
						case ShaderResourceType::TextureSampler:
						{
							SpirvSampler sampler;
							sampler.name = binding->name;
							sampler.samplerType = MusaConstantToDescriptorType(constant);
							sampler.bindIndex = binding->binding;
							shaderHeader.samplers.Add(sampler);
						}break;
						default:
							break;
					}
				}
			}

			spvReflectDestroyShaderModule(&module);

			// Shader code output
			size_t compiledCodeSize = spirv.size() * sizeof(u32);
			DynamicArray<u32> spirvBytecode((u32)spirv.size());
			Memcpy(spirvBytecode.GetData(), compiledCodeSize, spirv.data(), compiledCodeSize);
			shaderHeader.bytecodeHash = fnv32(spirvBytecode.GetData(), spirvBytecode.SizeInBytes());

			MemorySerializer memorySer(output.compiledOutput.shaderCode);
			Serialize(memorySer, shaderHeader);
			Serialize(memorySer, spirvBytecode);

			// setup shader header
			{
				ShaderHeader& header = output.compiledOutput.header;
				for (const auto& constant : output.compiledOutput.bindingToConstants)
				{
					const ShaderConstant& shaderConst = constant.second;
					ShaderResourceInfo info = {};
					info.bindIndex = (u16)shaderConst.binding;
					info.size = (u16)shaderConst.size;
					info.type = shaderConst.bindingType;
					header.resourceNames.AddShaderResource(*shaderConst.name, info);
				}
				header.resourceTable = ConstructShaderConstantTable(header.resourceNames);
				header.stage = inputs.shaderStage;
				header.id.bytecodeHash = shaderHeader.bytecodeHash;
			}

			SpvReadBuf buf;
			std::ostream stream(&buf);
			spv::Disassemble(stream, spirv);
			output.readableShaderInfo.spirvByteCode = buf.str().c_str();
			output.readableShaderInfo.glslProcessedCode = preprocessedShader.outputGlsl;

			result = true;
		}
	}

	return result;
}
