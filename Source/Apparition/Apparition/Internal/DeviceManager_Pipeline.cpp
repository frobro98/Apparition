
#include "DeviceManager.h"

#include "Apparition/Pipeline.h"

#include "Apparition/Internal/ApparitionInternals.h"
#include "Apparition/Internal/Conversions.h"
#include "Apparition/Internal/ImageFormatConversion.h"

namespace
{
// Vk structs
DynamicArray<VkVertexInputAttributeDescription> ApparitionAttributesToVk(const DynamicArray<VertexAttributeDescription>& attributes)
{
    DynamicArray<VkVertexInputAttributeDescription> vkAttributes(attributes.Size());
    for (u32 i = 0; i < attributes.Size(); ++i)
    {
        const VertexAttributeDescription& attribute = attributes[i];
        vkAttributes[i] = VkVertexInputAttributeDescription{
            .location = attribute.location,
            .binding = attribute.binding,
            .format = ApparitionInputFormatToVk(attribute.format),
            .offset = attribute.offset
        };
    }

    return vkAttributes;
}

DynamicArray<VkVertexInputBindingDescription> ApparitionBindingsToVk(const DynamicArray<VertexBindingDescription>& bindings)
{
    DynamicArray<VkVertexInputBindingDescription> vkBindings(bindings.Size());
    for (u32 i = 0; i < bindings.Size(); ++i)
    {
        const VertexBindingDescription& binding = bindings[i];
        vkBindings[i] = VkVertexInputBindingDescription{
            .binding = binding.binding,
            .stride = binding.stride,
            .inputRate = ApparitionInputRateToVk(binding.inputRate)
        };
    }

    return vkBindings;
}

DynamicArray<VkFormat> ApparitionFormatsToVk(const DynamicArray<ImageFormat::Type>& formats)
{
    DynamicArray<VkFormat> vkFormats(formats.Size());
    for (u32 i = 0; i < formats.Size(); ++i)
    {
        vkFormats[i] = ApparitionFormatToVk(formats[i]);
    }

    return vkFormats;
}

DynamicArray<VkPipelineColorBlendAttachmentState> ApparitionBlendAttachmentsToVk(const DynamicArray<ColorBlendAttachment>& blendAttachments)
{
    DynamicArray<VkPipelineColorBlendAttachmentState> vkAttachments(blendAttachments.Size());
    for (u32 i = 0; i < blendAttachments.Size(); ++i)
    {
        const ColorBlendAttachment& blendAttachment = blendAttachments[i];
        vkAttachments[i] = VkPipelineColorBlendAttachmentState
        {
            .blendEnable = blendAttachment.colorBlendOperation != BlendOperation::None ||
                           blendAttachment.alphaBlendOperation != BlendOperation::None,
            .srcColorBlendFactor = ApparitionBlendFactorToVk(blendAttachment.srcColorFactor),
            .dstColorBlendFactor = ApparitionBlendFactorToVk(blendAttachment.dstColorFactor),
            .colorBlendOp = ApparitionBlendOpToVk(blendAttachment.colorBlendOperation),
            .srcAlphaBlendFactor = ApparitionBlendFactorToVk(blendAttachment.srcAlphaFactor),
            .dstAlphaBlendFactor = ApparitionBlendFactorToVk(blendAttachment.dstAlphaFactor),
            .alphaBlendOp = ApparitionBlendOpToVk(blendAttachment.alphaBlendOperation),
            .colorWriteMask = ApparitionColorWriteMaskToVk(blendAttachment.colorMask)
        };
    }

    return vkAttachments;
}
}

///////////////////////////////////////////////

VertexInputPipelineState DeviceManager::CreateVertexInputPipelineState(Device device, const VertexInputPipelineStateCreationParams& params)
{
    // GraphicsPipelineLibrary setup
    const VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
        .flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT
    };

    const DynamicArray<VkVertexInputBindingDescription> vkBindings = ApparitionBindingsToVk(params.bindings);
    Assert(vkBindings.Size() == params.bindings.Size());
    const DynamicArray<VkVertexInputAttributeDescription> vkAttributes = ApparitionAttributesToVk(params.attributes);
    Assert(vkAttributes.Size() == params.attributes.Size());

    const VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = params.bindings.Size(),
        .pVertexBindingDescriptions = vkBindings.GetData(),
        .vertexAttributeDescriptionCount = params.attributes.Size(),
        .pVertexAttributeDescriptions = vkAttributes.GetData()
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssembly
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = ApparitionTopologyToVk(params.primitiveTopology),
        .primitiveRestartEnable = VK_FALSE
    };

    const VkGraphicsPipelineCreateInfo pipelineStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &libraryInfo,
        .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly
    };
    
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    VkPipeline vertexInputState = VK_NULL_HANDLE;
    VkResult result = vkCreateGraphicsPipelines(deviceInternal.device, VK_NULL_HANDLE, 1, &pipelineStateInfo, nullptr, &vertexInputState);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        const VertexInputPipelineStateInternal vertexInputInternal
        {
            .state = vertexInputState
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.vertexInputResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetVertexInputPipelineStateInternalFromIndex(deviceInternal, handleIndex) = vertexInputInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.vertexInputResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return VertexInputPipelineState{ handleData };
        }
    }
    return { InvalidHandle };
}

