
#include "DescriptorSets.h"

#include "Math/MathFunctions.hpp"
#include "Math/Matrix4.hpp"
#include "Math/MatrixFunctions.hpp"
#include "Math/Quat.hpp"
#include "Math/Vector3.hpp"

#include "glTFModel/VulkanglTFModel.h"
#include "glTFModel/VulkanTexture.h"

extern f32 cameraRotationX;
extern f32 cameraRotationY;

namespace
{
struct Cube
{
    struct Matrices
    {
        Matrix4 projection;
        Matrix4 view;
        Matrix4 model;
    } matrices;
    vks::Texture2D texture;
    StaticArray<AptnBuffer, maxConcurrentFrames> uniformBuffers{};
    StaticArray<AptnDescriptorSet, maxConcurrentFrames> descriptorSets{};
    Quat rotation;
};
StaticArray<Cube, 2> cubes;

vkglTF::Model model;

AptnDevice device;
AptnQueue graphicsQueue;
AptnPipeline pipeline;
AptnDescriptorSetLayout descriptorSetLayout;
AptnDescriptorPool descriptorPool;
AptnCommandPool commandPool;

StaticArray<AptnCommandBuffer, maxConcurrentFrames> drawCommandBuffers;
AptnImage depthStencilImage;
AptnImageView depthStencilView;
}

