#pragma once

#include "imgui/imcxx/scopes.hpp"
#include <vector>

namespace imcxx::misc
{
	/// <summary>
	/// Create or query a notification, configurable by 'popup_info'
	/// </summary>
	class notification : public scope_wrap<notification, imcxx::scope_traits::no_dtor, false>
	{
	public:

		struct reg_info;
		struct query_info;

		/// <summary>
		/// Register a new notification for display
		/// <param name="id">if 'Id' is not nullptr, Id will have notification's id</param>
		/// <returns></returns>
		IMGUI_API notification(const reg_info&, uint32_t* id = nullptr);


		/// <summary>
		/// Find an pre-existing popup
		/// </summary>
		IMGUI_API notification(query_info&);

		/// <summary>
		/// Display the popups
		/// </summary>
		/// <returns></returns>
		IMGUI_API static void render();
	};


	struct notification::reg_info
	{
		struct string_color
		{
			std::string string;
			uint32_t color;

			_CONSTEXPR20 string_color(std::string_view str, uint32_t color = 0xFF'FF'FF'FF) noexcept :
				string(str), color(color)
			{}
		};
		/// <summary>
		/// title's string with color
		/// </summary>
		std::vector<string_color> Title;
		/// <summary>
		/// text's string with color
		/// </summary>
		std::vector<string_color> Texts;
		/// <summary>
		/// notification's duration in milli-seconds
		/// </summary>
		uint64_t Duration;
		/// <summary>
		/// notification's background/foreground color
		/// </summary>
		uint32_t 
			BGColor{ ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]) }, 
			BorderColor{ ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Border]) };
	};


	struct notification::query_info
	{
		struct string_color
		{
			std::string string;
			uint32_t color;

			string_color(std::string_view str, uint32_t color = 0xFF'FF'FF'FF) noexcept :
				string(str), color(color)
			{}
		};
		// In
		
		/// <summary>
		/// if 'Delete' was set to true, 
		/// </summary>
		bool Delete;
		/// <summary>
		/// Id of notification
		/// </summary>
		uint32_t Id;

		// Out

		/// <summary>
		/// if 'Exists' is not nullptr and the notification exists, 'Exists' will be set to true, else false
		/// </summary>
		bool* Exists{ };
		/// <summary>
		/// if 'Title' is not nullptr and the notification exists, 'Title' will be pointing to notification's title
		/// </summary>
		std::vector<string_color>** Title{ };
		/// <summary>
		/// if 'Texts' is not nullptr and the notification exists, 'Texts' will be pointing to notification's title
		/// </summary>
		std::vector<string_color>** Texts{ };

		/// <summary>
		/// if 'BGColor' or 'FGColor' is not nullptr and the notification exists, they will be pointing to notification's background/foreground color
		/// </summary>
		uint32_t
			** BGColor,
			** BorderColor;
	};
}