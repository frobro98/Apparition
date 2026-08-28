/*
* Vulkan glTF model and texture loading class based on tinyglTF (https://github.com/syoyo/tinygltf)
*
* Copyright (C) 2018-2026 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

/*
 * Note that this isn't a complete glTF loader and not all features of the glTF 2.0 spec are supported
 * For details on how glTF 2.0 works, see the official spec at https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * If you are looking for a complete glTF implementation, check out https://github.com/SaschaWillems/Vulkan-glTF-PBR/
 */

#pragma once

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tinygltf/tiny_gltf.h"

#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/DescriptorSet.h"
#include "Apparition/Device.h"
#include "Apparition/Image.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/Pipeline.h"
#include "Apparition/Queue.h"

#include "ktx/include/ktx.h"
#include "ktx/include/ktxvulkan.h"

namespace vkglTF
{
	enum DescriptorBindingFlags {
		ImageBaseColor = 0x00000001,
		ImageNormalMap = 0x00000002
	};

	extern AptnDescriptorSetLayout descriptorSetLayoutImage;
	extern AptnDescriptorSetLayout descriptorSetLayoutUbo;
	//extern VkDescriptorSetLayout descriptorSetLayoutImage;
	//extern VkDescriptorSetLayout descriptorSetLayoutUbo;
	extern VkMemoryPropertyFlags memoryPropertyFlags;
	extern uint32_t descriptorBindingFlags;

	struct Node;

	/*
		glTF texture loading class
	*/
	struct Texture {
		//vks::VulkanDevice* device = nullptr;
		AptnDevice device;
		AptnCommandPool commandPool;
		//VkImage image;
		AptnImage image;
		AptnImageView view;
		AptnImageAccess access;
		//VkImageLayout imageLayout;
		//VkDeviceMemory deviceMemory;
		//VkImageView view;
		uint32_t width, height;
		uint32_t mipLevels;
		uint32_t layerCount;
		AptnImageDescriptorInfo descriptor;
		//VkDescriptorImageInfo descriptor;
		AptnSampler sampler;
		//VkSampler sampler;
		uint32_t index;
		void updateDescriptor();
		void destroy();
		void fromglTfImage(tinygltf::Image& gltfimage, std::string path, AptnDevice device, AptnQueue copyQueue);
	};

	/*
		glTF material class
	*/
	struct Material {
		AptnDevice device;
		//vks::VulkanDevice* device = nullptr;
		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alphaMode = ALPHAMODE_OPAQUE;
		float alphaCutoff = 1.0f;
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		vkglTF::Texture* baseColorTexture = nullptr;
		vkglTF::Texture* metallicRoughnessTexture = nullptr;
		vkglTF::Texture* normalTexture = nullptr;
		vkglTF::Texture* occlusionTexture = nullptr;
		vkglTF::Texture* emissiveTexture = nullptr;

		vkglTF::Texture* specularGlossinessTexture;
		vkglTF::Texture* diffuseTexture;

		AptnDescriptorSet descriptorSet;
		//VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		Material(AptnDevice device) : device(device) {};
		void createDescriptorSet(/*VkDescriptorPool descriptorPool*/AptnDescriptorPool descriptorPool, /*VkDescriptorSetLayout descriptorSetLayout*/AptnDescriptorSetLayout descriptorSetLayout, uint32_t descriptorBindingFlags);
	};

	/*
		glTF primitive
	*/
	struct Primitive {
		uint32_t firstIndex;
		uint32_t indexCount;
		uint32_t firstVertex;
		uint32_t vertexCount;
		Material& material;

		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
			glm::vec3 size;
			glm::vec3 center;
			float radius;
		} dimensions;

