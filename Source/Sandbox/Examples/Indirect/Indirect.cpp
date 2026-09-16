
#include "Indirect.h"

#include "glTFModel/VulkanglTFModel.h"
#include "glTFModel/VulkanTexture.h"

#define OBJECT_INSTANCE_COUNT 2048
// Circular range of plant distribution
#define PLANT_RADIUS 25.0f

extern f32 cameraRotationX;
extern f32 cameraRotationY;

namespace
{
struct
{
    vks::Texture2DArray plants;
    vks::Texture2D ground;
} textures;

struct
{
    vkglTF::Model plants;
    vkglTF::Model ground;
    vkglTF::Model skysphere;
} models;

struct InstanceData
{
    Vector3 pos;
    Vector3 rot;
    float scale;
    u32 texIndex;
};

AptnDevice device;
AptnQueue graphicsQueue;

AptnBuffer instanceBuffer;
AptnBuffer indirectCommandsBuffer;
u32 indirectDrawCount;

struct UniformData
{
    Matrix4 projection;
    Matrix4 view;
} uniformData;

struct UniformBuffer
{
    AptnBuffer buffer;
    AptnBufferDescriptorInfo descriptorInfo;
};
StaticArray<UniformBuffer, maxConcurrentFrames> uniformBuffers;

struct
{
    AptnPipeline plants;
    AptnPipeline ground;
    AptnPipeline skysphere;
} pipelines;

AptnDescriptorPool descriptorPool;
AptnDescriptorSetLayout descriptorSetLayout;
StaticArray<AptnDescriptorSet, maxConcurrentFrames> descriptorSets{};

u32 objectCount = 0;

DynamicArray<AptnDrawIndexedIndirectCommand> indirectCommands;

AptnCommandPool commandPool;
StaticArray<AptnCommandBuffer, maxConcurrentFrames> drawCommandBuffers;
AptnImage depthStencilImage;
AptnImageView depthStencilView;
}

