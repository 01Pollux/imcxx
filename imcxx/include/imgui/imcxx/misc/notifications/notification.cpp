
#include <chrono>
#include <deque>
#include <algorithm>

#include "../notification.hpp"

#include "imgui/imcxx/viewports.hpp"
#include "imgui/imcxx/window.hpp"
#include "imgui/imcxx/override.hpp"
#include "imgui/imcxx/popup.hpp"


namespace imcxx::misc
{
#ifndef IMCXX_NOTIFICATION_NO_DEMO
	void render_demo();
#endif

	using string_color = notification::query_info::string_color;
	using in_string_color = notification::reg_info::string_color;

	struct pending_notification_info
	{
		std::vector<string_color> Title;
		std::vector<string_color> Texts;

		uint64_t Duration;

		uint32_t Id;
		uint32_t BGColor, BorderColor;

		pending_notification_info(
			const std::vector<in_string_color>& title,
			const std::vector<in_string_color>& texts,
			uint64_t duration,
			uint32_t id,
			uint32_t bgcolor,
			uint32_t fgcolor
		) :
			Duration(duration),
			Id(id),

			BGColor(bgcolor),
			BorderColor(fgcolor)
		{
			Title.reserve(title.size());
			for (auto& v : title)
				Title.emplace_back(v.string, v.color);

			Texts.reserve(texts.size());
			for (auto& v : texts)
				Texts.emplace_back(v.string, v.color);
		}
	};

	struct notification_info
	{
		std::vector<string_color> Title;
		std::vector<string_color> Texts;

		std::chrono::system_clock::time_point StartTime;
		std::chrono::system_clock::time_point EndTime;

		uint32_t Id;
		uint32_t BGColor, BorderColor;

		notification_info(
			const std::vector<in_string_color>& title,
			const std::vector<in_string_color>& texts,
			std::chrono::system_clock::time_point time_point,
			uint64_t duration,
			uint32_t id,
			uint32_t bgcolor,
			uint32_t fgcolor
		) :
			StartTime(time_point),
			EndTime(time_point + std::chrono::milliseconds(duration)),
			Id(id),

			BGColor(bgcolor),
			BorderColor(fgcolor)
		{
			Title.reserve(title.size());
			for (auto& v : title)
				Title.emplace_back(v.string, v.color);

			Texts.reserve(texts.size());
			for (auto& v : texts)
				Texts.emplace_back(v.string, v.color);
		}

		notification_info(
			pending_notification_info&& pending
		) :
			Title(std::move(pending.Title)),
			Texts(std::move(pending.Texts)),

			StartTime(std::chrono::system_clock::now()),
			EndTime(StartTime + std::chrono::milliseconds(pending.Duration)),
			Id(std::move(pending.Id)),

			BGColor(pending.BGColor),
			BorderColor(pending.BorderColor)
		{}
	};

	static std::vector<notification_info> _Notifications;
	static std::deque<pending_notification_info> _PendingNotifications;


	static constexpr auto fade_in_cooldown = std::chrono::milliseconds(500);
	static constexpr float fade_in_mult = 0.003f;

	static constexpr auto fade_out_cooldown = std::chrono::milliseconds(500);
	static constexpr float fade_out_mult = 0.003f;

	static constexpr size_t max_popups_in_window = 3;


	notification::notification(const reg_info& info, uint32_t* out_id)
	{
		uint32_t id = 0;
		while (std::any_of(_Notifications.begin(), _Notifications.end(), [id](const auto& it) { return it.Id == id; }) ||
			std::any_of(_PendingNotifications.begin(), _PendingNotifications.end(), [id](const auto& it) { return it.Id == id; }))
			++id;

		if (out_id)
			*out_id = id;

		if (_Notifications.size() < max_popups_in_window)
		{
			auto now = std::chrono::system_clock::now();
			_Notifications.emplace_back(
				info.Title,
				info.Texts,
				now,
				info.Duration,
				id,
				info.BGColor,
				info.BorderColor
			);
		}
		else
		{
			_PendingNotifications.emplace_back(
				info.Title,
				info.Texts,
				info.Duration,
				id,
				info.BGColor,
				info.BorderColor
			);
		}
	}

	notification::notification(query_info& info)
	{
		auto iter = std::find_if(
			_Notifications.begin(), _Notifications.end(), [id = info.Id](const auto& it){ return it.Id == id; }
		);

		if (iter != _Notifications.end())
		{
			if (info.Exists)
				*info.Exists = true;

			if (info.Delete)
				_Notifications.erase(iter);
			else
			{
				if (info.Title)
					*info.Title = std::addressof(iter->Title);
				if (info.Texts)
					*info.Texts = std::addressof(iter->Texts);
				if (info.BGColor)
					*info.BGColor = std::addressof(iter->BGColor);
				if (info.BorderColor)
					*info.BorderColor = std::addressof(iter->BorderColor);
			}
		}
		else
		{
			if (info.Exists)
				*info.Exists = false;
		}
	}

