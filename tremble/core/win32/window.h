#pragma once

namespace tremble
{
	class GameManager;

	/**
	* A window that can be used for rendering
	* @author Anton Gavrilov
	*/
	class Window
	{
	public:
		/**
		* Window creation and initialization function. Creates a window with given parameters
		* @param[in] name A name that you want to use for this window
		* @param[in] width Width, that you wish this window to have
		* @param[in] height Height, that you wish this window to have
		* @param[in] game_manager Game manager object, that owns this window
		*/
		Window(const std::string& name, unsigned int width, unsigned int height, GameManager* game_manager);
		~Window(); //!< Window destructor

		const HWND& GetWindowHandle() const { return hwindow_; }; //!< Get the window handle
		const HINSTANCE& GetWindowInstance() const { return hinstance_; }; //!< Get the window instance
		const unsigned int& GetWidth() const { return width_; }; //!< Get the width of the window
		const unsigned int& GetHeight() const { return height_; }; //!< Get the height of the window
		void GetPosition(int& x, int& y) const; //!< Get the position of the window on screen
		const float& GetAspectRatio() const { return (float)width_ / (float)height_; }; //!< Get the aspect ratio of the window
		void ProcessMessages(); //!< Process windows messages, sent to the window

		void SetWindowInputListener(std::function<void(MSG message)> listener) { process_input_messages_ = listener; }; //!< Set input listener function of this window

		static LRESULT CALLBACK WindowProc(HWND hwindow, UINT message, WPARAM wparam, LPARAM lparam); //!<Windows callback function. Only used for WM_CREATE and WM_CLOSE

		bool HasFocus() { return has_focus_; };
	private:
		GameManager* game_manager_; //!< pointer to owner game manager

		HWND hwindow_; //!< Handle to the window
		HINSTANCE hinstance_; //!< Windows process handle
		unsigned int width_; //!< Width of the window
		unsigned int height_; //!< Height of the window
		bool has_focus_;
		int style_;

		std::string window_name_; //!<name of the window (not the title)

		std::string window_title_; //!<title of the window

		std::function<void(MSG message)> process_input_messages_; //!< process input messages. Set in the input manager object construction
		std::function<void(void)> quit_function_; //!< process quit messages. Set on window's construction
	};
}