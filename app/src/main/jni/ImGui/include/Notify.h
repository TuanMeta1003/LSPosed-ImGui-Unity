#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once

#include <vector>			// Vector for storing notifications list
#include <string>
#include <chrono>			// For the notifications timed dissmiss
#include <functional>		// For storing the code, which executest on the button click in the notification

#include "imgui.h"
#include "imgui_internal.h"

#include "IconsFontAwesome6.h"

#define NOTIFY_MAX_MSG_LENGTH				4096		// Max message content length
#define NOTIFY_PADDING_X					20.f		// Bottom-left X padding
#define NOTIFY_PADDING_Y					20.f		// Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y			10.f		// Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME				150			// Fade in and out duration
#define NOTIFY_DEFAULT_DISMISS				3000		// Auto dismiss after X ms (default, applied only of no data provided in constructors)
#define NOTIFY_OPACITY						0.8f		// 0-1 Toast opacity
#define NOTIFY_USE_SEPARATOR 				false 		// If true, a separator will be rendered between the title and the content
#define NOTIFY_USE_DISMISS_BUTTON			true		// If true, a dismiss button will be rendered in the top right corner of the toast
#define NOTIFY_RENDER_LIMIT					0			// Max number of toasts rendered at the same time. Set to 0 for unlimited

static const ImGuiWindowFlags NOTIFY_DEFAULT_TOAST_FLAGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

#define NOTIFY_NULL_OR_EMPTY(str)		(!str || !strlen(str))
#define NOTIFY_FORMAT(fn, format, ...)	if (format) {va_list args; va_start(args, format); fn(format, args, ##__VA_ARGS__); va_end(args);}

enum class ImGuiToastType : uint8_t
{
	None,
	Success,
	Warning,
	Error,
	Info,
	COUNT
};

enum class ImGuiToastPhase : uint8_t
{
	FadeIn,
	Wait,
	FadeOut,
	Expired,
	COUNT
};

enum class ImGuiToastPos : uint8_t
{
	TopLeft,
	TopCenter,
	TopRight,
	BottomLeft,
	BottomCenter,
	BottomRight,
	Center,
	COUNT
};

/**
 * @brief A class for creating toast notifications with ImGui.
 */
class ImGuiToast
{
private:
	ImGuiWindowFlags							flags = NOTIFY_DEFAULT_TOAST_FLAGS;

	ImGuiToastType								type = ImGuiToastType::None;
	char										title[NOTIFY_MAX_MSG_LENGTH];
	char										content[NOTIFY_MAX_MSG_LENGTH];

	int											dismissTime = NOTIFY_DEFAULT_DISMISS;
	std::chrono::system_clock::time_point		creationTime = std::chrono::system_clock::now();

	std::function<void()>						onButtonPress = nullptr; // A lambda variable, which will be executed when button in notification is pressed
	char 										buttonLabel[NOTIFY_MAX_MSG_LENGTH];

private:
	// Setters

	inline void setTitle(const char* format, va_list args)
	{
		vsnprintf(this->title, sizeof(this->title), format, args);
	}

	inline void setContent(const char* format, va_list args)
	{
		vsnprintf(this->content, sizeof(this->content), format, args);
	}

	inline void setButtonLabel(const char* format, va_list args)
	{
		vsnprintf(this->buttonLabel, sizeof(this->buttonLabel), format, args);
	}

public:

	/**
	 * @brief Set the title of the toast notification.
	 * 
	 * @param format The format string for the title.
	 * @param ... The arguments for the format string.
	 */
	inline void setTitle(const char* format, ...)
	{
		NOTIFY_FORMAT(this->setTitle, format);
	}

	/**
	 * @brief Set the content of the toast notification.
	 * 
	 * @param format The format string for the content.
	 * @param ... The arguments for the format string.
	 */
	inline void setContent(const char* format, ...)
	{
		NOTIFY_FORMAT(this->setContent, format);
	}