void InitializeIndirectExample(AptnDevice inDevice)
{
    device = inDevice;
    graphicsQueue = Apparition::AllocateGraphicsQueue(device);

    AptnBackbufferSetupParams backbufferSetupParams{
        .wndHandle = window->windowHandle,
        .wndWidth = window->width,
        .wndHeight = window->height
    };
    Apparition::SetupBackbuffer(device, backbufferSetupParams);

    const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;
    Path plantsModelPath = Path(SandboxModelPath()) / "plants.gltf";
    Path groundModelPath = Path(SandboxModelPath()) / "plane_circle.gltf";
    Path skysphereModelPath = Path(SandboxModelPath()) / "sphere.gltf";
    Path plantsTexturePath = Path(SandboxTexturePath()) / "texturearray_plants_rgba.ktx";
    Path groundTexturePath = Path(SandboxTexturePath()) / "ground_dry_rgba.ktx";
    models.plants.loadFromFile(plantsModelPath.GetString(), device, graphicsQueue, glTFLoadingFlags);
    models.ground.loadFromFile(groundModelPath.GetString(), device, graphicsQueue, glTFLoadingFlags);
    models.skysphere.loadFromFile(skysphereModelPath.GetString(), device, graphicsQueue, glTFLoadingFlags);
    textures.plants.loadFromFile(plantsTexturePath.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);
    textures.ground.loadFromFile(groundTexturePath.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);

    indirectCommands.Clear();

    // Create an indirect command for note in the scene with a mesh attached to it
    u32 m = 0;
    for (auto& node : models.plants.nodes)
    {
        if (node->mesh)
        {
            AptnDrawIndexedIndirectCommand indirectCommand
            {
                .instanceCount = OBJECT_INSTANCE_COUNT,
                .firstInstance = m * OBJECT_INSTANCE_COUNT,
                // NOTE(Sacha.Willems): A glTF node may consist of multiple primitives, but for this example we only care for the first primitive
                .firstIndex = node->mesh->primitives[0]->firstIndex,
                .indexCount = node->mesh->primitives[0]->indexCount
            };
            indirectCommands.Add(indirectCommand);
            ++m;
        }
    }

    indirectDrawCount = indirectCommands.Size();

    objectCount = 0;
    for (const AptnDrawIndexedIndirectCommand& indirectCommand : indirectCommands)
    {
        objectCount += indirectCommand.instanceCount;
    }

    {
        AptnCommandPoolCreationParams poolParams
        {
            .queueIndex = Apparition::GetQueueIndex(graphicsQueue)
        };
        commandPool = Apparition::CreateCommandPool(device, poolParams);
    }

    // Copy data to indirect buffer
    {
        AptnBufferCreationParams stagingBufferParams
        {
            .size = indirectCommands.SizeInBytes(),
            .usage = AptnBufferUsageFlags::TransferSrc,
            .supportsMappedMemory = true
        };
        AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, stagingBufferParams);

        {
            void* data = Apparition::MapBuffer(stagingBuffer);
            Memcpy(data, indirectCommands.GetData(), indirectCommands.SizeInBytes());
            Apparition::UnmapBuffer(stagingBuffer);
        }

        AptnBufferCreationParams indirectBufferParams
        {
            .usage = AptnBufferUsageFlags::IndirectBuffer | AptnBufferUsageFlags::TransferDst,
            .size = indirectCommands.SizeInBytes()
        };
        indirectCommandsBuffer = Apparition::CreateBuffer(device, indirectBufferParams);

        AptnCommandBuffer copyBuffer = Apparition::AllocateCommandBuffer(commandPool);

        Apparition::BeginCommandBuffer(copyBuffer, true);
        AptnBufferCopyDesc copyDesc
        {
            .srcBuffer = stagingBuffer,
            .dstBuffer = indirectCommandsBuffer,
            .size = indirectCommands.SizeInBytes()
        };
        Apparition::CopyBuffer(copyBuffer, copyDesc);
        Apparition::EndCommandBuffer(copyBuffer);

        Apparition::SubmitCommandBuffer(graphicsQueue, copyBuffer);
        Apparition::WaitForIdle(graphicsQueue);

        Apparition::FreeCommandBuffer(copyBuffer);
        Apparition::DestroyBuffer(stagingBuffer);
    }

    // Initialize Instance Data
    DynamicArray<InstanceData> instanceData(objectCount);

    std::default_random_engine rndEngine((u32)time(nullptr));
    std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);

    for (uint32_t i = 0; i < objectCount; i++) {
        float theta = 2 * float(Math::Pi) * uniformDist(rndEngine);
        float phi = acos(1 - 2 * uniformDist(rndEngine));
        instanceData[i].rot = Vector3(0.0f, float(Math::Pi) * uniformDist(rndEngine), 0.0f);
        instanceData[i].pos = Vector3(Math::Sin(phi) * Math::Cos(theta), 0.0f, Math::Cos(phi)) * PLANT_RADIUS;
        instanceData[i].scale = 1.0f + uniformDist(rndEngine) * 2.0f;
        instanceData[i].texIndex = i / OBJECT_INSTANCE_COUNT;
    }

    // Copy data to instance buffer
    {
        AptnBufferCreationParams stagingBufferParams
        {
            .usage = AptnBufferUsageFlags::TransferSrc,
            .size = instanceData.SizeInBytes(),
            .supportsMappedMemory = true
        };
        AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, stagingBufferParams);

        {
            void* data = Apparition::MapBuffer(stagingBuffer);
            Memcpy(data, instanceData.GetData(), instanceData.SizeInBytes());
            Apparition::UnmapBuffer(stagingBuffer);
        }

        AptnBufferCreationParams instanceBufferParams
        {
            .usage = AptnBufferUsageFlags::VertexBuffer | AptnBufferUsageFlags::TransferDst,
            .size = instanceData.SizeInBytes(),
        };
        instanceBuffer = Apparition::CreateBuffer(device, instanceBufferParams);

        AptnCommandBuffer copyBuffer = Apparition::AllocateCommandBuffer(commandPool);

        Apparition::BeginCommandBuffer(copyBuffer, true);
        AptnBufferCopyDesc copyDesc
        {
            .srcBuffer = stagingBuffer,
            .dstBuffer = instanceBuffer,
            .size = instanceData.SizeInBytes()
        };
        Apparition::CopyBuffer(copyBuffer, copyDesc);
        Apparition::EndCommandBuffer(copyBuffer);

        Apparition::SubmitCommandBuffer(graphicsQueue, copyBuffer);
        Apparition::WaitForIdle(graphicsQueue);

        Apparition::FreeCommandBuffer(copyBuffer);
        Apparition::DestroyBuffer(stagingBuffer);
    }

    // Initialize UniformBuffers
    for (auto& uniformBuffer : uniformBuffers)
    {
        AptnBufferCreationParams bufferParams
        {
            .usage = AptnBufferUsageFlags::UniformBuffer,
            .size = sizeof(UniformData),
            .supportsMappedMemory = true
        };
        uniformBuffer.buffer = Apparition::CreateBuffer(device, bufferParams);

        uniformBuffer.descriptorInfo = AptnBufferDescriptorInfo
        {
            .buffer = uniformBuffer.buffer,
            .range = sizeof(UniformData)
        };
    }

    constexpr StaticArray poolSizes = {
        AptnDescriptorPoolSize
        {
            .poolType = AptnDescriptor::UniformBuffer,
            .size = maxConcurrentFrames
        },
        AptnDescriptorPoolSize
        {
            .poolType = AptnDescriptor::CombinedImageSampler,
            .size = maxConcurrentFrames * 2
        }
    };
    AptnDescriptorPoolCreationParams poolParams
    {
        .poolSizes = poolSizes
    };
    descriptorPool = Apparition::CreateDescriptorPool(device, poolParams);

    constexpr StaticArray bindings = {
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
        },
        AptnDescriptorSetLayoutDesc
        {
            .binding = 2,
            .descriptorType = AptnDescriptor::CombinedImageSampler,
            .descriptorCount = 1,
            .shaderStageFlags = AptnShaderStageFlags::Fragment
        }
    };

    AptnDescriptorSetLayoutCreationParams layoutParams
    {
        .bindings = bindings
    };
    descriptorSetLayout = Apparition::CreateDescriptorSetLayout(device, layoutParams);

    AptnDescriptorSetAllocParams allocParams
    {
        .layout = descriptorSetLayout
    };
    for (u32 i = 0; i < maxConcurrentFrames; ++i) 
    {
        descriptorSets[i] = Apparition::AllocateDescriptorSet(descriptorPool, allocParams);

        DynamicArray<AptnUpdateDescriptorSetDesc> updateDescriptorSets = {
            AptnUpdateDescriptorSetDesc
            {
                .descriptorSet = descriptorSets[i],
                .setBinding = 0,
                .descriptorType = AptnDescriptor::UniformBuffer,
                .bufferDescriptor = &uniformBuffers[i].descriptorInfo
            },
            AptnUpdateDescriptorSetDesc
            {
                .descriptorSet = descriptorSets[i],
                .setBinding = 1,
                .descriptorType = AptnDescriptor::CombinedImageSampler,
                .imageDescriptor = &textures.plants.descriptor
            },
            AptnUpdateDescriptorSetDesc
            {
                .descriptorSet = descriptorSets[i],
                .setBinding = 2,
                .descriptorType = AptnDescriptor::CombinedImageSampler,
                .imageDescriptor = &textures.ground.descriptor
            }
        };
        Apparition::UpdateDescriptorSets(updateDescriptorSets);
    }

    const StaticArray descriptorSetLayouts = { descriptorSetLayout };
    // Create pipelines
    AptnPipelineDescription pipelineDesc
    { 
        .descriptorSets = descriptorSetLayouts
    };

    AptnVertexInputPipelineState indirectVertexInput;
    AptnVertexInputPipelineState noninstancedVertexInput;
    {
        {
            constexpr StaticArray attributes = {
                // Per-vertex
                AptnVertexAttributeDescription
                {
                    .location = 0,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = 0
                },
                AptnVertexAttributeDescription
                {
                    .location = 1,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = sizeof(float) * 3
                },
                AptnVertexAttributeDescription
                {
                    .location = 2,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_2,
                    .offset = sizeof(float) * 6
                },
                AptnVertexAttributeDescription
                {
                    .location = 3,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = sizeof(float) * 8
                },

                // Per-instance
                AptnVertexAttributeDescription
                {
                    .location = 4,
                    .binding = 1,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = offsetof(InstanceData, pos)
                },
                AptnVertexAttributeDescription
                {
                    .location = 5,
                    .binding = 1,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = offsetof(InstanceData, rot)
                },
                AptnVertexAttributeDescription
                {
                    .location = 6,
                    .binding = 1,
                    .format = AptnVertexInputFormat::F32_1,
                    .offset = offsetof(InstanceData, scale)
                },
                AptnVertexAttributeDescription
                {
                    .location = 7,
                    .binding = 1,
                    .format = AptnVertexInputFormat::I32,
                    .offset = offsetof(InstanceData, texIndex)
                }
            };
            constexpr StaticArray bindings = {
                AptnVertexBindingDescription
                {
                    .binding = 0,
                    .stride = sizeof(vkglTF::Vertex),
                    .inputRate = AptnVertexInputRate::Vertex
                },
                AptnVertexBindingDescription
                {
                    .binding = 1,
                    .stride = sizeof(InstanceData),
                    .inputRate = AptnVertexInputRate::Instance
                }
            };
            AptnVertexInputPipelineStateCreationParams vertexInputParams
            {
                .primitiveTopology = AptnPrimitiveTopology::TriangleList,
                .attributes = attributes,
                .bindings = bindings
            };
            indirectVertexInput = Apparition::CreateVertexInputPipelineState(device, vertexInputParams);
        }

        {
            constexpr StaticArray attributes = {
                // Per-vertex
                AptnVertexAttributeDescription
                {
                    .location = 0,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = 0
                },
                AptnVertexAttributeDescription
                {
                    .location = 1,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = sizeof(float) * 3
                },
                AptnVertexAttributeDescription
                {
                    .location = 2,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_2,
                    .offset = sizeof(float) * 6
                },
                AptnVertexAttributeDescription
                {
                    .location = 3,
                    .binding = 0,
                    .format = AptnVertexInputFormat::F32_3,
                    .offset = sizeof(float) * 8
                }
            };
            constexpr StaticArray bindings = {
                AptnVertexBindingDescription
                {
                    .binding = 0,
                    .stride = sizeof(vkglTF::Vertex),
                    .inputRate = AptnVertexInputRate::Vertex
                }
            };
            AptnVertexInputPipelineStateCreationParams vertexInputParams =
            {
                .primitiveTopology = AptnPrimitiveTopology::TriangleList,
                .attributes = attributes,
                .bindings = bindings
            };
            noninstancedVertexInput = Apparition::CreateVertexInputPipelineState(device, vertexInputParams);
        }
    }

    AptnPrerasterShadersPipelineState plantsPreraster;
    AptnPrerasterShadersPipelineState groundPreraster;
    AptnPrerasterShadersPipelineState skyspherePreraster;
    {
        AptnPreRasterShadersPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .fillMode = AptnFillMode::Full,
            .cullingMode = AptnCullMode::None,
            .frontFace = AptnFrontFace::CounterClockwise,
            .lineWidth = 1.f
        };
        DynamicArray<u32> vertShader;
        {
            const MemoryBuffer vertShaderCode = LoadShader("Indirect/indirectdraw.vert.spv");
            vertShader.Resize(vertShaderCode.Size() / sizeof(u32));
            Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
        }
        params.vertexShader = { .code = vertShader, .entryName = "main" };
        plantsPreraster = Apparition::CreatePrerasterShadersPipelineState(device, params);

        {
            const MemoryBuffer vertShaderCode = LoadShader("Indirect/ground.vert.spv");
            vertShader.Resize(vertShaderCode.Size() / sizeof(u32));
            Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
        }
        params.cullingMode = AptnCullMode::Back;
        params.vertexShader = { .code = vertShader, .entryName = "main" };
        groundPreraster = Apparition::CreatePrerasterShadersPipelineState(device, params);

        {
            const MemoryBuffer vertShaderCode = LoadShader("Indirect/skysphere.vert.spv");
            vertShader.Resize(vertShaderCode.Size() / sizeof(u32));
            Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
        }
        params.cullingMode = AptnCullMode::Front;
        params.vertexShader = { .code = vertShader, .entryName = "main" };
        skyspherePreraster = Apparition::CreatePrerasterShadersPipelineState(device, params);
    }

    AptnFragmentShaderPipelineState plantsFragShader;
    AptnFragmentShaderPipelineState groundFragShader;
    AptnFragmentShaderPipelineState skysphereFragShader;
    {
        AptnFragmentShaderPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .depthTestEnabled = true,
            .depthWriteEnabled = true,
            .depthCompareOp = AptnCompareOperation::LessThanOrEqual,
        };

        DynamicArray<u32> fragShader;
        {
            const MemoryBuffer fragShaderCode = LoadShader("Indirect/indirectdraw.frag.spv");
            fragShader.Resize(fragShaderCode.Size() / sizeof(u32));
            Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
        }
        params.fragmentShader = { .code = fragShader, .entryName = "main" };
        plantsFragShader = Apparition::CreateFragmentShaderPipelineState(device, params);

        {
            const MemoryBuffer fragShaderCode = LoadShader("Indirect/ground.frag.spv");
            fragShader.Resize(fragShaderCode.Size() / sizeof(u32));
            Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
        }
        params.fragmentShader = { .code = fragShader, .entryName = "main" };
        groundFragShader = Apparition::CreateFragmentShaderPipelineState(device, params);

        {
            const MemoryBuffer fragShaderCode = LoadShader("Indirect/skysphere.frag.spv");
            fragShader.Resize(fragShaderCode.Size() / sizeof(u32));
            Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
        }
        params.fragmentShader = { .code = fragShader, .entryName = "main" };
        params.depthWriteEnabled = false;
        skysphereFragShader = Apparition::CreateFragmentShaderPipelineState(device, params);
    }

    constexpr StaticArray attachment = {
        AptnColorBlendAttachment
        {
        }
    };
    const StaticArray colorFormats = {
        Apparition::GetBackbufferFormat(device)
    };
    AptnFragmentOutputPipelineState fragOutputState;
    {
        AptnFragmentOutputPipelineStateCreationParams params
        {
            .pipelineDesc = pipelineDesc,
            .attachments = attachment,
            .colorAttachmentFormats = colorFormats,
            .depthAttachmentFormat = AptnImageFormat::DS_32f_8u,
            .stencilAttachmentFormat = AptnImageFormat::DS_32f_8u
        };
        fragOutputState = Apparition::CreateFragmentOutputPipelineState(device, params);
    }

    {

        AptnPipelineCreationParams pipelineParams
        {
            .pipelineDesc = pipelineDesc,
            .vertexInput = indirectVertexInput,
            .prerasterShaders = plantsPreraster,
            .fragmentShader = plantsFragShader,
            .fragmentOutput = fragOutputState
        };
        pipelines.plants = Apparition::CreatePipeline(device, pipelineParams);

        pipelineParams.vertexInput = noninstancedVertexInput;
        pipelineParams.prerasterShaders = groundPreraster;
        pipelineParams.fragmentShader = groundFragShader;
        pipelines.ground = Apparition::CreatePipeline(device, pipelineParams);

        pipelineParams.prerasterShaders = skyspherePreraster;
        pipelineParams.fragmentShader = skysphereFragShader;
        pipelines.skysphere = Apparition::CreatePipeline(device, pipelineParams);
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

void TickIndirectExample()
{
    AptnBackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
    Assert(preparationStatus != AptnBackbufferStatus::Unavailable);
    AptnImageView backbufferView = Apparition::GetBackBufferImageView(device);

    uniformData.projection = Math::ConstructPerspectiveMatrix(60.f, (float)window->width / (float)window->height, 0.1f, 512.f);
    const f32 radRotX = Math::DegreesToRadians(cameraRotationX);
    const f32 radRotY = Math::DegreesToRadians(cameraRotationY);
    constexpr bool orbit = false;
    uniformData.view = Math::ConstructViewMatrix(Vector4(.4f, 1.25f, 0.f), Quat(ROT_XYZ, radRotX, radRotY, 0.f), orbit);
    void* data = Apparition::MapBuffer(uniformBuffers[currentBuffer].buffer);
    Memcpy(data, &uniformData, sizeof(UniformData));
    Apparition::UnmapBuffer(uniformBuffers[currentBuffer].buffer);

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

    const StaticArray colorAttachments = { colorAttachment };
    AptnRenderSetupParams renderSetup = {};
    renderSetup.colorAttachments = { colorAttachments };
    renderSetup.depthAttachment = depthAttachment;
    renderSetup.renderWidth = backbufferWidth;
    renderSetup.renderHeight = backbufferHeight;
    Apparition::BeginRendering(commandBuffer, renderSetup);

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

    const StaticArray descriptorSetLayouts = { descriptorSetLayout };
    AptnPipelineDescription pipelineDesc = { .descriptorSets = descriptorSetLayouts };

    AptnBindDescriptorSetsDesc bindDSDesc
    {
        .pipelineDesc = pipelineDesc,
        .bindPoint = AptnBindPoint::Graphics,
        .descriptorSets = {&descriptorSets[currentBuffer], 1},
        .firstSet = 0
    };
    Apparition::BindDescriptorSets(commandBuffer, bindDSDesc);

    // Skysphere
    Apparition::BindGraphicsPipeline(commandBuffer, pipelines.skysphere);
    models.skysphere.draw(commandBuffer);
    // Ground
    Apparition::BindGraphicsPipeline(commandBuffer, pipelines.ground);
    models.ground.draw(commandBuffer);

    // Instanced multi draw rendering of the plants
    Apparition::BindGraphicsPipeline(commandBuffer, pipelines.plants);
    // Bind point 0: mesh buffer
    AptnBindVertexBufferDesc vertDesc
    {
        .vertexBuffer = models.plants.vertices.buffer,
        .binding = 0
    };
    Apparition::BindVertexBuffers(commandBuffer, vertDesc);
    // Bind point 1: instanced data buffer
    vertDesc = {
        .vertexBuffer = instanceBuffer,
        .binding = 1
    };
    Apparition::BindVertexBuffers(commandBuffer, vertDesc);

    AptnBindIndexBufferDesc indexDesc
    {
        .indexBuffer = models.plants.indices.buffer
    };
    Apparition::BindIndexBuffer(commandBuffer, indexDesc);

    for (u32 i = 0; i < indirectCommands.Size(); ++i)
    {
        Apparition::DrawIndexedIndirect(commandBuffer, indirectCommandsBuffer, i * sizeof(AptnDrawIndexedIndirectCommand), 1, sizeof(AptnDrawIndexedIndirectCommand));
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
    Apparition::WaitForIdle(graphicsQueue);
    Apparition::PresentBackbuffer(graphicsQueue);

    Apparition::WaitForIdle(graphicsQueue);

    currentBuffer = (currentBuffer + 1) % maxConcurrentFrames;
}

void DestroyIndirectExample()
{
    Apparition::WaitForIdle(graphicsQueue);

    Apparition::DestroyImageView(depthStencilView);
    Apparition::DestroyImage(depthStencilImage);

    Apparition::FreeCommandBuffer(drawCommandBuffers[0]);
    Apparition::FreeCommandBuffer(drawCommandBuffers[1]);
    Apparition::DestroyCommandPool(commandPool);

    Apparition::DestroyPipeline(pipelines.ground);
    Apparition::DestroyPipeline(pipelines.plants);
    Apparition::DestroyPipeline(pipelines.skysphere);

    Apparition::DestroyDescriptorPool(descriptorPool);
    Apparition::DestroyDescriptorSetLayout(descriptorSetLayout);

    textures.plants.destroy();
    textures.ground.destroy();

    models.plants.releaseResources();
    models.ground.releaseResources();
    models.skysphere.releaseResources();
}