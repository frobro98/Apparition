// Copyright 2020, Nathan Blane

#pragma once

//#include "ECS/System.hpp"
//
//
//struct RenderView;
//
//// Culls and hands of meshes to be rendered
//class ViewCullingSystem : public Musa::System
//{
//public:
//
//	virtual void Initialize() override;
//	virtual void Update() override;
//
//private:
//	void CollectAllViewsIntoWorld(DynamicArray<RenderView*>& views) const;
//	void CullPrimitivesPerView(DynamicArray<RenderView*>& views) const;
//	void CullLightsPerView(DynamicArray<RenderView*>& views) const;
//
//private:
//	Musa::Query* viewsIntoScene = nullptr;
//	Musa::Query* renderPrimsToCull = nullptr;
//};
