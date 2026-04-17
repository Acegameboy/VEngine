#pragma once

namespace VEngine
{
	enum class ComponentId
	{
		Invalid,              //default value, not valid Id
		Transfrom,            // transform component for location data
		Camera,				  //contains the camera for viewing
		FPSCamera,            // move the Camera Component with FPS controls
		Count                 //last value, can be used to chain custom components
	};
}

#define SET_TYPE_ID(id)\
	static uint32_t StaticGetTypedId() {return static_cast<uint32_t>(id);}\
	uint32_t GetTypedId() const override { return StaticGeetTypedId(); }