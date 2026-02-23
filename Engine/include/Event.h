#pragma once

/** 
 * @brief the event struct for the event system
 */
struct Event
{
	virtual ~Event() = default;
	bool m_handled = false;
};

/**
 * @brief the event struct for the key event
 */
struct KeyEvent : public Event
{
	/**
	 * @brief the constructor for the key event
	 * \param key
	 * \param action
	 */
	KeyEvent(int key, int action) : m_key(key), m_action(action) {}
	int m_key;
	int m_action;
};

/**
 * @brief the event struct for the mouse event
 */
struct MouseEvent : public Event
{
	/**
	 * @brief the constructor for the mouse event
	 * \param x
	 * \param y
	 */
	MouseEvent(double x, double y) : xPos(x), yPos(y) {}
	double xPos;
	double yPos;
};

/**
 * @brief the event struct for the mouse button event
 */
struct MouseButtonEvent : public Event
{
	/**
	 * @brief the constructor for the mouse button event
	 * \param button
	 * \param action
	 */
	MouseButtonEvent(int button, int action) : m_button(button), m_action(action) {}
	int m_button;
	int m_action;
};

/**
 * @brief the event struct for the scroll event
 */
struct ScrollEvent : public Event
{
	/**
	 * @brief the constructor for the scroll event
	 * \param x
	 * \param y
	 */
	ScrollEvent(double x, double y) : xOffset(x), yOffset(y) {}
	double xOffset;
	double yOffset;
};

/**
 * @brief the event struct for the window resize event
 */
struct WindowResizeEvent : public Event
{
	/**
	 * @brief the constructor for the window resize event
	 * \param width
	 * \param height
	 */
	WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}
	int m_width;
	int m_height;
};

/**
 * @brief the event struct for the window close event
 */
struct WindowCloseEvent : public Event
{
	/**
	 * @brief the constructor for the window close event
	 */
	WindowCloseEvent() = default;
};

/**
 * @brief the event struct for the framebuffer resize event
 */
struct FramebufferResizeEvent : public Event
{
	/**
	 * @brief the constructor for the framebuffer resize event
	 * \param width
	 * \param height
	 */
	FramebufferResizeEvent(int width, int height) : m_width(width), m_height(height) {}
	int m_width;
	int m_height;
};