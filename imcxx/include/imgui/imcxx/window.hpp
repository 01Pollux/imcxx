#pragma once

#include <array>
#include "scopes.hpp"
#include "imgui/imgui_internal.h"

namespace imcxx
{
	/// <summary>
	/// - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
	/// - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
	///   which clicking will set the boolean to false when clicked.
	/// - You may append multiple times to the same window during the same frame by calling Begin()/End() pairs multiple times.
	///   Some information such as 'flags' or 'p_open' will only be considered by the first call to Begin().
	/// - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
	///   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
	///   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
	///    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
	///    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
	/// - Note that the bottom of window stack always contains a window called "Debug".
	/// </summary>
	class [[nodiscard]] window : public scope_wrap<window, scope_traits::always_dtor>
	{
		friend class scope_wrap;
	public:
		class child;
		class capture;

		window(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) :
			scope_wrap(ImGui::Begin(name, p_open, flags))
		{}

		window(std::string_view str, bool* p_open = nullptr, ImGuiWindowFlags flags = 0) :
			window(str.data(), p_open, flags)
		{}
		
	private:
		void destruct()
		{
			ImGui::End();
		}
	};

	using window_child = window::child;
	using current_window = window::capture;

	/// <summary>
	/// Child Windows
	/// - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
	/// - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
	/// - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
	///   Always call a matching EndChild() for each BeginChild() call, regardless of its return value.
	///   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
	///    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
	///    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
	/// </summary>
	class [[nodiscard]] window::child : public scope_wrap<window::child, scope_traits::always_dtor>
	{
		friend class scope_wrap;
	public:
		struct frame {};

		child(const char* str_id, const ImVec2& size = {}, bool border = false, ImGuiWindowFlags flags = 0) :
			scope_wrap(ImGui::BeginChild(str_id, size, border, flags))
		{}

		child(std::string_view str, const ImVec2& size = {}, bool border = false, ImGuiWindowFlags flags = 0) :
			child(str.data(), size, border, flags)
		{}


		child(const char* str_id, const std::array<float, 2>& size = { 0.f, 0.f }, bool border = false, ImGuiWindowFlags flags = 0) :
			child(str_id, ImVec2{ size[0], size[1] }, border, flags)
		{}

		child(std::string_view str, const std::array<float, 2>& size = { 0.f, 0.f }, bool border = false, ImGuiWindowFlags flags = 0) :
			child(str, ImVec2{ size[0], size[1] }, border, flags)
		{}
		

		child(ImGuiID id, const ImVec2& size = {}, bool border = false, ImGuiWindowFlags flags = 0) :
			scope_wrap(ImGui::BeginChild(id, size, border, flags))
		{}

		child(ImGuiID id, const std::array<float, 2>& size = {}, bool border = false, ImGuiWindowFlags flags = 0) :
			child(id, ImVec2{ size[0], size[1] }, border, flags)
		{}

		
		child(frame, ImGuiID id, const ImVec2& size = {}, ImGuiWindowFlags flags = 0) :
			scope_wrap(ImGui::BeginChildFrame(id, size, flags))
		{}

		child(frame, ImGuiID id, const std::array<float, 2>& size = {}, ImGuiWindowFlags flags = 0) :
			child(frame{}, id, ImVec2{ size[0], size[1] }, flags)
		{}

	private:
		void destruct()
		{
			ImGui::EndChild();
		}
	};
}