#pragma once

#include "scopes.hpp"

namespace imcxx
{
	/// <summary>
	/// Widgets: Trees
	/// - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
	/// </summary>
	class tree_node : public scope_wrap<tree_node>
	{
		friend class scope_wrap;
	public:
		struct va_args {};
		struct push {};

		tree_node(const char* label, ImGuiTreeNodeFlags flags = 0) :
			scope_wrap(ImGui::TreeNodeEx(label, flags) && should_tree_pop(flags))
		{}

		template<typename ..._Args>
		tree_node(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, _Args&&... args) :
			scope_wrap(ImGui::TreeNodeEx(ptr_id, flags, fmt, std::forward<_Args>(args)...) && should_tree_pop(flags))
		{}
		
		template<typename ..._Args>
		tree_node(std::string_view str_id, ImGuiTreeNodeFlags flags, const char* fmt, _Args&&... args) :
			scope_wrap(ImGui::TreeNodeEx(str_id.data(), flags, fmt, std::forward<_Args>(args)...) && should_tree_pop(flags))
		{}
		

		tree_node(va_args, const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list arg_list) :
			scope_wrap(ImGui::TreeNodeExV(ptr_id, flags, fmt, arg_list) && should_tree_pop(flags))
		{}
		
		tree_node(va_args, std::string_view str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list arg_list) :
			scope_wrap(ImGui::TreeNodeExV(str_id.data(), flags, fmt, arg_list) && should_tree_pop(flags))
		{}


		template<typename ..._Args>
		tree_node(const void* ptr_id, const char* fmt, _Args&&... args) :
			tree_node(ptr_id, 0, fmt, std::forward<_Args>(args)...)
		{}

		template<typename ..._Args>
		tree_node(std::string_view str_id, const char* fmt, _Args&&... args) :
			tree_node(str_id.data(), 0, fmt, std::forward<_Args>(args)...)
		{}


		tree_node(va_args, const void* ptr_id, const char* fmt, va_list arg_list) :
			tree_node(ptr_id, 0, fmt, arg_list)
		{}

		tree_node(va_args, std::string_view str_id, const char* fmt, va_list arg_list) :
			tree_node(str_id.data(), 0, fmt, arg_list)
		{}


		tree_node(push, const void* ptr_id) :
			scope_wrap(true)
		{
			ImGui::TreePush(ptr_id);
		}

		tree_node(push, std::string_view str_id) :
			scope_wrap(true)
		{
			ImGui::TreePush(str_id.data());
		}

		/// <summary>
		/// set next TreeNode/CollapsingHeader open state.
		/// </summary>
		void open_next(bool is_open, ImGuiCond cond = 0)
		{
			ImGui::SetNextItemOpen(is_open, cond);
		}

	private:
		static constexpr bool should_tree_pop(ImGuiTreeNodeFlags flags) noexcept
		{
			return !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen);
		}

		void destruct()
		{
			ImGui::TreePop();
		}
	};

	class collapsing_header : public scope_wrap<collapsing_header, scope_traits::no_dtor>
	{
	public:
		collapsing_header(const char* label, ImGuiTreeNodeFlags flags = 0) :
			scope_wrap(ImGui::CollapsingHeader(label, flags))
		{}

		collapsing_header(std::string_view label, ImGuiTreeNodeFlags flags = 0) :
			collapsing_header(label.data(), flags)
		{}

		void open_next(bool is_open, ImGuiCond cond = 0)
		{
			ImGui::SetNextItemOpen(is_open, cond);
		}
	};
}