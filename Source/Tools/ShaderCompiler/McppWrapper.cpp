// Copyright 2020, Nathan Blane

#include "McppWrapper.hpp"
#include "Debugging/Assertion.hpp"
#include "String/String.h"
#include "File/DirectoryLocations.hpp"
#include "File/FileSystem.hpp"
#include "Path/Path.hpp"

static void AddDefinition(const String& macro, const String& macroDef, String& definitions)
{
	definitions += " -D";
	definitions += macro + "=" + macroDef;
}

ShaderPreprocessor::ShaderPreprocessor(ShaderStage::Type stage, const Map<String, String>& definitions)
	: preprocessorDefinitions(definitions),
	shaderStage(stage)
{
}

void ShaderPreprocessor::Preprocess(const Path& pathToShader)
{
	String options;
	for (const auto& defPair : preprocessorDefinitions)
	{
		AddDefinition(defPair.first, defPair.second, options);
	}

	ansichar* output = nullptr;
	ansichar* error = nullptr;
	Path absolutePath = pathToShader.GetAbsolute();
	mcppCode = mcpp_run(*options, absolutePath.GetString(), &output, &error, GetLoader());

	preprocessedOutput = output;
	if (error)
	{
		preprocessError = error;
	}

// 	delete output;
// 	delete error;
}

const char* ShaderPreprocessor::ErrorString() const
{
	return preprocessError.IsEmpty() ? "" : *preprocessError;
}

bool ShaderPreprocessor::Success() const
{
	return mcppCode == 0;
}

const char* ShaderPreprocessor::PreprocessedOutput() const
{
	return *preprocessedOutput;
}

int ShaderPreprocessor::Mcpp_Process(void* userData, const char* filename, const char** outContents, size_t* outContentsSize)
{
	ShaderPreprocessor* preprocessor = reinterpret_cast<ShaderPreprocessor*>(userData);
	auto& fileContentsMap = preprocessor->filePathToContents;
	String path(filename);
	DynamicArray<char>* fileContents;

	// TODO - This copies the data from one array to the other...consider adding callback functionality
	fileContents = fileContentsMap.Find(path);
	if (!fileContents)
	{
		FileSystem::Handle shaderFile = 0;
		bool result = FileSystem::OpenFile(shaderFile, filename, FileMode::Read);
		Assert(result);

		u32 size = (u32)FileSystem::FileSize(shaderFile);

		if (size > 0)
		{
			DynamicArray<tchar> data(size + 1);
			result = FileSystem::ReadFile(shaderFile, data.GetData(), size);
			Assert(result);

			data[size] = 0;
			fileContentsMap.Add(path, data);
			fileContents = fileContentsMap.Find(path);
			Assert(fileContents);
		}

		result = FileSystem::CloseFile(shaderFile);
		Assert(result);
	}

	if (outContents)
	{
		*outContents = fileContents != nullptr ? fileContents->GetData() : nullptr;
	}
	if (outContentsSize)
	{
		*outContentsSize = fileContents != nullptr ? fileContents->Size() : 0;
	}

	return fileContents != nullptr;
}

file_loader ShaderPreprocessor::GetLoader()
{
	file_loader mcppLoader = {};
	mcppLoader.user_data = this;
	mcppLoader.get_file_contents = &ShaderPreprocessor::Mcpp_Process;
	return mcppLoader;
}