PrerasterShadersPipelineState DeviceManager::CreatePrerasterShadersPipelineState(Device device, const PreRasterShadersPipelineStateCreationParams& params)
{
    // Dynamic State
    DynamicArray<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.Size());
    dynamicState.pDynamicStates = dynamicStates.GetData();

    // Viewport State
    VkPipelineViewportStateCreateInfo viewportState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = ApparitionFillToVk(params.fillMode),
        .cullMode = ApparitionCullToVk(params.cullingMode),
        .frontFace = ApparitionFrontFaceToVk(params.frontFace),
        .lineWidth = params.lineWidth
    };

    VkShaderModuleCreateInfo shaderInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = params.vertexShader.code.SizeInBytes(),
        .pCode = params.vertexShader.code.GetData()
    };

    // Shader Module
    VkPipelineShaderStageCreateInfo vertStageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = &shaderInfo,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .pName = params.vertexShader.entryName
    };

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    // Create VkPipelineLayout for this operation
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        const VkResult result = vkCreatePipelineLayout(deviceInternal.device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        CHECK_VK(result);
    }

    // GraphicsPipelineLibrary setup
    const VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
        .flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT
    };

    // Pipeline State creation
    const VkGraphicsPipelineCreateInfo pipelineStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &libraryInfo,
        .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
        .stageCount = 1,
        .pStages = &vertStageInfo,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout
    };

    VkPipeline prerasterShadersState = VK_NULL_HANDLE;
    const VkResult result = vkCreateGraphicsPipelines(deviceInternal.device, VK_NULL_HANDLE, 1, &pipelineStateInfo, nullptr, &prerasterShadersState);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

        const PrerasterShadersPipelineStateInternal prerasterShadersInternal
        {
            .state = prerasterShadersState
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.prerasterShadersResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetPrerasterShadersPipelineStateInternalFromIndex(deviceInternal, handleIndex) = prerasterShadersInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.prerasterShadersResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return PrerasterShadersPipelineState{ handleData };
        }
    }

    // TODO - make this one call instead of multiple calls
    vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

    return { InvalidHandle };
}

FragmentShaderPipelineState DeviceManager::CreateFragmentShaderPipelineState(Device device, const FragmentShaderPipelineStateCreationParams& params)
{
    UNUSED(params);
    
    // Depth/Stencil
    VkPipelineDepthStencilStateCreateInfo depthStencilState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = params.depthTestEnabled,
        .depthWriteEnable = params.depthWriteEnabled,
        .depthCompareOp = ApparitionCompareOpToVk(params.depthCompareOp),
    };

    // TODO - Support multisampling
    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkShaderModuleCreateInfo shaderInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = params.fragmentShader.code.SizeInBytes(),
        .pCode = params.fragmentShader.code.GetData(),
    };

    VkPipelineShaderStageCreateInfo fragStageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = &shaderInfo,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pName = params.fragmentShader.entryName
    };

    // GraphicsPipelineLibrary setup
    const VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
        .flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT
    };

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    // Create VkPipelineLayout for this operation
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        const VkResult result = vkCreatePipelineLayout(deviceInternal.device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        CHECK_VK(result);
    }

    // Pipeline State creation
    const VkGraphicsPipelineCreateInfo pipelineStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &libraryInfo,
        .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
        .stageCount = 1,
        .pStages = &fragStageInfo,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencilState,
        .layout = pipelineLayout
    };

    VkPipeline fragmentShaderState = VK_NULL_HANDLE;
    const VkResult result = vkCreateGraphicsPipelines(deviceInternal.device, VK_NULL_HANDLE, 1, &pipelineStateInfo, nullptr, &fragmentShaderState);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

        const FragmentShaderPipelineStateInternal fragmentShadersInternal
        {
            .state = fragmentShaderState
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.fragmentShaderResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetFragmentShaderPipelineStateInternalFromIndex(deviceInternal, handleIndex) = fragmentShadersInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.fragmentShaderResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return FragmentShaderPipelineState{ handleData };
        }
    }
    vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

    return { InvalidHandle };
}

