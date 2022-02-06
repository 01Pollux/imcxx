#pragma once

#include "scopes.hpp"

namespace imcxx
{
	/// <summary>
	/// Tab Bars, Tabs
	/// Note: Tabs are automatically created by the docking system. Use this to create tab bars/tabs yourself without docking being involved.
	/// </summary>
	class tabbar : public scope_wrap<tabbar>
	{
		friend class scope_wrap;
	public:
		class item;

		tabbar(const char* name, ImGuiTabBarFlags flags = 0) :
			scope_wrap(ImGui::BeginTabBar(name, flags))
		{}

		tabbar(std::string_view name, ImGuiTabBarFlags flags = 0) :
			tabbar(name.data(), flags)
		{}

		/// <summary>
		/// create a Tab. Returns true if the Tab is selected.
		/// </summary>
		[[nodiscard]] item add_item(const char* label, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0);

		/// <summary>
		/// create a Tab. Returns true if the Tab is selected.
		/// </summary>
		[[nodiscard]] item add_item(std::string_view label, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0);

		/// <summary>
		/// create a Tab behaving like a button. return true when clicked. cannot be selected in the tab bar
		/// </summary>
		[[nodiscard]] bool add_button(const char* label, ImGuiTabItemFlags flags = 0)
		{
			return ImGui::TabItemButton(label, flags);
		}

		/// <summary>
		/// create a Tab behaving like a button. return true when clicked. cannot be selected in the tab bar
		/// </summary>
		[[nodiscard]] bool add_button(std::string_view label, ImGuiTabItemFlags flags = 0)
		{
			return ImGui::TabItemButton(label.data(), flags);
		}

		/// <summary>
		/// notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.
		/// </summary>
		void notify_tab_closed(const char* tab_or_docked_window_label)
		{
			ImGui::SetTabItemClosed(tab_or_docked_window_label);
		}
		
	private:
		void destruct()
		{
			ImGui::EndTabBar();
		}
	};

	class [[nodiscard]] tabbar::item : public scope_wrap<item>
	{
		friend class scope_wrap;
	public:
		item(const char* label, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0) :
			scope_wrap(ImGui::BeginTabItem(label, p_open, flags))
		{}

		item(std::string_view label, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0) :
			item(label.data(), p_open, flags)
		{}

	private:
		void destruct()
		{
			ImGui::EndTabItem();
		}
	};

	inline auto tabbar::add_item(const char* label, bool* p_open, ImGuiTabItemFlags flags) -> item
	{
		return item{ label, p_open, flags };
	}

	inline auto tabbar::add_item(std::string_view label, bool* p_open, ImGuiTabItemFlags flags) -> item
	{
		return item{ label.data(), p_open, flags};
	}
}