	/**
	 * @brief Set the type of the toast notification.
	 * 
	 * @param type The type of the toast notification.
	 */
	inline void setType(const ImGuiToastType& type)
	{
		IM_ASSERT(type < ImGuiToastType::COUNT);
		this->type = type;
	};

	/**
	 * @brief Set the ImGui window flags for the notification.
	 * 
	 * @param flags ImGui window flags to set.
	*/
	inline void setWindowFlags(const ImGuiWindowFlags& flags)
	{
		this->flags = flags;
	}

	/**
	 * @brief Set the function to run on the button click in the notification.
	 * 
	 * @param onButtonPress std::fuction or lambda expression, which contains the code for execution.
	*/
	inline void setOnButtonPress(const std::function<void()>& onButtonPress)
	{
		this->onButtonPress = onButtonPress;
	}

	/**
	 * @brief Set the label for the button in the notification.
	 * 
	 * @param format The format string for the label.
	 * @param ... The arguments for the format string.
	*/
	inline void setButtonLabel(const char* format, ...)
	{
		NOTIFY_FORMAT(this->setButtonLabel, format);
	}

public:
	// Getters

	/**
	 * @brief Get the title of the toast notification.
	 * 
	 * @return const char* The title of the toast notification.
	 */
	inline const char* getTitle()
	{
		return this->title;
	};

	/**
	 * @brief Get the default title of the toast notification based on its type.
	 * 
	 * @return const char* The default title of the toast notification.
	 */
	inline const char* getDefaultTitle()
	{
		if (!strlen(this->title))
		{
			switch (this->type)
			{
			case ImGuiToastType::None:
				return nullptr;
			case ImGuiToastType::Success:
				return "Success";
			case ImGuiToastType::Warning:
				return "Warning";
			case ImGuiToastType::Error:
				return "Error";
			case ImGuiToastType::Info:
				return "Info";
			default:
				return nullptr;
			}
		}

		return this->title;
	};

	/**
	 * @brief Get the type of the toast notification.
	 * 
	 * @return ImGuiToastType The type of the toast notification.
	 */
	inline ImGuiToastType getType()
	{
		return this->type;
	};

	/**
	 * @brief Get the color of the toast notification based on its type.
	 * 
	 * @return ImVec4 The color of the toast notification.
	 */
	inline ImVec4 getColor()
	{
		switch (this->type)
		{
		case ImGuiToastType::None:
			return {0, 0, 0, 255}; // Black (complement of white)
		case ImGuiToastType::Success:
			return {0, 0, 0, 255}; // Magenta (complement of green)
		case ImGuiToastType::Warning:
			return {0, 0, 0, 255}; // Blue (complement of yellow)
		case ImGuiToastType::Error:
			return {0, 0, 0, 255}; // Cyan (complement of red)
		case ImGuiToastType::Info:
			return {0, 0, 0, 255}; // Orange (complement of blue)
		default:
			return {0, 0, 0, 255}; // Black
		}

	}

	/**
	 * @brief Get the icon of the toast notification based on its type.
	 * 
	 * @return const char* The icon of the toast notification.
	 */
	inline const char* getIcon() 
	{
		switch (this->type)
		{
		case ImGuiToastType::None:
			return nullptr;
		case ImGuiToastType::Success:
			return ICON_FA_CIRCLE_CHECK; // Font Awesome 6
		case ImGuiToastType::Warning:
			return ICON_FA_TRIANGLE_EXCLAMATION; // Font Awesome 6
		case ImGuiToastType::Error:
			return ICON_FA_CIRCLE_EXCLAMATION; // Font Awesome 6
		case ImGuiToastType::Info:
			return ICON_FA_CIRCLE_INFO; // Font Awesome 6
		default:
			return nullptr;
		}
	}

	/**
	 * @brief Get the content of the toast notification.
	 * 
	 * @return char* The content of the toast notification.
	 */
	inline char* getContent() 
	{
		return this->content;
	};