void InitializeDescriptorSetsExample(AptnDevice inDevice)
{
    // Store device internally
    device = inDevice;
    graphicsQueue = Apparition::AllocateGraphicsQueue(device);

    AptnBackbufferSetupParams backbufferSetupParams{
        .wndHandle = window->windowHandle,
        .wndWidth = window->width,
        .wndHeight = window->height
    };
    Apparition::SetupBackbuffer(device, backbufferSetupParams);

    const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;
    Path cubeModelPath = Path(SandboxModelPath()) / "cube.gltf";
    Path tex0Path = Path(SandboxTexturePath()) / "crate01_color_height_rgba.ktx";
    Path tex1Path = Path(SandboxTexturePath()) / "crate02_color_height_rgba.ktx";
    model.loadFromFile(cubeModelPath.GetString(), device, graphicsQueue, glTFLoadingFlags);
    cubes[0].texture.loadFromFile(tex0Path.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);
    cubes[1].texture.loadFromFile(tex1Path.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);

    for (auto& cube : cubes)
    {
        for (auto& buffer : cube.uniformBuffers)
        {
            AptnBufferCreationParams params
            {
                .usage = AptnBufferUsageFlags::UniformBuffer,
                .size = sizeof(Cube::Matrices),
                .supportsMappedMemory = true
            };
            buffer = Apparition::CreateBuffer(device, params);
            
        }
    }

    // DescriptorSetLayout
    AptnDescriptorSetLayoutCreationParams layoutParams
    {
        .bindings
        {
            AptnDescriptorSetLayoutDesc
            {
                .binding = 0,
                .descriptorType = AptnDescriptor::UniformBuffer,
                .descriptorCount = 1,
                .shaderStageFlags = AptnShaderStageFlags::Vertex
            },
            AptnDescriptorSetLayoutDesc
            {
                .binding = 1,
                .descriptorType = AptnDescriptor::CombinedImageSampler,
                .descriptorCount = 1,
                .shaderStageFlags = AptnShaderStageFlags::Fragment
            }
        }
    };
    descriptorSetLayout = Apparition::CreateDescriptorSetLayout(device, layoutParams);

    AptnDescriptorPoolCreationParams poolParams
    {
        .poolSizes
        {
            AptnDescriptorPoolSize
            {
                .poolType = AptnDescriptor::UniformBuffer,
                .size = cubes.Size() * maxConcurrentFrames
            },
            AptnDescriptorPoolSize
            {
                .poolType = AptnDescriptor::CombinedImageSampler,
                .size = cubes.Size() * maxConcurrentFrames
            },
        }
    };
    descriptorPool = Apparition::CreateDescriptorPool(device, poolParams);

    // DescriptorSets
    for (auto& cube : cubes)
    {
        for (i32 i = 0; i < cube.uniformBuffers.Size(); ++i)
        {
            AptnDescriptorSetAllocParams params
            {
                .layout = descriptorSetLayout
            };
            cube.descriptorSets[i] = Apparition::AllocateDescriptorSet(descriptorPool, params);

            AptnBufferDescriptorInfo bufferDescriptor
            {
                .buffer = cube.uniformBuffers[i],
                .offset = 0,
                .range = sizeof(Cube::Matrices)
            };

            DynamicArray<AptnUpdateDescriptorSetDesc> updateDescriptors
            {
                {
                    .setBinding = 0,
                    .descriptorType = AptnDescriptor::UniformBuffer,
                    .descriptorSet = cube.descriptorSets[i],
                    .bufferDescriptor = &bufferDescriptor,
                },
                {
                    .setBinding = 1,
                    .descriptorType = AptnDescriptor::CombinedImageSampler,
                    .descriptorSet = cube.descriptorSets[i],
                    .imageDescriptor = &cube.texture.descriptor
                }
            };

            Apparition::UpdateDescriptorSets(updateDescriptors);
        }
    }

    // Pipeline
    AptnPipelineDescription pipelineDesc
    {
        .descriptorSets{ descriptorSetLayout }
    };

    AptnVertexInputPipelineState vertexInput;
    {
        AptnVertexInputPipelineStateCreationParams params
        {
            .primitiveTopology = AptnPrimitiveTopology::TriangleList,
            .attributes = vkglTF::Vertex::inputAttributeDescriptions(0, {vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV, vkglTF::VertexComponent::Color}),
            .bindings = { vkglTF::Vertex::inputBindingDescription(0) }
        };
        vertexInput = Apparition::CreateVertexInputPipelineState(device, params);
    }

    AptnPrerasterShadersPipelineState prerasterShaders;
    {
        const MemoryBuffer vertShaderCode = LoadShader("DescriptorSets/cube.vert.spv");
        DynamicArray<u32> vertShader(vertShaderCode.Size() / sizeof(u32));
        Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
        AptnPreRasterShadersPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .fillMode = AptnFillMode::Full,
            .cullingMode = AptnCullMode::Back,
            .frontFace = AptnFrontFace::CounterClockwise,
            .lineWidth = 1.f,
            .vertexShader =
            {
                .code = vertShader,
                .entryName = "main"
            }
        };

        prerasterShaders = Apparition::CreatePrerasterShadersPipelineState(device, params);
    }

    AptnFragmentShaderPipelineState fragmentShader;
    {
        MemoryBuffer fragShaderCode = LoadShader("DescriptorSets/cube.frag.spv");
        DynamicArray<u32> fragShader(fragShaderCode.Size() / sizeof(u32));
        Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
        const AptnFragmentShaderPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .fragmentShader =
            {
                .code = fragShader,
                .entryName = "main"
            },
            .depthTestEnabled = true,
            .depthWriteEnabled = true,
            .depthCompareOp = AptnCompareOperation::LessThanOrEqual
        };

        fragmentShader = Apparition::CreateFragmentShaderPipelineState(device, params);
    }

    AptnFragmentOutputPipelineState fragmentOutput;
    {
        AptnFragmentOutputPipelineStateCreationParams params
        {
            .attachments
            {
                AptnColorBlendAttachment
                {
                    .srcColorFactor = AptnBlendFactor::Zero,
                    .dstColorFactor = AptnBlendFactor::Zero,
                    .colorBlendOperation = AptnBlendOperation::None,
                    .srcAlphaFactor = AptnBlendFactor::Zero,
                    .dstAlphaFactor = AptnBlendFactor::Zero,
                    .alphaBlendOperation = AptnBlendOperation::None,
                    .colorMask = AptnColorComponentFlags::RGBA
                }
            },
            .colorAttachmentFormats
            {
                Apparition::GetBackbufferFormat(device),
            },
            .depthAttachmentFormat = AptnImageFormat::DS_32f_8u,
            .stencilAttachmentFormat = AptnImageFormat::DS_32f_8u
        };

        fragmentOutput = Apparition::CreateFragmentOutputPipelineState(device, params);

        {
            AptnPipelineCreationParams params
            {
                .pipelineDesc = pipelineDesc,
                .vertexInput = vertexInput,
                .prerasterShaders = prerasterShaders,
                .fragmentShader = fragmentShader,
                .fragmentOutput = fragmentOutput
            };

            pipeline = Apparition::CreatePipeline(device, params);
        }
    }

    // Depth
    {
        AptnImageCreationParams imageParams
        {
            .format = AptnImageFormat::DS_32f_8u,
            .mipLevels = 1,
            .width = window->width,
            .height = window->height,
            .usageFlags = AptnImageUsageFlags::DepthStencilAttachment
        };
        depthStencilImage = Apparition::CreateImage(device, imageParams);

        AptnImageViewCreationParams viewParams
        {
            .aspect = AptnImageAspectFlags::DepthStencil,
            .baseMipLevel = 0,
            .format = AptnImageFormat::DS_32f_8u,
            .mipCount = 1
        };
        depthStencilView = Apparition::CreateImageView(depthStencilImage, viewParams);
    }

    // Draw Command Buffers
    {
        AptnCommandPoolCreationParams createParams{
            .queueIndex = Apparition::GetQueueIndex(graphicsQueue),
            .canResetCommandBuffers = true
        };
        commandPool = Apparition::CreateCommandPool(device, createParams);
    }

    {
        AptnCommandBufferAllocParams allocParams{
            .isSecondary = false
        };
        drawCommandBuffers[0] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
        drawCommandBuffers[1] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
    }
}