	[[nodiscard]] float get_opacity(
		std::chrono::system_clock::time_point begin_time,
		std::chrono::system_clock::time_point end_time,
		std::chrono::system_clock::time_point cur_time
	)
	{
		using namespace std::chrono_literals;
		auto time_left = end_time - cur_time;
		if (time_left < fade_out_cooldown)
		{
			return ((end_time - cur_time) / 1ms) * fade_out_mult;
		}
		else if ((cur_time - begin_time) < fade_in_cooldown)
		{
			return ((cur_time - begin_time) / 1ms) * fade_in_mult;
		}
		else return 1.f;
	}

	void notification::render()
	{
#ifndef IMCXX_NOTIFICATION_NO_DEMO
		render_demo();
#endif
		auto now = std::chrono::system_clock::now();

		auto it = std::remove_if(
			_Notifications.begin(),
			_Notifications.end(),
			[now](const auto& it)
			{
				return it.EndTime <= now;
			}
		);
		_Notifications.erase(it, _Notifications.end());

		if (_PendingNotifications.size() && _Notifications.size() < max_popups_in_window)
		{
			const size_t num_pendings = std::clamp(std::min(_Notifications.size(), _PendingNotifications.size()), 0u, 3u);
			for (size_t i = 0; i < num_pendings; i++)
			{
				_Notifications.emplace_back(std::move(_PendingNotifications.front()));
				_PendingNotifications.pop_front();
			}
		}

		imcxx::viewport main_view;
		ImVec2 view_start_pos{ main_view->WorkPos.x + main_view->WorkSize.x - 20.f, main_view->WorkPos.y + main_view->WorkSize.y - 20.f };

		char window_name[48];

		size_t num_rendered = 0;
		for (auto notif = _Notifications.begin(); notif != _Notifications.end() && num_rendered < max_popups_in_window;)
		{
			ImGui::SetNextWindowBgAlpha(get_opacity(notif->StartTime, notif->EndTime, now));
			ImGui::SetNextWindowPos({ view_start_pos.x, view_start_pos.y }, ImGuiCond_Always, ImVec2(1.f, 1.f));
			ImGui::SetNextWindowSize({ view_start_pos.x / 3.2f, -FLT_MAX });

			imcxx::shared_color bg_and_border{
				ImGuiCol_WindowBg,
				notif->BGColor,
				ImGuiCol_Border,
				notif->BorderColor
			};

			bool close = false;

			sprintf_s(window_name, "##NOTIF%x", notif->Id);
			imcxx::window notification(
				window_name,
				nullptr,
#ifdef IMGUI_HAS_DOCK
				ImGuiWindowFlags_NoDocking |
#endif
				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings
			);

			bg_and_border.pop_all();

			if (notification)
			{
				using namespace std::chrono_literals;

				if (ImGui::IsWindowHovered())
					notif->EndTime += static_cast<size_t>(ImGui::GetIO().DeltaTime * 1600.f) * 1ms;

				if (imcxx::popup close_popup{ imcxx::popup::context_window{}, "##Close Window" })
				{
					notif->EndTime += static_cast<size_t>(ImGui::GetIO().DeltaTime * 1600.f) * 1ms;
					if (ImGui::Selectable("Close"))
						close = true;
				}

				imcxx::shared_textwrap text_wrap(view_start_pos.x / 3.4f);

				for (auto& title : notif->Title)
				{
					imcxx::shared_color color(ImGuiCol_Text, title.color);
					ImGui::TextUnformatted(title.string.c_str(), title.string.c_str() + title.string.size());
				}

				const float frac = static_cast<float>((notif->EndTime - now) / 1ms) / ((notif->EndTime - notif->StartTime) / 1ms);
				ImGui::ProgressBar(frac, { -FLT_MIN, 2.f }, "");

				if (!notif->Title.empty())
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);

				for (auto& text : notif->Texts)
				{
					imcxx::shared_color color(ImGuiCol_Text, text.color);
					ImGui::TextUnformatted(text.string.c_str(), text.string.c_str() + text.string.size());
				}
			}

			view_start_pos.y -= ImGui::GetWindowSize().y + 10.f;

			if (close)
				notif = _Notifications.erase(notif);
			else
			{
				++notif;
				++num_rendered;
			}
		}
	}
}