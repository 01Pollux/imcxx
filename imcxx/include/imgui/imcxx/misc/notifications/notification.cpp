
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
#ifndef IMCXX_MISC_NOTIFICATION_NO_DEMO
	void render_notifications_demo();
#endif

	using string_color = notification::string_color;

	struct pending_notification_info
	{
		std::vector<string_color> Title;
		std::vector<string_color> Texts;

		uint64_t Duration;

		uint32_t Id;
		uint32_t BGColor, BorderColor;

		std::function<void(uint32_t)> OnRightClick;
		std::function<void(uint32_t, bool)> OnEnd;

		pending_notification_info(
			uint32_t id,
			notification::reg_info&& info
		) noexcept :
			Title(std::move(info.Title)),
			Texts(std::move(info.Texts)),

			Duration(info.Duration),
			Id(id),

			BGColor(info.BGColor),
			BorderColor(info.BorderColor),

			OnRightClick(std::move(info.OnRightClick)),
			OnEnd(std::move(info.OnEnd))
		{}
	};

	struct notification_info
	{
		std::vector<string_color> Title;
		std::vector<string_color> Texts;

		std::chrono::system_clock::time_point StartTime;
		std::chrono::system_clock::time_point EndTime;

		uint32_t Id;
		uint32_t BGColor, BorderColor;

		std::function<void(uint32_t)> OnRightClick;
		std::function<void(uint32_t, bool)> OnEnd;

		notification_info(
			uint32_t id,
			std::chrono::system_clock::time_point time_point,
			notification::reg_info&& info
		) noexcept :
			Title(std::move(info.Title)),
			Texts(std::move(info.Texts)),

			StartTime(time_point),
			EndTime(time_point + std::chrono::milliseconds(info.Duration)),
			Id(id),

			BGColor(info.BGColor),
			BorderColor(info.BorderColor),

			OnRightClick(std::move(info.OnRightClick)),
			OnEnd(std::move(info.OnEnd))
		{}

		notification_info(
			pending_notification_info&& pending
		) noexcept :
			Title(std::move(pending.Title)),
			Texts(std::move(pending.Texts)),

			StartTime(std::chrono::system_clock::now()),
			EndTime(StartTime + std::chrono::milliseconds(pending.Duration)),
			Id(std::move(pending.Id)),

			BGColor(pending.BGColor),
			BorderColor(pending.BorderColor),

			OnRightClick(std::move(pending.OnRightClick)),
			OnEnd(std::move(pending.OnEnd))
		{}
	};

	static std::vector<notification_info> _Notifications;
	static std::deque<pending_notification_info> _PendingNotifications;


	static constexpr auto fade_in_cooldown = std::chrono::milliseconds(500);
	static constexpr float fade_in_mult = 0.003f;

	static constexpr auto fade_out_cooldown = std::chrono::milliseconds(500);
	static constexpr float fade_out_mult = 0.003f;

	static constexpr size_t max_popups_in_window = 3;


	notification::notification(reg_info&& info, uint32_t* out_id)
	{
		uint32_t id = 0;
		while (std::any_of(_Notifications.begin(), _Notifications.end(), [id](const auto& it) { return it.Id == id; }) ||
			std::any_of(_PendingNotifications.begin(), _PendingNotifications.end(), [id](const auto& it) { return it.Id == id; }))
			++id;

		if (out_id)
			*out_id = id;

		_PendingNotifications.emplace_back(
			id,
			std::move(info)
		);
	}


	notification::notification(query_info& info)
	{
		auto check_in = [&info](auto& container, bool pending)
		{
			auto iter = std::find_if(
				std::begin(container), std::end(container), [id = info.Id](const auto& it){ return it.Id == id; }
			);

			if (iter != std::end(container))
			{
				if (info.Exists)
					*info.Exists = true;
				if (info.Pending)
					*info.Pending = pending;

				if (info.Delete)
					container.erase(iter);
				else
				{
					if (info.Title)
						*info.Title = &iter->Title;
					if (info.Texts)
						*info.Texts = &iter->Texts;
					if (info.BGColor)
						*info.BGColor = &iter->BGColor;
					if (info.BorderColor)
						*info.BorderColor = &iter->BorderColor;
				}

				if (info.Pending)
					*info.Pending = pending;
				return true;
			}
			else
			{
				if (info.Exists)
					*info.Exists = false;
				if (info.Pending)
					*info.Pending = pending;
				return false;
			}
		};

		if (!check_in(_Notifications, false))
			check_in(_PendingNotifications, true);
	}


	[[nodiscard]]
	float get_opacity(
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
		render_notifications_demo();
#endif
		auto now = std::chrono::system_clock::now();

		{
			auto it = std::remove_if(
				_Notifications.begin(),
				_Notifications.end(),
				[now](const auto& it)
				{
					if (it.EndTime <= now)
					{
						if (it.OnEnd)
							it.OnEnd(it.Id, false);
						return true;
					}
					else return false;
				}
			);
			_Notifications.erase(it, _Notifications.end());

			if (_PendingNotifications.size() && _Notifications.size() < max_popups_in_window)
			{
				for (size_t i = 0; i < _PendingNotifications.size() && _Notifications.size() < max_popups_in_window; i++)
				{
					_Notifications.emplace_back(std::move(_PendingNotifications.front()));
					_PendingNotifications.pop_front();
				}
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
			ImGui::SetNextWindowSize({ main_view->WorkSize.x / 3.2f, -FLT_MAX });

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
				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoSavedSettings
			);

			bg_and_border.pop_all();

			if (notification)
			{
				using namespace std::chrono_literals;

				if (ImGui::IsWindowHovered())
					notif->EndTime += static_cast<size_t>(ImGui::GetIO().DeltaTime * 1600.f) * 1ms;

				if (imcxx::popup close_popup{ imcxx::popup::context_window{}, "##NotificationPopup" })
				{
					notif->EndTime += static_cast<size_t>(ImGui::GetIO().DeltaTime * 1600.f) * 1ms;
					if (notif->OnRightClick)
						notif->OnRightClick(notif->Id);
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
			{
				if (notif->OnEnd)
					notif->OnEnd(notif->Id, true);
				notif = _Notifications.erase(notif);
			}
			else
			{
				++notif;
				++num_rendered;
			}
		}
	}
}