static u32 currentBuffer = 0;

void TickDescriptorSetsExample(/*Apparition::Device device*/)
{
    AptnBackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
    Assert(preparationStatus != AptnBackbufferStatus::Unavailable);
    AptnImageView backbufferView = Apparition::GetBackBufferImageView(device);

    // Update uniform buffers
    cubes[0].matrices.model = Matrix4(TRANS, -2.f, 0.f, 0.f);
    cubes[1].matrices.model = Matrix4(TRANS, 1.5f, .5f, 0.f);

    for (auto& cube : cubes)
    {
        cube.matrices.projection = Math::ConstructPerspectiveMatrix(60.f, (float)window->width / (float)window->height, 0.1f, 512.f);
        const f32 radRotX = Math::DegreesToRadians(cameraRotationX);
        const f32 radRotY = Math::DegreesToRadians(cameraRotationY);
        constexpr bool orbit = true;
        cube.matrices.view = Math::ConstructViewMatrix(Vector4(0.f, 0.f, -5.f), Quat(ROT_XYZ, radRotX, radRotY, 0.f), orbit);
        //*
        cube.matrices.model = Quat(Vector4::RightAxis, Math::DegreesToRadians(cube.rotation.x)) * cube.matrices.model;
        cube.matrices.model = Quat(Vector4::UpAxis, Math::DegreesToRadians(cube.rotation.y)) * cube.matrices.model;
        cube.matrices.model = Quat(Vector4::ForwardAxis, Math::DegreesToRadians(cube.rotation.z)) * cube.matrices.model;
        cube.matrices.model = Matrix4(SCALE, .25f) * cube.matrices.model;
        //*/

        void* data = Apparition::MapBuffer(cube.uniformBuffers[currentBuffer]);
        memcpy(data, &cube.matrices, sizeof(Cube::Matrices));
        Apparition::UnmapBuffer(cube.uniformBuffers[currentBuffer]);
    }

    AptnCommandBuffer commandBuffer = drawCommandBuffers[currentBuffer];
    Apparition::BeginCommandBuffer(commandBuffer);

    {
        AptnImageMemoryBarrierDesc barrierDesc = {
            .image = Apparition::GetAcquiredBackbufferImage(device),
            .access = AptnImageAccess::ColorWrite,
            .aspect = AptnImageAspectFlags::Color,
            .mipLevelCount = 1
        };

        Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);

        barrierDesc = {
            .image = depthStencilImage,
            .access = AptnImageAccess::DepthStencilWrite,
            .aspect = AptnImageAspectFlags::DepthStencil,
            .mipLevelCount = 1
        };

        Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
    }

    AptnRenderAttachment colorAttachment
    {
        .imageView = backbufferView,
        .loadStoreOps = AptnAttachmentOperations::Clear_Store,
        .clearValue = {{0.025f, 0.025f, 0.025f, 1.0f}}
    };

    AptnRenderAttachment depthAttachment
    {
        .imageView = depthStencilView,
        .loadStoreOps = AptnAttachmentOperations::Clear_Store,
        .clearValue = {.depthStencil{1.f, 0}}
    };

    const u32 backbufferWidth = Apparition::GetBackbufferWidth(device);
    const u32 backbufferHeight = Apparition::GetBackbufferHeight(device);


    AptnRenderSetupParams renderSetup = {};
    renderSetup.colorAttachments.Add(colorAttachment);
    renderSetup.depthAttachment = depthAttachment;
    renderSetup.renderWidth = backbufferWidth;
    renderSetup.renderHeight = backbufferHeight;
    Apparition::BeginRendering(commandBuffer, renderSetup);

    Apparition::BindGraphicsPipeline(commandBuffer, pipeline);

    // TODO - Push this functionality into the API
    // This is the official way to translate "normal" drawing into Vulkan viewport space.
    // There used to be a projection matrix change that allowed the -y direction to be up
    // which is what Vulkan expects. This viewport change prevents this issue and also 
    // allows the usual winding order, which the projection change would reverse
    //AptnViewportDesc viewportDesc = {
    //    .x = 0.f,
    //    .y = static_cast<float>(backbufferHeight),
    //    .width = static_cast<float>(backbufferWidth),
    //    .height = -static_cast<float>(backbufferHeight)
    //};
    AptnViewportDesc viewportDesc = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(backbufferWidth),
        .height = static_cast<float>(backbufferHeight)
    };
    AptnScissorDesc scissorDesc = {
        .offsetX = 0,
        .offsetY = 0,
        .extentX = backbufferWidth,
        .extentY = backbufferHeight
    };
    Apparition::SetViewportAndScissor(commandBuffer, viewportDesc, scissorDesc);

    model.bindBuffers(commandBuffer);

    for (auto cube : cubes)
    {
        AptnBindDescriptorSetsDesc bindDesc
        {
            .bindPoint = AptnBindPoint::Graphics,
            .pipelineDesc =
            {
                .descriptorSets{ descriptorSetLayout }
            },
            .descriptorSets{cube.descriptorSets[currentBuffer]},
            .firstSet = 0
        };
        Apparition::BindDescriptorSets(commandBuffer, bindDesc);
        model.draw(commandBuffer);
    }

    Apparition::EndRendering(commandBuffer);

    {
        AptnImageMemoryBarrierDesc barrierDesc = {
            .image = Apparition::GetAcquiredBackbufferImage(device),
            .access = AptnImageAccess::Present,
            .aspect = AptnImageAspectFlags::Color,
            .mipLevelCount = 1
        };

        Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
    }

    Apparition::EndCommandBuffer(commandBuffer);

    Apparition::SubmitBackbufferCommandBuffer(commandBuffer, graphicsQueue);
    Apparition::PresentBackbuffer(graphicsQueue);

    Apparition::WaitForIdle(graphicsQueue);

    currentBuffer = (currentBuffer + 1) % maxConcurrentFrames;
}

void DestroyDescriptorSetsExample(/*Apparition::Device device*/)
{
    Apparition::WaitForIdle(graphicsQueue);

    Apparition::DestroyImageView(depthStencilView);
    Apparition::DestroyImage(depthStencilImage);
    
    Apparition::FreeCommandBuffer(drawCommandBuffers[0]);
    Apparition::FreeCommandBuffer(drawCommandBuffers[1]);
    Apparition::DestroyCommandPool(commandPool);
    
    Apparition::DestroyPipeline(pipeline);

    model.releaseResources();

    for (auto& cube : cubes)
    {
        for (i32 i = 0; i < cube.uniformBuffers.Size(); ++i)
        {
            Apparition::FreeDescriptorSet(cube.descriptorSets[i]);
            Apparition::DestroyBuffer(cube.uniformBuffers[i]);
        }
    }

    Apparition::DestroyDescriptorPool(descriptorPool);
    Apparition::DestroyDescriptorSetLayout(descriptorSetLayout);
}