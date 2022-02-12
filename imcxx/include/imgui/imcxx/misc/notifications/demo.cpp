#ifndef IMCXX_NOTIFICATION_NO_DEMO

#include "../notification.hpp"

#include "imgui/imcxx/viewports.hpp"
#include "imgui/imcxx/window.hpp"
#include "imgui/imcxx/override.hpp"
#include "imgui/imcxx/popup.hpp"
#include "imgui/imcxx/nodes.hpp"
#include "imgui/imcxx/colors.hpp"
#include "imgui/imcxx/input.hpp"


namespace imcxx::misc
{
	void render_demo()
	{
		ImGui::SetNextWindowSize({ 550.f, 550.f }, ImGuiCond_Once);
		imcxx::window notification_demo(
			"Notification demo"
		);
		if (!notification_demo)
			return;

		if (imcxx::collapsing_header example_header{ "Random examples", ImGuiTreeNodeFlags_DefaultOpen })
		{
			if (ImGui::Button("Error"))
			{
				imcxx::misc::notification::reg_info cfg;
				cfg.Title.emplace_back("Error", ImGui::ColorConvertFloat4ToU32({ 1.f, 0.f, 0.f, 1.f }));
				cfg.Texts.emplace_back("Something went wrong...");
				cfg.Duration = 4'000;

				imcxx::misc::notification::call(cfg);
			}
			
			if (ImGui::Button("Debug"))
			{
				imcxx::misc::notification::reg_info cfg;

				cfg.Title.emplace_back("Debug", ImGui::ColorConvertFloat4ToU32({ 1.f, 0.f, 0.f, 1.f }));
				cfg.Texts.emplace_back("Some debug information.");
				cfg.Duration = 4'000;

				imcxx::misc::notification::call(cfg);
			}
			if (ImGui::Button("Lorem ipsum"))
			{
				imcxx::misc::notification::reg_info cfg;
				cfg.BGColor = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.2f, 0.4f, 0.4f });
				cfg.BorderColor = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.8f, 0.f, 0.9f });

				cfg.Title.emplace_back("Lorem Ipsum", ImGui::ColorConvertFloat4ToU32({ 1.f, 0.f, 0.f, 1.f }));
				cfg.Texts.emplace_back(
					"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut"
					"labore et dolore magna aliqua.Posuere sollicitudin aliquam ultrices sagittis.A lacus vestibulum "
					"sed arcu non odio euismod lacinia.Enim neque volutpat ac tincidunt.Enim neque volutpat ac tincidunt "
					"vitae semper quis lectus.Lacus viverra vitae congue eu consequat ac felis donec et.Enim sed faucibus "
					"turpis in eu mi bibendum neque.Mi quis hendrerit dolor magna eget est lorem.Molestie ac feugiat sed "
					"lectus vestibulum.Vitae proin sagittis nisl rhoncus.",
					ImGui::ColorConvertFloat4ToU32({ 0.f, 1.f, 0.f, 1.f })
				);
				cfg.Duration = 7'000;

				imcxx::misc::notification::call(cfg);
			}
		}

		if (imcxx::collapsing_header example_header{ "Custom" })
		{
			static std::vector<std::pair<std::string, ImVec4>> titles, texts;
			static ImVec4 bgcolor{}, border_color{};
			static int duration{};

			imcxx::color::call(imcxx::color::edit{}, "Background color", bgcolor);
			imcxx::color::call(imcxx::color::edit{}, "Border color", border_color);
			ImGui::InputInt("Duration", &duration);


			for (auto entry : std::array{
					std::addressof(titles),
					std::addressof(texts)
				})
			{
				imcxx::shared_item_id entry_override(entry);
				if (ImGui::Button("New Text"))
					entry->emplace_back();
				ImGui::SameLine();
				if (ImGui::Button("Pop Text"))
					entry->pop_back();

				if (entry->size())
				{
					for (auto& str_col : *entry)
					{
						imcxx::shared_item_id id_override(std::addressof(str_col));
						imcxx::input::call(imcxx::input::text{}, "##InputText", str_col.first);
						imcxx::color::call(imcxx::color::edit{}, "##color", str_col.second);
					}
				}

				ImGui::Separator();
				ImGui::Separator();
			}
			
			if (ImGui::Button("Send"))
			{
				imcxx::misc::notification::reg_info cur_config;

				cur_config.Duration = static_cast<uint64_t>(duration);
				cur_config.BGColor = ImGui::ColorConvertFloat4ToU32(bgcolor);
				cur_config.BorderColor = ImGui::ColorConvertFloat4ToU32(border_color);

				for (auto& entry : std::array{
						std::pair{ std::addressof(cur_config.Title), titles},
						std::pair{ std::addressof(cur_config.Texts), texts}
					})
				{
					for (auto& [str, color] : entry.second)
						entry.first->emplace_back(std::string_view{ str.c_str(), str.size() }, ImGui::ColorConvertFloat4ToU32(color));
				}

				imcxx::misc::notification::call(cur_config);
			}
		}
	}
}


#endif