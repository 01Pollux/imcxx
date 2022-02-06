#pragma once

#include "scopes.hpp"

namespace imcxx
{
	/// <summary>
	/// Widgets: Main
	/// - Most widgets return true when the value has been changed or when pressed/selected
	/// - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.
	/// </summary>
	class color : public scope_wrap<color, scope_traits::no_dtor>
	{
	public:
		struct edit {};
		struct picker {};

		template<typename _Ty, size_t _Size>
		color(picker, const char* label, _Ty (&col)[_Size], ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr)
		{
			constexpr float to_ratio = std::is_same_v<_Ty, float> ? 1.f : 255.f;
			float tmp[4]{
				col[0] / to_ratio,
				col[1] / to_ratio,
				col[2] / to_ratio,
				_Size < 4 ? 1.f : col[3] / to_ratio
			};
			if (_Size != 4)
				flags |= ImGuiColorEditFlags_NoAlpha;
			if (m_Result._Value = ImGui::ColorPicker4(label, tmp, flags, ref_col))
			{
				for (size_t i = 0; i < _Size; i++)
				{
					col[i] = static_cast<_Ty>(tmp[i] * to_ratio);
				}
			}
		}

		template<typename _Ty, size_t _Size>
		color(picker, std::string_view label, _Ty(&col)[_Size], ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr) :
			color(picker{}, label.data(), col, flags, ref_col)
		{}


		template<typename _Ty, size_t _Size>
		color(picker, const char* label, std::array<_Ty, _Size>& col, ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr)
		{
			constexpr float to_ratio = std::is_same_v<_Ty, float> ? 1.f : 255.f;
			float tmp[4]{
				col[0] / to_ratio,
				col[1] / to_ratio,
				col[2] / to_ratio,
				_Size < 4 ? 1.f : col[3] / to_ratio
			};
			if (_Size != 4)
				flags |= ImGuiColorEditFlags_NoAlpha;
			if (m_Result._Value = ImGui::ColorPicker4(label, tmp, flags, ref_col))
			{
				for (size_t i = 0; i < _Size; i++)
				{
					col[i] = static_cast<_Ty>(tmp[i] * to_ratio);
				}
			}
		}

		template<typename _Ty, size_t _Size>
		color(picker, std::string_view label, std::array<_Ty, _Size>& col, ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr) :
			color(picker{}, label.data(), col, flags, ref_col)
		{}


		color(picker, const char* label, ImVec4& col, ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr) :
			scope_wrap(ImGui::ColorPicker4(label, &col.x, flags, ref_col))
		{}
			
		color(picker, std::string_view label, ImVec4& col, ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr) :
			color(picker{}, label.data(), col, flags, ref_col)
		{}


		template<typename _Ty, size_t _Size>
		color(edit, const char* label, _Ty(&col)[_Size], ImGuiColorEditFlags flags = 0)
		{
			constexpr float to_ratio = std::is_same_v<_Ty, float> ? 1.f : 255.f;
			float tmp[4]{
				col[0] / to_ratio,
				col[1] / to_ratio,
				col[2] / to_ratio,
				_Size < 4 ? 1.f : col[3] / to_ratio
			};
			if (_Size != 4)
				flags |= ImGuiColorEditFlags_NoAlpha;
			if (m_Result._Value = ImGui::ColorEdit4(label, tmp, flags))
			{
				for (size_t i = 0; i < _Size; i++)
				{
					col[i] = static_cast<_Ty>(tmp[i] * to_ratio);
				}
			}
		}

		template<typename _Ty, size_t _Size>
		color(edit, std::string_view label, _Ty(&col)[_Size], ImGuiColorEditFlags flags = 0) :
			color(edit{}, label.data(), col, flags)
		{}


		template<typename _Ty, size_t _Size>
		color(edit, const char* label, std::array<_Ty, _Size>& col, ImGuiColorEditFlags flags = 0)
		{
			constexpr float to_ratio = std::is_same_v<_Ty, float> ? 1.f : 255.f;
			float tmp[4]{
				col[0] / to_ratio,
				col[1] / to_ratio,
				col[2] / to_ratio,
				_Size < 4 ? 1.f : col[3] / to_ratio
			};
			if (_Size != 4)
				flags |= ImGuiColorEditFlags_NoAlpha;
			if (m_Result._Value = ImGui::ColorEdit4(label, tmp, flags))
			{
				for (size_t i = 0; i < _Size; i++)
				{
					col[i] = static_cast<_Ty>(tmp[i] * to_ratio);
				}
			}
		}

		template<typename _Ty, size_t _Size>
		color(edit, std::string_view label, std::array<_Ty, _Size>& col, ImGuiColorEditFlags flags = 0) :
			color(edit{}, label.data(), col, flags)
		{}


		color(edit, const char* label, ImVec4& col, ImGuiColorEditFlags flags = 0) :
			scope_wrap(ImGui::ColorEdit4(label, &col.x, flags))
		{}

		color(edit, std::string_view label, ImVec4& col, ImGuiColorEditFlags flags = 0) :
			color(edit{}, label.data(), col, flags)
		{}


		color(ImGuiColorEditFlags flags)
		{
			ImGui::SetColorEditOptions(flags);
		}
	};
}