FragmentOutputPipelineState DeviceManager::CreateFragmentOutputPipelineState(Device device, const FragmentOutputPipelineStateCreationParams& params)
{
    // Color Blending
    DynamicArray<VkPipelineColorBlendAttachmentState> vkBlendAttachments = ApparitionBlendAttachmentsToVk(params.attachments);
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = vkBlendAttachments.Size();
    colorBlending.pAttachments = vkBlendAttachments.GetData();
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    // TODO - Support multisampling
    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional


    // Dynamic Rendering Setup: Gets passed to shader stages
    DynamicArray<VkFormat> vkFormats = ApparitionFormatsToVk(params.colorAttachmentFormats);
    VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .viewMask = 0,
        .colorAttachmentCount = vkFormats.Size(),
        .pColorAttachmentFormats = vkFormats.GetData(),
        .depthAttachmentFormat = ApparitionFormatToVk(params.depthAttachmentFormat),
        .stencilAttachmentFormat = ApparitionFormatToVk(params.stencilAttachmentFormat)
    };

    // GraphicsPipelineLibrary setup
    const VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
        .pNext = &pipelineRenderingCreateInfo,
        .flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT
    };

    // Pipeline State creation
    const VkGraphicsPipelineCreateInfo pipelineStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &libraryInfo,
        .flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
    };

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    VkPipeline fragmentOutputState = VK_NULL_HANDLE;
    const VkResult result = vkCreateGraphicsPipelines(deviceInternal.device, VK_NULL_HANDLE, 1, &pipelineStateInfo, nullptr, &fragmentOutputState);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        const FragmentOutputPipelineStateInternal fragmentOutputInternal
        {
            .state = fragmentOutputState
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.fragmentOutputResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetFragmentOutputPipelineStateInternalFromIndex(deviceInternal, handleIndex) = fragmentOutputInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.fragmentOutputResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return FragmentOutputPipelineState{ handleData };
        }
    }
    return { InvalidHandle };
}

void DeviceManager::DestroyVertexInputPipelineState(VertexInputPipelineState /*state*/)
{

}

void DeviceManager::DestroyPrerasterShadersPipelineState(PrerasterShadersPipelineState /*state*/)
{

}

void DeviceManager::DestroyFragmentShaderPipelineState(FragmentShaderPipelineState /*state*/)
{

}

void DeviceManager::DestroyFragmentOutputPipelineState(FragmentOutputPipelineState /*state*/)
{

}

Pipeline DeviceManager::CreatePipeline(Device device, const PipelineCreationParams& params)
{
    UNUSED(params);

    // Find VkPipeline handles for API handles
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    VertexInputPipelineStateInternal& vertexInputInternal = GetVertexInputPipelineStateInternal(params.vertexInput);
    PrerasterShadersPipelineStateInternal& prerasterShadersInternal = GetPrerasterShadersPipelineStateInternal(params.prerasterShaders);
    FragmentShaderPipelineStateInternal& fragmentShaderInternal = GetFragmentShaderPipelineStateInternal(params.fragmentShader);
    FragmentOutputPipelineStateInternal& fragmentOutputInternal = GetFragmentOutputPipelineStateInternal(params.fragmentOutput);
    VkPipeline vertexInput = vertexInputInternal.state;
    VkPipeline prerasterShaders = prerasterShadersInternal.state;
    VkPipeline fragmentShader = fragmentShaderInternal.state;
    VkPipeline fragmentOutput = fragmentOutputInternal.state;

    constexpr u32 numPipelineStates = 4;
    const StaticArray<VkPipeline, numPipelineStates> libraries
    {
        vertexInput,
        prerasterShaders,
        fragmentShader,
        fragmentOutput
    };

    // Pipeline Library Info
    const VkPipelineLibraryCreateInfoKHR pipelineLibraryInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
        .libraryCount = libraries.Size(),
        .pLibraries = libraries.internalData
    };

    // Create VkPipelineLayout
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        const VkResult result = vkCreatePipelineLayout(deviceInternal.device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        CHECK_VK(result);
    }

    // Graphics Pipeline
    const VkGraphicsPipelineCreateInfo pipelineInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipelineLibraryInfo,
        .layout = pipelineLayout
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    const VkResult result = vkCreateGraphicsPipelines(deviceInternal.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

        const PipelineInternal pipelineInternal
        {
            .pipeline = pipeline
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.pipelineResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetPipelineInternalFromIndex(deviceInternal, handleIndex) = pipelineInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.pipelineResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return Pipeline{ handleData };
        }
    }
    vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);

    return { InvalidHandle };
}

void DeviceManager::DestroyPipeline(Pipeline pipeline)
{
    UNUSED(pipeline);
}