	/**
	 * @brief Get the elapsed time in milliseconds since the creation of the object.
	 * 
	 * @return int64_t The elapsed time in milliseconds.
	 * @throws An exception with the message "Unsupported platform" if the platform is not supported.
	 */
	inline std::chrono::nanoseconds getElapsedTime() 
	{
		return std::chrono::system_clock::now() - this->creationTime;
	}

	/**
	 * @brief Get the current phase of the toast notification based on the elapsed time since its creation.
	 * 
	 * @return ImGuiToastPhase The current phase of the toast notification.
	 *		 - ImGuiToastPhase::FadeIn: The notification is fading in.
	 *		 - ImGuiToastPhase::Wait: The notification is waiting to be dismissed.
	 *		 - ImGuiToastPhase::FadeOut: The notification is fading out.
	 *		 - ImGuiToastPhase::Expired: The notification has expired and should be removed.
	 */
	inline ImGuiToastPhase getPhase()
	{
		const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(getElapsedTime()).count();

		if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismissTime + NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiToastPhase::Expired;
		} else 
		if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismissTime)
		{
			return ImGuiToastPhase::FadeOut;
		} else 
		if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiToastPhase::Wait;
		} else
		{
			return ImGuiToastPhase::FadeIn;
		}
	}

	/**
	 * Returns the percentage of fade for the notification.
	 * @return The percentage of fade for the notification.
	 */
	inline float getFadePercent()
	{
		const ImGuiToastPhase phase = getPhase();
		const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(getElapsedTime()).count();

		if (phase == ImGuiToastPhase::FadeIn)
		{
			return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
		} else 
		if (phase == ImGuiToastPhase::FadeOut)
		{
			return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismissTime) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
		}

		return 1.f * NOTIFY_OPACITY;
	}

	/**
	 * @return ImGui window flags for the notification.
	*/
	inline ImGuiWindowFlags getWindowFlags()
	{
		return this->flags;
	}

	/**
	 * @return The function, which is executed on the button click in the notification.
	*/
	inline std::function<void()> getOnButtonPress()
	{
		return this->onButtonPress;
	}

	/**
	 * @return The label on the button in notification.
	*/
	inline const char* getButtonLabel()
	{
		return this->buttonLabel;
	}

public:
	// Constructors

	/**
	 * @brief Creates a new ImGuiToast object with the specified type and dismiss time.
	 * 
	 * @param type The type of the toast.
	 * @param dismissTime The time in milliseconds after which the toast should be dismissed. Default is NOTIFY_DEFAULT_DISMISS.
	 */
	ImGuiToast(ImGuiToastType type, int dismissTime = NOTIFY_DEFAULT_DISMISS)
	{
		IM_ASSERT(type < ImGuiToastType::COUNT);

		this->type = type;
		this->dismissTime = dismissTime;

		this->creationTime = std::chrono::system_clock::now();

		memset(this->title, 0, sizeof(this->title));
		memset(this->content, 0, sizeof(this->content));
	}

	/**
	 * @brief Constructor for creating an ImGuiToast object with a specified type and message format.
	 * 
	 * @param type The type of the toast message.
	 * @param format The format string for the message.
	 * @param ... The variable arguments to be formatted according to the format string.
	 */
	ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type)
	{
		NOTIFY_FORMAT(this->setContent, format);
	}

	/**
	 * @brief Constructor for creating a new ImGuiToast object with a specified type, dismiss time, and content format.
	 * 
	 * @param type The type of the toast message.
	 * @param dismissTime The time in milliseconds before the toast message is dismissed.
	 * @param format The format string for the content of the toast message.
	 * @param ... The variable arguments to be formatted according to the format string.
	 */
	ImGuiToast(ImGuiToastType type, int dismissTime, const char* format, ...) : ImGuiToast(type, dismissTime)
	{
		NOTIFY_FORMAT(this->setContent, format);
	}

	/**
	 * @brief Constructor for creating a new ImGuiToast object with a specified type, dismiss time, title format, content format and a button.
	 * 
	 * @param type The type of the toast message.
	 * @param dismissTime The time in milliseconds before the toast message is dismissed.
	 * @param buttonLabel The label for the button.
	 * @param onButtonPress The lambda function to be executed when the button is pressed.
	 * @param format The format string for the content of the toast message.
	 * @param ... The variable arguments to be formatted according to the format string.
	 */
	ImGuiToast(ImGuiToastType type, int dismissTime, const char* buttonLabel, const std::function<void()>& onButtonPress, const char* format, ...) : ImGuiToast(type, dismissTime)
	{
		NOTIFY_FORMAT(this->setContent, format);

		this->onButtonPress = onButtonPress;
		this->setButtonLabel(buttonLabel);
	}
};

