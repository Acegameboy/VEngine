#pragma once


namespace VEngine
{
	class App final
	{
	public:
		void Run();
		void Quit();

	private:
		bool mRunning = false;
	};
}
