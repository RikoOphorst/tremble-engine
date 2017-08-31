#pragma once

#include "../math/math.h"
#include "design/config_design.h"
#include "config.h"

namespace tremble
{
	class ConfigManager : public QObject
	{
		Q_OBJECT
	public:
		ConfigManager();
		~ConfigManager();

		const std::vector<IDXGIAdapter1*>& GetAdapters() { return available_adapters_; }
		const std::vector<Resolution>& GetWindowResolutions() { return available_window_resolutions_; }
		const std::vector<Resolution>& GetRenderResolutions() { return available_render_resolutions_; }
		const std::vector<std::string>& GetScenes() { return available_scenes_; }

		const Config& GetConfig();

	public slots:
		void Launch();
		void Shutdown();
		void FullScreenStateChange(int state);

	private:
		void FindAvailableAdapters();
		void FindAvailableWindowResolutions();
		void FindAvailableRenderResolutions();
		void FindAvailableScenes();

		void FillGPUBox();
		void FillWindowResolutionBox();
		void FillRenderResolutionBox();
		void FillSceneBox();

	private:
		QMainWindow* main_window_;

		Ui::SetupDialog* setup_dialog_;
		bool running_;

		std::vector<IDXGIAdapter1*> available_adapters_;
		std::vector<Resolution> available_window_resolutions_;
		std::vector<Resolution> available_render_resolutions_;
		std::vector<std::string> available_scenes_;

		Config config_;
	};
}