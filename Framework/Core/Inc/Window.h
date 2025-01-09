#pragma once

namespace VEngine::Core
{
	class Window
	{
	public:
		void Initialize(HINSTANCE instance, const std::wstring& appName, uint32_t width, uint32_t height);
		void Terminate();

		void ProcessMessage();
		HWND GetWindowHandle() const;
		bool IsActive() const;

	private:
		HINSTANCE mInstance = nullptr;
		HWND mWindow = nullptr;
		std::wstring mAppName;
		bool mActive = false;
	};
}