namespace ImGui
{
	inline std::vector<ImGuiToast> notifications;

	/**
	 * Inserts a new notification into the notification queue.
	 * @param toast The notification to be inserted.
	 */
	inline void InsertNotification(const ImGuiToast& toast)
	{
		notifications.push_back(toast);
	}

	/**
	 * @brief Removes a notification from the list of notifications.
	 * 
	 * @param index The index of the notification to remove.
	 */
	inline void RemoveNotification(int index)
	{
		notifications.erase(notifications.begin() + index);
	}


	/**
	 * Renders all notifications in the notifications vector.
	 * Each notification is rendered as a toast window with a title, content and an optional icon.
	 * If a notification is expired, it is removed from the vector.
	 */
	inline void RenderNotifications()
	{
		const ImVec2 mainWindowSize = GetMainViewport()->Size;
		float height = 0.f;

		for (size_t i = 0; i < notifications.size(); ++i)
		{
			ImGuiToast* currentToast = &notifications[i];

			if (currentToast->getPhase() == ImGuiToastPhase::Expired)
			{
				RemoveNotification(i);
				continue;
			}

		#if NOTIFY_RENDER_LIMIT > 0
			if (i > NOTIFY_RENDER_LIMIT)
				continue;
		#endif

			// Get info
			const char* icon		 = currentToast->getIcon();
			const char* title		= currentToast->getTitle();
			const char* content	  = currentToast->getContent();
			const char* defaultTitle = currentToast->getDefaultTitle();
			const float opacity	  = currentToast->getFadePercent();

			// Window name
			char windowName[50];
		#ifdef _WIN32
			sprintf_s(windowName, "##TOAST%d", (int)i);
		#else
			sprintf(windowName, "##TOAST%d", (int)i);
		#endif

			// Pick colors from theme
			const ImU32 bg_color	 = ImGui::GetColorU32(ImGuiCol_PopupBg, opacity);
			const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border, opacity);
			const ImVec4 textColor   = ImGui::GetStyleColorVec4(ImGuiCol_Text);

			// Accent color based on toast type
			ImU32 accent_color = ImGui::GetColorU32(ImGuiCol_ButtonHovered, opacity);
			if (currentToast->getType() == ImGuiToastType::Warning)
				accent_color = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered, opacity);
			else if (currentToast->getType() == ImGuiToastType::Error)
				accent_color = ImGui::GetColorU32(ImGuiCol_PlotLinesHovered, opacity);
			else if (currentToast->getType() == ImGuiToastType::Success)
				accent_color = ImGui::GetColorU32(ImGuiCol_ButtonActive, opacity);

			ImVec2 mainWindowPos = GetMainViewport()->Pos;
			SetNextWindowBgAlpha(opacity);
			SetNextWindowPos(ImVec2(mainWindowPos.x + mainWindowSize.x - NOTIFY_PADDING_X, mainWindowPos.y + mainWindowSize.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));

			if (!NOTIFY_USE_DISMISS_BUTTON && currentToast->getOnButtonPress() == nullptr)
				currentToast->setWindowFlags(NOTIFY_DEFAULT_TOAST_FLAGS | ImGuiWindowFlags_NoInputs);

