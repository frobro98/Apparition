
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
    StaticArray<Apparition::Buffer, maxConcurrentFrames> uniformBuffers{};
    StaticArray<Apparition::DescriptorSet, maxConcurrentFrames> descriptorSets{};
    Quat rotation;
};
StaticArray<Cube, 2> cubes;

vkglTF::Model model;

Apparition::Device device;
Apparition::Queue graphicsQueue;
Apparition::Pipeline pipeline;
Apparition::DescriptorSetLayout descriptorSetLayout;
Apparition::DescriptorPool descriptorPool;
Apparition::CommandPool commandPool;

StaticArray<Apparition::CommandBuffer, maxConcurrentFrames> drawCommandBuffers;
Apparition::Image depthStencilImage;
Apparition::ImageView depthStencilView;
}

void InitializeDescriptorSetsExample(Apparition::Device inDevice)
{
    // Store device internally
    device = inDevice;
    graphicsQueue = Apparition::AllocateGraphicsQueue(device);

    Apparition::BackbufferSetupParams backbufferSetupParams{
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
    cubes[0].texture.loadFromFile(tex0Path.GetString(), Apparition::ImageFormat::RGBA_8norm, device, graphicsQueue);
    cubes[1].texture.loadFromFile(tex1Path.GetString(), Apparition::ImageFormat::RGBA_8norm, device, graphicsQueue);

    for (auto& cube : cubes)
    {
        for (auto& buffer : cube.uniformBuffers)
        {
            Apparition::BufferCreationParams params
            {
                .usage = Apparition::BufferUsageFlagBits::UniformBuffer,
                .size = sizeof(Cube::Matrices),
                .supportsMappedMemory = true
            };
            buffer = Apparition::CreateBuffer(device, params);
            
        }
    }

    // DescriptorSetLayout
    Apparition::DescriptorSetLayoutCreationParams layoutParams
    {
        .bindings
        {
            Apparition::DescriptorSetLayoutDesc
            {
                .binding = 0,
                .descriptorType = Apparition::Descriptor::UniformBuffer,
                .descriptorCount = 1,
                .shaderStageFlags = Apparition::ShaderStageFlagBits::Vertex
            },
            Apparition::DescriptorSetLayoutDesc
            {
                .binding = 1,
                .descriptorType = Apparition::Descriptor::CombinedImageSampler,
                .descriptorCount = 1,
                .shaderStageFlags = Apparition::ShaderStageFlagBits::Fragment
            }
        }
    };
    descriptorSetLayout = Apparition::CreateDescriptorSetLayout(device, layoutParams);

    Apparition::DescriptorPoolCreationParams poolParams
    {
        .poolSizes
        {
            Apparition::DescriptorPoolSize
            {
                .poolType = Apparition::Descriptor::UniformBuffer,
                .size = cubes.Size() * maxConcurrentFrames
            },
            Apparition::DescriptorPoolSize
            {
                .poolType = Apparition::Descriptor::CombinedImageSampler,
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
            Apparition::DescriptorSetAllocParams params
            {
                .layout = descriptorSetLayout
            };
            cube.descriptorSets[i] = Apparition::AllocateDescriptorSet(descriptorPool, params);

            Apparition::BufferDescriptorInfo bufferDescriptor
            {
                .buffer = cube.uniformBuffers[i],
                .offset = 0,
                .range = sizeof(Cube::Matrices)
            };

            DynamicArray<Apparition::UpdateDescriptorSetDesc> updateDescriptors
            {
                {
                    .setBinding = 0,
                    .descriptorType = Apparition::Descriptor::UniformBuffer,
                    .descriptorSet = cube.descriptorSets[i],
                    .bufferDescriptor = &bufferDescriptor,
                },
                {
                    .setBinding = 1,
                    .descriptorType = Apparition::Descriptor::CombinedImageSampler,
                    .descriptorSet = cube.descriptorSets[i],
                    .imageDescriptor = &cube.texture.descriptor
                }
            };

            Apparition::UpdateDescriptorSets(updateDescriptors);
        }
    }

    // Pipeline
    Apparition::PipelineDescription pipelineDesc
    {
        .descriptorSets{ descriptorSetLayout }
    };

    Apparition::VertexInputPipelineState vertexInput;
    {
        Apparition::VertexInputPipelineStateCreationParams params
        {
            .primitiveTopology = Apparition::PrimitiveTopology::TriangleList,
            .attributes = vkglTF::Vertex::inputAttributeDescriptions(0, {vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV, vkglTF::VertexComponent::Color}),
            .bindings = { vkglTF::Vertex::inputBindingDescription(0) }
        };
        vertexInput = Apparition::CreateVertexInputPipelineState(device, params);
    }

    Apparition::PrerasterShadersPipelineState prerasterShaders;
    {
        const MemoryBuffer vertShaderCode = LoadShader("DescriptorSets/cube.vert.spv");
        DynamicArray<u32> vertShader(vertShaderCode.Size() / sizeof(u32));
        Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
        Apparition::PreRasterShadersPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .fillMode = Apparition::FillMode::Full,
            .cullingMode = Apparition::CullMode::Back,
            .frontFace = Apparition::FrontFace::CounterClockwise,
            .lineWidth = 1.f,
            .vertexShader =
            {
                .code = vertShader,
                .entryName = "main"
            }
        };

        prerasterShaders = Apparition::CreatePrerasterShadersPipelineState(device, params);
    }

    Apparition::FragmentShaderPipelineState fragmentShader;
    {
        MemoryBuffer fragShaderCode = LoadShader("DescriptorSets/cube.frag.spv");
        DynamicArray<u32> fragShader(fragShaderCode.Size() / sizeof(u32));
        Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
        const Apparition::FragmentShaderPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .fragmentShader =
            {
                .code = fragShader,
                .entryName = "main"
            },
            .depthTestEnabled = true,
            .depthWriteEnabled = true,
            .depthCompareOp = Apparition::CompareOperation::LessThanOrEqual
        };

        fragmentShader = Apparition::CreateFragmentShaderPipelineState(device, params);
    }

    Apparition::FragmentOutputPipelineState fragmentOutput;
    {
        Apparition::FragmentOutputPipelineStateCreationParams params
        {
            .attachments
            {
                Apparition::ColorBlendAttachment
                {
                    .srcColorFactor = Apparition::BlendFactor::Zero,
                    .dstColorFactor = Apparition::BlendFactor::Zero,
                    .colorBlendOperation = Apparition::BlendOperation::None,
                    .srcAlphaFactor = Apparition::BlendFactor::Zero,
                    .dstAlphaFactor = Apparition::BlendFactor::Zero,
                    .alphaBlendOperation = Apparition::BlendOperation::None,
                    .colorMask = Apparition::ColorComponentFlagBits::RGBA
                }
            },
            .colorAttachmentFormats
            {
                Apparition::GetBackbufferFormat(device),
            },
            .depthAttachmentFormat = Apparition::ImageFormat::DS_32f_8u,
            .stencilAttachmentFormat = Apparition::ImageFormat::DS_32f_8u
        };

        fragmentOutput = Apparition::CreateFragmentOutputPipelineState(device, params);

        {
            Apparition::PipelineCreationParams params
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
        Apparition::ImageCreationParams imageParams
        {
            .format = Apparition::ImageFormat::DS_32f_8u,
            .mipLevels = 1,
            .width = window->width,
            .height = window->height,
            .usageFlags = Apparition::ImageUsageFlagBits::DepthStencilAttachment
        };
        depthStencilImage = Apparition::CreateImage(device, imageParams);

        Apparition::ImageViewCreationParams viewParams
        {
            .aspect = Apparition::ImageAspect::DepthStencil,
            .baseMipLevel = 0,
            .format = Apparition::ImageFormat::DS_32f_8u,
            .mipCount = 1
        };
        depthStencilView = Apparition::CreateImageView(depthStencilImage, viewParams);
    }

    // Draw Command Buffers
    {
        Apparition::CommandPoolCreationParams createParams{
            .queueIndex = Apparition::GetQueueIndex(graphicsQueue),
            .canResetCommandBuffers = true
        };
        commandPool = Apparition::CreateCommandPool(device, createParams);
    }

    {
        Apparition::CommandBufferAllocParams allocParams{
            .isSecondary = false
        };
        drawCommandBuffers[0] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
        drawCommandBuffers[1] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
    }
}

static u32 currentBuffer = 0;

void TickDescriptorSetsExample(/*Apparition::Device device*/)
{
    Apparition::BackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
    Assert(preparationStatus != Apparition::BackbufferStatus::Unavailable);
    Apparition::ImageView backbufferView = Apparition::GetBackBufferImageView(device);

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

    Apparition::CommandBuffer commandBuffer = drawCommandBuffers[currentBuffer];
    Apparition::BeginCommandBuffer(commandBuffer);

    {
        Apparition::ImageMemoryBarrierDesc barrierDesc = {
            .image = Apparition::GetAcquiredBackbufferImage(device),
            .access = Apparition::ImageAccess::ColorWrite,
            .aspect = Apparition::ImageAspect::Color,
            .mipLevelCount = 1
        };

        Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);

        barrierDesc = {
            .image = depthStencilImage,
            .access = Apparition::ImageAccess::DepthStencilWrite,
            .aspect = Apparition::ImageAspect::DepthStencil,
            .mipLevelCount = 1
        };

        Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
    }

    Apparition::RenderAttachment colorAttachment
    {
        .imageView = backbufferView,
        .loadStoreOps = Apparition::AttachmentOperations::Clear_Store,
        .clearValue = {{0.025f, 0.025f, 0.025f, 1.0f}}
    };

    Apparition::RenderAttachment depthAttachment
    {
        .imageView = depthStencilView,
        .loadStoreOps = Apparition::AttachmentOperations::Clear_Store,
        .clearValue = {.depthStencil{1.f, 0}}
    };

    const u32 backbufferWidth = Apparition::GetBackbufferWidth(device);
    const u32 backbufferHeight = Apparition::GetBackbufferHeight(device);


    Apparition::RenderSetupParams renderSetup = {};
    renderSetup.colorAttachments.Add(colorAttachment);
    renderSetup.depthAttachment = depthAttachment;
    renderSetup.renderWidth = backbufferWidth;
    renderSetup.renderHeight = backbufferHeight;
    Apparition::BeginRendering(commandBuffer, renderSetup);

    Apparition::BindGraphicsPipeline(commandBuffer, pipeline);

    Apparition::ViewportDesc viewportDesc = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(backbufferWidth),
        .height = static_cast<float>(backbufferHeight)
    };
    Apparition::ScissorDesc scissorDesc = {
        .offsetX = 0,
        .offsetY = 0,
        .extentX = backbufferWidth,
        .extentY = backbufferHeight
    };
    Apparition::SetViewportAndScissor(commandBuffer, viewportDesc, scissorDesc);

    model.bindBuffers(commandBuffer);

    for (auto cube : cubes)
    {
        Apparition::BindDescriptorSetsDesc bindDesc
        {
            .bindPoint = Apparition::BindPoint::Graphics,
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
        Apparition::ImageMemoryBarrierDesc barrierDesc = {
            .image = Apparition::GetAcquiredBackbufferImage(device),
            .access = Apparition::ImageAccess::Present,
            .aspect = Apparition::ImageAspect::Color,
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