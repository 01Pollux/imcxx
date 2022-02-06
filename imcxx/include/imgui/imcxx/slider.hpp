#pragma once

#include <type_traits>
#include <numeric>
#include "scopes.hpp"

namespace imcxx
{
	/// <summary>
	/// Widgets: Regular Sliders
	/// - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped by default and can go off-bounds. Use ImGuiSliderFlags_AlwaysClamp to always clamp.
	/// - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
	/// - Format string may also be set to NULL or use the default format ("%f" or "%d").
	/// - Legacy: Pre-1.78 there are SliderXXX() function signatures that takes a final `float power=1.0f' argument instead of the `ImGuiSliderFlags flags=0' argument.
	///   If you get a warning converting a float to ImGuiSliderFlags, read https://github.com/ocornut/imgui/issues/3361	
	/// </summary>
	class slider : public scope_wrap<slider, scope_traits::no_dtor>
	{
	public:
		struct angle {};
		struct pointer {};
		template<size_t _Size>
		struct array 
		{
			static constexpr size_t size = _Size;
		};
		/// <summary>
		/// take the whole array instead of slicing it
		/// </summary>
		using auto_array = array<0>;

		template<typename T>
		static constexpr const char* default_c_format = "";

		/// <summary>
		/// Calls 'ImGui::SliderScalar'
		/// </summary>
		template<typename _Ty>
		slider(
			const char* label, 
			_Ty* v, 
			_Ty v_min,
			_Ty v_max, 
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::SliderScalar()'");
#if _HAS_CXX20
			m_Result._Value = ImGui::SliderScalar(
				label,
				type, 
				std::bit_cast<void*>(v),
				std::bit_cast<const void*>(&v_min),
				std::bit_cast<const void*>(&v_max),
				format,
				flags
			);
#else
			m_Result._Value = ImGui::SliderScalar(
				label, 
				type,
				reinterpret_cast<void*>(v),
				reinterpret_cast<const void*>(&v_min),
				reinterpret_cast<const void*>(&v_max), 
				format, 
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::SliderScalar'
		/// </summary>
		template<typename _Ty, typename = std::enable_if_t<!std::is_array_v<_Ty>>>
		slider(
			const char* label, 
			_Ty& v, 
			_Ty v_min,
			_Ty v_max, 
			const char* format = default_c_format<_Ty>, 
			ImGuiSliderFlags flags = 0
		) : 
			slider(label, &v, v_min, v_max, format, flags)
		{}


		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v', where v is contiguous-pointer
		/// </summary>
		template<typename _Ty>
		slider(
			pointer, 
			const char* label,
			_Ty* v, 
			size_t size, 
			_Ty v_min, 
			_Ty v_max, 
			const char* format = default_c_format<_Ty>, 
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::SliderScalarN()'");
#if _HAS_CXX20
			m_Result._Value = ImGui::SliderScalarN(
				label, 
				type,
				std::bit_cast<void*>(v),
				size,
				std::bit_cast<const void*>(&v_min), 
				std::bit_cast<const void*>(&v_max),
				format,
				flags
			);
#else
			m_Result._Value = ImGui::SliderScalarN(
				label, 
				type, 
				reinterpret_cast<void*>(v), size, 
				reinterpret_cast<const void*>(&v_min), 
				reinterpret_cast<const void*>(&v_max), 
				format,
				flags
			);
#endif
		}


		/// <summary>
		/// Calls 'ImGui::SliderAngle' 
		/// </summary>
		slider(
			angle, 
			const char* label,
			float* v_rad, 
			float v_degrees_min = -360.0f, 
			float v_degrees_max = +360.0f,
			const char* format = "%.0f deg", 
			ImGuiSliderFlags flags = 0
		) :
			scope_wrap(ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format, flags))
		{}

		/// <summary>
		/// Calls 'ImGui::SliderAngle' 
		/// </summary>
		slider(
			angle,
			const char* label,
			float& v_rad, 
			float v_degrees_min = -360.0f, 
			float v_degrees_max = +360.0f, 
			const char* format = "%.0f deg", 
			ImGuiSliderFlags flags = 0
		) :
			scope_wrap(ImGui::SliderAngle(label, &v_rad, v_degrees_min, v_degrees_max, format, flags))
		{}


		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		slider(
			array<_ViewSize> arr_size,
			const char* label, 
			std::array<_Ty, _Size>* v,
			_Ty v_min, 
			_Ty v_max,
			const char* format = default_c_format<_Ty>, 
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::SliderScalarN()'");
			static constexpr size_t size = _ViewSize ? _ViewSize : _Size;
#if _HAS_CXX20
			m_Result._Value = ImGui::SliderScalarN(
				label,
				type, 
				std::bit_cast<void*>(v->data()),
				size, 
				std::bit_cast<void*>(&v_min),
				std::bit_cast<void*>(&v_max), 
				format,
				flags
			);
#else
			m_Result._Value = ImGui::SliderScalarN(
				label, 
				type,
				reinterpret_cast<void*>(v->data()),
				size, 
				reinterpret_cast<const void*>(&v_min),
				reinterpret_cast<const void*>(&v_max), 
				format, 
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		slider(
			array<_ViewSize> arr_size,
			const char* label, 
			_Ty (&v)[_Size], 
			_Ty v_min, 
			_Ty v_max, 
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::SliderScalarN()'");
			static constexpr size_t size = _ViewSize ? _ViewSize : _Size;
#if _HAS_CXX20
			m_Result._Value = ImGui::SliderScalarN(label,
				type,
				std::bit_cast<void*>(static_cast<_Ty*>(v)), 
				size, 
				std::bit_cast<void*>(&v_min),
				std::bit_cast<void*>(&v_max),
				format, 
				flags
			);
#else
			m_Result._Value = ImGui::SliderScalarN(
				label, 
				type,
				reinterpret_cast<void*>(v), 
				size, 
				reinterpret_cast<const void*>(&v_min), 
				reinterpret_cast<const void*>(&v_max), 
				format, 
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		slider(
			array<_ViewSize> arr_size, 
			const char* label, 
			std::array<_Ty, _Size>& v,
			_Ty v_min, 
			_Ty v_max, 
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		) :
			slider(arr_size, label, &v, v_min, v_max, format, flags)
		{}


		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v', overload for 'ImVec2'
		/// </summary>
		template<size_t _ViewSize>
		slider(
			array<_ViewSize> arr_size,
			const char* label, 
			ImVec2* v, 
			float v_min, 
			float v_max, 
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
#if _HAS_CXX20
			slider(arr_size, label, std::bit_cast<std::array<float, 2>*>(&v), v_min, v_max, format, flags)
#else
			slider(arr_size, label, reinterpret_cast<std::array<float, 2>*>(&v), v_min, v_max, format, flags)
#endif
		{}


		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v', overload for 'ImVec2'
		/// </summary>
		template<size_t _ViewSize>
		slider(
			array<_ViewSize> arr_size, 
			const char* label, 
			ImVec2& v,
			float v_min, 
			float v_max,
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
			slider(arr_size, label, &v, v_min, v_max, format, flags)
		{}


		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v', overload for 'ImVec4'
		/// </summary>
		template<size_t _ViewSize>
		slider(
			array<_ViewSize> arr_size, 
			const char* label,
			ImVec4* v,
			float v_min, 
			float v_max, 
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
#if _HAS_CXX20
			slider(arr_size, label, std::bit_cast<std::array<float, 4>*>(&v), v_min, v_max, format, flags)
#else
			slider(arr_size, label, reinterpret_cast<std::array<float, 4>*>(&v), v_min, v_max, format, flags)
#endif
		{}

		/// <summary>
		/// Calls 'ImGui::SliderScalarN' with a smaller or equal to array size 'v', overload for 'ImVec4'
		/// </summary>
		template<size_t _ViewSize>
		slider(
			array<_ViewSize> arr_size,
			const char* label, 
			ImVec4& v, 
			float v_min, 
			float v_max, 
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
			slider(arr_size, label, &v, v_min, v_max, format, flags)
		{}

	private:
		template<typename _Ty>
		static constexpr ImGuiDataType get_type() noexcept
		{
			if constexpr (std::is_same_v<_Ty, char>)
				return ImGuiDataType_S8;
			else if constexpr (std::is_same_v<_Ty, uint8_t>)
				return ImGuiDataType_U8;
			else if constexpr (std::is_same_v<_Ty, int16_t>)
				return ImGuiDataType_S16;
			else if constexpr (std::is_same_v<_Ty, uint16_t>)
				return ImGuiDataType_U16;
			else if constexpr (std::is_same_v<_Ty, int32_t>)
				return ImGuiDataType_S32;
			else if constexpr (std::is_same_v<_Ty, uint32_t>)
				return ImGuiDataType_U32;
			else if constexpr (std::is_same_v<_Ty, int64_t>)
				return ImGuiDataType_S64;
			else if constexpr (std::is_same_v<_Ty, uint64_t>)
				return ImGuiDataType_U64;
			else if constexpr (std::is_same_v<_Ty, float>)
				return ImGuiDataType_Float;
			else if constexpr (std::is_same_v<_Ty, double>)
				return ImGuiDataType_Double;
			else return ImGuiDataType_COUNT;
		}
	};


	template<>
	constexpr const char* slider::default_c_format<char> = "%d";
	template<>
	constexpr const char* slider::default_c_format<uint8_t> = "%u";
	template<>
	constexpr const char* slider::default_c_format<int16_t> = "%d";
	template<>
	constexpr const char* slider::default_c_format<uint16_t> = "%u";
	template<>
	constexpr const char* slider::default_c_format<int32_t> = "%d";
	template<>
	constexpr const char* slider::default_c_format<uint32_t> = "%u";
#ifdef _MSC_VER
	template<>
	constexpr const char* slider::default_c_format<int64_t> = "%I64d";
	template<>
	constexpr const char* slider::default_c_format<uint64_t> = "%I64u";
#else
	template<>
	constexpr const char* slider::default_c_format<int64_t> = "%lld";
	template<>
	constexpr const char* slider::default_c_format<uint64_t> = "%llu";
#endif
	template<>
	constexpr const char* slider::default_c_format<float> = "%.3f";
	template<>
	constexpr const char* slider::default_c_format<double> = "%.6lf";
}