			if (Begin(windowName, nullptr, currentToast->getWindowFlags()))
			{
				BringWindowToDisplayFront(GetCurrentWindow());

				ImDrawList* draw_list = GetWindowDrawList();
				ImVec2 min = GetWindowPos();
				ImVec2 max = ImVec2(min.x + GetWindowWidth(), min.y + GetWindowHeight());

				draw_list->AddRectFilled(min, max, bg_color, 5.f);
				draw_list->AddRect(min, max, border_color, 5.f);

				PushTextWrapPos(mainWindowSize.x / 3.f);
				PushStyleColor(ImGuiCol_Text, ImVec4(textColor.x, textColor.y, textColor.z, opacity));

				bool wasTitleRendered = false;

				if (!NOTIFY_NULL_OR_EMPTY(icon))
				{
					ImVec4 iconColor;
					switch (currentToast->getType())
					{
						case ImGuiToastType::Success:
							iconColor = ImVec4(0.0f, 1.0f, 0.0f, opacity); // Xanh lá
							break;
						case ImGuiToastType::Warning:
							iconColor = ImVec4(1.0f, 1.0f, 0.0f, opacity); // Vàng
							break;
						case ImGuiToastType::Error:
							iconColor = ImVec4(1.0f, 0.0f, 0.0f, opacity); // Đỏ
							break;
						case ImGuiToastType::Info:
							iconColor = ImVec4(0.0f, 0.0f, 1.0f, opacity); // Xanh dương
							break;
						
						default:
							iconColor = ImVec4(1.0f, 1.0f, 1.0f, opacity); // Trắng cho None
							break;
					}
					
					TextColored(iconColor, "%s", icon);
					wasTitleRendered = true;
				}

				if (!NOTIFY_NULL_OR_EMPTY(title))
				{
					if (!NOTIFY_NULL_OR_EMPTY(icon)) SameLine();
					Text("%s", title);
					wasTitleRendered = true;
				}
				else if (!NOTIFY_NULL_OR_EMPTY(defaultTitle))
				{
					if (!NOTIFY_NULL_OR_EMPTY(icon)) SameLine();
					Text("%s", defaultTitle);
					wasTitleRendered = true;
				}
				
				if (NOTIFY_USE_DISMISS_BUTTON)
				{
					// Kích thước nút
					const float buttonSize = GetFrameHeight(); // hoặc CalcTextSize(ICON_FA_XMARK).y + padding
					const float padding = 6.0f;

					// Tính vị trí góc trên cùng bên phải (có padding)
					ImVec2 windowPos = GetWindowPos();
					ImVec2 contentRegionMax = GetWindowContentRegionMax(); // tính từ windowPos
					float x = windowPos.x + contentRegionMax.x - buttonSize - padding;
					float y = windowPos.y + padding;

					SetCursorScreenPos(ImVec2(x, y));

					PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // Không viền nếu muốn gọn gàng
					if (Button(ICON_FA_XMARK))
					{
						PopStyleVar();
						RemoveNotification(i);
						PopStyleColor(); PopTextWrapPos(); End();
						continue;
					}
					PopStyleVar();
				}


				if (wasTitleRendered && !NOTIFY_NULL_OR_EMPTY(content))
					SetCursorPosY(GetCursorPosY() + 5.f);

				if (!NOTIFY_NULL_OR_EMPTY(content))
				{
				#if NOTIFY_USE_SEPARATOR
					if (wasTitleRendered)
						Separator();
				#endif
					Text("%s", content);
				}

				if (currentToast->getOnButtonPress() != nullptr)
				{
					if (Button(currentToast->getButtonLabel()))
						currentToast->getOnButtonPress()();
				}

				PopStyleColor();
				PopTextWrapPos();
			}

			height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;
			End();
		}
	}
}

#endif
