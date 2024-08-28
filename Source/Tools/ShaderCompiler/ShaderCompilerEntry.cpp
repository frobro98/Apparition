// Copyright 2020, Nathan Blane

#include <stdio.h>

#include "Path/Path.hpp"
#include "String/CStringUtilities.hpp"
#include "Archiver/FileSerializer.hpp"
#include "File/DirectoryLocations.hpp"
#include "Shader/ShaderStages.hpp"

#include "ShaderCompiler.h"
#include "ShaderCompiledOutput.hpp"

constexpr const char* helpString =
"##-- Shader Compiler --##                                                        \n"
"    Usage: ShaderCompiler [Options] -f path/to/glsl_shader                       \n"
"                                                                                 \n"
"    Options:                                                                     \n"
"          -h               :: prints out compiler usage                          \n"
"          -p               :: preprocesses the glsl shader and saves the output  \n"
"          -o [output_file] :: designates an output file. If left out, this will  \n"
"                              default to \"filename.stage.spv\"                  \n"
"          -d def_name val  :: adds a preprocessor definition to be used when     \n"
"                              compiling the shader                               \n";

static ShaderStage::Type DetermineStageBasedOnExtension(const String& ext)
{
	if (ext == "vert")
	{
		return ShaderStage::Vertex;
	}
	else if (ext == "frag")
	{
		return ShaderStage::Fragment;
	}
	else if (ext == "geom")
	{
		return ShaderStage::Geometry;
	}
	else if (ext == "comp")
	{
		return ShaderStage::Compute;
	}
	else
	{
		printf("Invalid extension! Couldn't associate shader stage with \"%s\"\n", *ext);
	}

	return ShaderStage::MAX_WORD_SIZE;
}

static const char* DetermineSPVExtensionFrom(ShaderStage::Type stage)
{
	switch (stage)
	{
		case ShaderStage::Vertex:
			return ".mvs";
		case ShaderStage::Fragment:
			return ".mfs";
		case ShaderStage::Geometry:
			return ".mgs";
		case ShaderStage::TessalationEval:
			return ".mte";
		case ShaderStage::TessalationControl:
			return ".mtc";
		case ShaderStage::Compute:
			return ".mcs";
		default:
			return "";
	}
}

// static void SaveSPVInformation(const Path& outputShaderFile, const ShaderStructure& shaderInfo)
// {
// 	FileSystem::Handle h;
// 	bool result = FileSystem::OpenFile(h, outputShaderFile.GetString(), FileMode::Write);
// 	FILE_CHECK_VA(result, "Error when trying to open file %s\n", outputShaderFile.GetString());
// 
// 	result = FileSystem::WriteFile(h, shaderInfo.compiledOutput.shaderCode.GetData(), sizeof(u32) * (u32)shaderInfo.compiledOutput.shaderCode.Size());
// 	FILE_CHECK_VA(result, "Error when trying to write file %s\n", outputShaderFile.GetString());
// 
// 	result = FileSystem::CloseFile(h);
// 	FILE_CHECK_VA(result, "Error when trying to close file %s\n", outputShaderFile.GetString());
// }
// 
// static void SavePreprocessedGLSL(const Path& outputShaderFile, const PreprocessedShaderOutput& output)
// {
// 	FileSystem::Handle h;
// 	bool result = FileSystem::OpenFile(h, outputShaderFile.GetString(), FileMode::Write);
// 	FILE_CHECK_VA(result, "Error when trying to open file %s\n", outputShaderFile.GetString());
// 
// 	result = FileSystem::WriteFile(h, *output.outputGlsl, output.outputGlsl.Length());
// 	if (!result)
// 	{
// 		u32 err = GetLastError();
// 		printf("%u\n", err);
// 	}
// 	FILE_CHECK_VA(result, "Error when trying to write file %s\n", outputShaderFile.GetString());
// 
// 	result = FileSystem::CloseFile(h);
// 	FILE_CHECK_VA(result, "Error when trying to close file %s\n", outputShaderFile.GetString());
// }

static void PrintHelp()
{
	printf("%s", helpString);
}