		void setDimensions(glm::vec3 min, glm::vec3 max);
		Primitive(uint32_t firstIndex, uint32_t indexCount, Material& material) : firstIndex(firstIndex), indexCount(indexCount), material(material) {};
	};

	/*
		glTF mesh
	*/
	struct Mesh {
		AptnDevice device;
		//vks::VulkanDevice* device;

		std::vector<Primitive*> primitives;
		std::string name;

		struct UniformBuffer {
			AptnBuffer buffer;
			/*VkBuffer buffer;
			VkDeviceMemory memory;*/
			AptnBufferDescriptorInfo descriptor;
			//VkDescriptorBufferInfo descriptor;
			AptnDescriptorSet descriptorSet;
			//VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			void* mapped;
		} uniformBuffer;

		struct UniformBlock {
			glm::mat4 matrix;
			glm::mat4 jointMatrix[64]{};
			float jointcount{ 0 };
		} uniformBlock;

		Mesh(/*vks::VulkanDevice* device*/AptnDevice device, glm::mat4 matrix);
		~Mesh();
	};

	/*
		glTF skin
	*/
	struct Skin {
		std::string name;
		Node* skeletonRoot = nullptr;
		std::vector<glm::mat4> inverseBindMatrices;
		std::vector<Node*> joints;
	};

	/*
		glTF node
	*/
	struct Node {
		Node* parent;
		uint32_t index;
		std::vector<Node*> children;
		glm::mat4 matrix;
		std::string name;
		Mesh* mesh;
		Skin* skin;
		int32_t skinIndex = -1;
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f };
		glm::quat rotation{};
		glm::mat4 localMatrix();
		glm::mat4 getMatrix();
		void update();
		~Node();
	};

	/*
		glTF animation channel
	*/
	struct AnimationChannel {
		enum PathType { TRANSLATION, ROTATION, SCALE };
		PathType path;
		Node* node;
		uint32_t samplerIndex;
	};

	/*
		glTF animation sampler
	*/
	struct AnimationSampler {
		enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
		InterpolationType interpolation;
		std::vector<float> inputs;
		std::vector<glm::vec4> outputsVec4;
	};

	/*
		glTF animation
	*/
	struct Animation {
		std::string name;
		std::vector<AnimationSampler> samplers;
		std::vector<AnimationChannel> channels;
		float start = std::numeric_limits<float>::max();
		float end = std::numeric_limits<float>::min();
	};

	/*
		glTF default vertex layout with easy Vulkan mapping functions
	*/
	enum class VertexComponent { Position, Normal, UV, Color, Tangent, Joint0, Weight0 };

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec4 color;
		glm::vec4 joint0;
		glm::vec4 weight0;
		glm::vec4 tangent;
		static AptnVertexBindingDescription vertexInputBindingDescription;
		//static VkVertexInputBindingDescription vertexInputBindingDescription;
		static DynamicArray<AptnVertexAttributeDescription> vertexInputAttributeDescriptions;
		//static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		//static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
		static AptnVertexBindingDescription inputBindingDescription(uint32_t binding);
		//static VkVertexInputBindingDescription inputBindingDescription(uint32_t binding);
		static AptnVertexAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
		//static VkVertexInputAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component);
		static DynamicArray<AptnVertexAttributeDescription> inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);
		//static std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components);
		/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
		//static VkPipelineVertexInputStateCreateInfo* getPipelineVertexInputState(const std::vector<VertexComponent> components);
	};

	enum FileLoadingFlags {
		None = 0x00000000,
		PreTransformVertices = 0x00000001,
		PreMultiplyVertexColors = 0x00000002,
		FlipY = 0x00000004,
		DontLoadImages = 0x00000008,
		FlipUV = 0x00000010
	};

	enum RenderFlags {
		BindImages = 0x00000001,
		RenderOpaqueNodes = 0x00000002,
		RenderAlphaMaskedNodes = 0x00000004,
		RenderAlphaBlendedNodes = 0x00000008
	};

	/*
		glTF model loading and rendering class
	*/
	class Model {
	private:
		vkglTF::Texture* getTexture(uint32_t index);
		vkglTF::Texture emptyTexture;
		void createEmptyTexture(/*VkQueue transferQueue*/AptnQueue transferQueue);
	public:
		AptnDevice device;
		//vks::VulkanDevice* device;
		AptnDescriptorPool descriptorPool;
		//VkDescriptorPool descriptorPool;

		struct Vertices {
			int count;
			AptnBuffer buffer;
			//VkBuffer buffer;
			//VkDeviceMemory memory;
		} vertices;
		struct Indices {
			int count;
			AptnBuffer buffer;
			//VkBuffer buffer;
			//VkDeviceMemory memory;
		} indices;

		std::vector<Node*> nodes;
		std::vector<Node*> linearNodes;

		std::vector<Skin*> skins;

		std::vector<Texture> textures;
		std::vector<Material> materials;
		std::vector<Animation> animations;

		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
			glm::vec3 size;
			glm::vec3 center;
			float radius;
		} dimensions;

		bool metallicRoughnessWorkflow = true;
		bool buffersBound = false;
		bool resourcesReleased = false;
		std::string path;

		Model() {};
		~Model();
		void releaseResources();
		void loadNode(vkglTF::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale);
		void loadSkins(tinygltf::Model& gltfModel);
		void loadImages(tinygltf::Model& gltfModel, /*vks::VulkanDevice* device*/AptnDevice device, /*VkQueue transferQueue*/AptnQueue transferQueue);
		void loadMaterials(tinygltf::Model& gltfModel);
		void loadAnimations(tinygltf::Model& gltfModel);
		void loadFromFile(std::string filename, /*vks::VulkanDevice* device*/AptnDevice device, /*VkQueue transferQueue*/AptnQueue transferQueue, uint32_t fileLoadingFlags = vkglTF::FileLoadingFlags::None, float scale = 1.0f);
		void bindBuffers(/*VkCommandBuffer commandBuffer*/AptnCommandBuffer commandBuffer);
		void drawNode(Node* node, /*VkCommandBuffer commandBuffer*/AptnCommandBuffer commandBuffer, uint32_t renderFlags = 0, /*VkPipelineLayout pipelineLayout*/const AptnPipelineDescription& pipelineDesc = {}, uint32_t bindImageSet = 1);
		void draw(/*VkCommandBuffer commandBuffer*/AptnCommandBuffer commandBuffer, uint32_t renderFlags = 0, /*VkPipelineLayout pipelineLayout*/const AptnPipelineDescription& pipelineDesc = {}, uint32_t bindImageSet = 1);
		void getNodeDimensions(Node* node, glm::vec3& min, glm::vec3& max);
		void getSceneDimensions();
		void updateAnimation(uint32_t index, float time);
		Node* findNode(Node* parent, uint32_t index);
		Node* nodeFromIndex(uint32_t index);
		Node* nodeFromName(const std::string name);
		void prepareNodeDescriptor(vkglTF::Node* node, /*VkDescriptorSetLayout descriptorSetLayout*/AptnDescriptorSetLayout descriptorSetLayout);
	};
}