// The shader compiler should compile the shader, but also serialize a header of metadata
i32 main(/*i32 argc, char *argv[]*/)
{
// 	if (argc < 2)
// 	{
// 		printf("You must pass in arguments describing what shaders to compile!\n\n");
// 		PrintHelp();
// 		return -1;
// 	}

// 	bool status = true;
// 	while (status) {}

	Path shaderPath;
	//const char* inputFile = nullptr;
	//const char* outputFile = nullptr;
	bool printHelp = false;
	Map<String, String> preprocessorDefinitions;
// 	for (i32 i = 1; i < argc; ++i)
// 	{
// 		if (Strcmp("-f", argv[i]) == 0 || Strcmp("-F", argv[i]) == 0)
// 		{
// 			++i;
// 			const tchar* s = Strchr(argv[i], '-');
// 			if (i >= argc || (s != nullptr && strlen(s) == 2))
// 			{
// 				printf("You must pass a correct type when using the -f option!\n");
// 				return -1;
// 			}
// 
// 			inputFile = argv[i];
// 		}
// 		else if (Strcmp("-o", argv[i]) == 0 || Strcmp("-O", argv[i]) == 0)
// 		{
// 			++i;
// 			const tchar* s = Strchr(argv[i], '-');
// 			if (i >= argc || (s != nullptr && strlen(s) == 2))
// 			{
// 				printf("You must pass a correct type when using the -o option!\n");
// 				return -1;
// 			}
// 			outputFile = argv[i];
// 		}
// 		else if (Strcmp("-p", argv[i]) == 0 || Strcmp("-P", argv[i]) == 0)
// 		{
// 			preprocessOnly = true;
// 		}
// 		else if (Strcmp("-d", argv[i]) == 0 || Strcmp("-D", argv[i]) == 0)
// 		{
// 			++i;
// 			constexpr i32 numDefArgs = 2;
// 			for (i32 arg = i; arg < numDefArgs; ++arg)
// 			{
// 				const tchar* s = Strchr(argv[arg], '-');
// 				if (i >= argc || (s != nullptr && strlen(s) == 2))
// 				{
// 					printf("Invalid preprocessor definition argument \"%s\"\n", argv[arg]);
// 					return -1;
// 				}
// 			}
// 
// 			preprocessorDefinitions.Add(argv[i], argv[i + 1]);
// 			++i;
// 		}
// 		else if (Strcmp("-h", argv[i]) == 0 || Strcmp("-H", argv[i]) == 0)
// 		{
// 			printHelp = true;
// 			break;
// 		}
// 		else
// 		{
// 			printf("Unknown argument \"%s\" was passed in\n", argv[i]);
// 		}
// 	}

	if (printHelp)
	{
		PrintHelp();
	}
	else
	{
		shaderPath = "../../../Assets/Shaders/Provided/ImGuiTransform.vert";
		String name = shaderPath.GetFileName();
		String nameNoExt = shaderPath.GetFileNameWithoutExtension();
		Path generatedShadersPath = shaderPath.GetDirectoryPath() / "../Generated/";
		if (!generatedShadersPath.DoesDirectoryExist())
		{
			FileSystem::MakeDirectory(generatedShadersPath);
		}
		// Getting information out of the file name
		if (!shaderPath.DoesFileExist())
		{
			printf("File named \"%s\" doesn't exist.\n", *name);
			return -1;
		}

		String extension = shaderPath.GetFileExtension();
		ShaderStage::Type stage = DetermineStageBasedOnExtension(extension);

		String processedShaderFileName = nameNoExt + DetermineSPVExtensionFrom(stage);
		u32 filenameHash = fnv32(*processedShaderFileName);
		Path outfile = generatedShadersPath / processedShaderFileName;

		ShaderCompilerDefinitions settings;
		settings.shaderStage = stage;
		settings.definitions = preprocessorDefinitions;

		InitializeCompiler();

		ShaderStructure resultingStructure;
		bool compiledSuccessfully = Compile(shaderPath.GetString(), "main", settings, resultingStructure);
		if (!compiledSuccessfully)
		{
			printf("File couldn't be compiled!\n");
			return -1;
		}

		// TODO - This is awful. With the refactor of the shader compiler, this needs to be better...
		resultingStructure.compiledOutput.header.id.shaderNameHash = filenameHash;
			
		FileSerializer serializer(outfile);
		Serialize(serializer, resultingStructure.compiledOutput.header);
		Serialize(serializer, resultingStructure.compiledOutput.shaderCode);

		DeinitializeCompiler();
	}

	return 0;
}