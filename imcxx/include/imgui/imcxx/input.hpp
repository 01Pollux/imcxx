#pragma once

#include <type_traits>
#include <numeric>
#include "scopes.hpp"

namespace imcxx
{
	/// <summary>
	/// Widgets: Input with Keyboard
	/// - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
	/// - Most of the ImGuiInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
	/// </summary>
	class input : public scope_wrap<input, scope_traits::no_dtor>
	{
		struct input_reforward_data
		{
			std::string* Str;
			ImGuiInputTextCallback  ChainCallback;
			void* ChainCallbackUserData;
		};

	public:
		struct text {};
		struct multiline {};
		struct hint {};

		struct pointer {};
		template<size_t _Size>
		struct array
		{
			static constexpr size_t size = _Size;
		};
		using auto_array = array<0>;

		template<typename T>
		static constexpr const char* default_c_format = "";

		/// <summary>
		/// Calls 'ImGui::InputScalar'
		/// </summary>
		template<typename _Ty>
		input(
			const char* label, 
			_Ty* v, 
			_Ty v_slow_step = {}, 
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>, 
			ImGuiInputTextFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::InputScalar()'");
#if _HAS_CXX20
			m_Result._Value = ImGui::InputScalar(
				label, 
				type,
				std::bit_cast<void*>(v),
				v_slow_step > _Ty{ } ? std::bit_cast<const void*>(&v_slow_step) : nullptr,
				v_fast_step > _Ty{ } ? std::bit_cast<const void*>(&v_fast_step) : nullptr,
				format,
				flags
			);
#else
			m_Result._Value = ImGui::InputScalar(
				label, 
				type, 
				reinterpret_cast<void*>(v), 
				v_slow_step > _Ty{ } ? reinterpret_cast<const void*>(&v_slow_step) : nullptr,
				v_fast_step > _Ty{ } ? reinterpret_cast<const void*>(&v_fast_step) : nullptr,
				format,
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::InputScalar'
		/// </summary>
		template<typename _Ty, typename = std::enable_if_t<!std::is_array_v<_Ty>>>
		input(
			const char* label,
			_Ty& v, 
			_Ty v_slow_step = {}, 
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>, 
			ImGuiInputTextFlags flags = 0
		) :
			input(label, &v, v_slow_step, v_fast_step, format, flags)
		{}


		/// <summary>
		/// ImGui::InputScalarN
		/// </summary>
		template<typename _Ty>
		input(
			pointer, 
			const char* label, 
			_Ty* v, size_t size,
			_Ty v_slow_step = {},
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>,
			ImGuiInputTextFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::InputScalarN()'");
#if _HAS_CXX20
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				std::bit_cast<void*>(v), 
				size, 
				v_slow_step > _Ty{ 0 } ? std::bit_cast<const void*>(&v_slow_step) : nullptr,
				v_fast_step > _Ty{ 0 } ? std::bit_cast<const void*>(&v_fast_step) : nullptr,
				format,
				flags
			);
#else
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				reinterpret_cast<void*>(v),
				size, 
				v_slow_step > _Ty{ 0 } ? reinterpret_cast<const void*>(&v_slow_step) : nullptr,
				v_fast_step > _Ty{ 0 } ? reinterpret_cast<const void*>(&v_fast_step) : nullptr,
				format,
				flags
			);
#endif
		}


		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			std::array<_Ty, _Size>* v,
			_Ty v_slow_step = {},
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::InputScalarN()'");
			static constexpr size_t size = _ViewSize ? _ViewSize : _Size;
#if _HAS_CXX20
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				std::bit_cast<void*>(v->data()),
				size,
				std::bit_cast<void*>(&v_slow_step),
				std::bit_cast<void*>(&v_fast_step),
				format,
				flags
			);
#else
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				reinterpret_cast<void*>(v->data()),
				size,
				reinterpret_cast<const void*>(&v_slow_step),
				reinterpret_cast<const void*>(&v_fast_step),
				format,
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			_Ty(&v)[_Size],
			_Ty v_slow_step = {},
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		)
		{
			static constexpr ImGuiDataType type = get_type<_Ty>();
			static_assert(type != ImGuiDataType_COUNT, "Invalid type was passed to 'ImGui::InputScalarN()'");
			static constexpr size_t size = _ViewSize ? _ViewSize : _Size;
#if _HAS_CXX20
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				std::bit_cast<void*>(static_cast<_Ty*>(v)),
				size,
				std::bit_cast<void*>(&v_slow_step),
				std::bit_cast<void*>(&v_fast_step),
				format,
				flags
			);
#else
			m_Result._Value = ImGui::InputScalarN(
				label,
				type,
				reinterpret_cast<void*>(v),
				size,
				reinterpret_cast<const void*>(&v_slow_step),
				reinterpret_cast<const void*>(&v_fast_step),
				format,
				flags
			);
#endif
		}

		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v'
		/// </summary>
		template<size_t _ViewSize, typename _Ty, size_t _Size>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			std::array<_Ty, _Size>& v,
			float speed = 1.f,
			_Ty v_slow_step = {},
			_Ty v_fast_step = {},
			const char* format = default_c_format<_Ty>,
			ImGuiSliderFlags flags = 0
		) :
			input(arr_size, label, &v, speed, v_slow_step, v_fast_step, format, flags)
		{}


		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v', overload for 'ImVec2'
		/// </summary>
		template<size_t _ViewSize>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			ImVec2* v,
			float v_slow_step = {},
			float v_fast_step = {},
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
#if _HAS_CXX20
			input(arr_size, label, std::bit_cast<std::array<float, 2>*>(&v), v_slow_step, v_fast_step, format, flags)
#else
			input(arr_size, label, reinterpret_cast<std::array<float, 2>*>(&v), v_slow_step, v_fast_step, format, flags)
#endif
		{}


		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v', overload for 'ImVec2'
		/// </summary>
		template<size_t _ViewSize>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			ImVec2& v,
			float v_slow_step = {},
			float v_fast_step = {},
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
			input(arr_size, label, &v, v_slow_step, v_fast_step, format, flags)
		{}


		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v', overload for 'ImVec4'
		/// </summary>
		template<size_t _ViewSize>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			ImVec4* v,
			float v_slow_step = {},
			float v_fast_step = {},
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
#if _HAS_CXX20
			input(arr_size, label, std::bit_cast<std::array<float, 4>*>(&v), v_slow_step, v_fast_step, format, flags)
#else
			input(arr_size, label, reinterpret_cast<std::array<float, 4>*>(&v), v_slow_step, v_fast_step, format, flags)
#endif
		{}

		/// <summary>
		/// Calls 'ImGui::InputScalarN' with a smaller or equal to array size 'v', overload for 'ImVec4'
		/// </summary>
		template<size_t _ViewSize>
		input(
			array<_ViewSize> arr_size,
			const char* label,
			ImVec4& v,
			float v_slow_step = {},
			float v_fast_step = {},
			const char* format = "%.3f",
			ImGuiSliderFlags flags = 0
		) :
			input(arr_size, label, &v, v_slow_step, v_fast_step, format, flags)
		{}


		/// <summary>
		/// ImGui::InputText
		/// </summary>
		template<typename _LabelTy, typename _InputTy>
		input(
			text,
			const _LabelTy& label,
			_InputTy& input, 
			ImGuiInputTextFlags flags = 0,
			ImGuiInputTextCallback callback = nullptr,
			void* user_data = nullptr
		)
		{
			const char* label_str = extract(label);
			if constexpr (std::is_same_v<std::remove_pointer_t<std::decay_t<_InputTy>>, std::string>)
			{
				std::string* p_input = nullptr;
				if constexpr (std::is_pointer_v<_InputTy>)
					p_input = input;
				else
					p_input = &input;

				input_reforward_data data
				{
					p_input,
					callback,
					user_data
				};
				m_Result._Value = ImGui::InputText(label_str, p_input->data(), p_input->capacity() + 1, flags, &input_text_reforward, &data);
			}
			else
			{
				static_assert(std::is_same_v<char*, std::decay_t<_InputTy>>, "Intput type is not a pointer to characters");
				m_Result._Value = ImGui::InputText(label_str, input, std::extent_v< _InputTy>, flags, callback, user_data);
			}
		}

		template<typename _LabelTy>
		input(
			text,
			const _LabelTy& label,
			char* input, 
			size_t input_size, 
			ImGuiInputTextFlags flags = 0,
			ImGuiInputTextCallback callback = nullptr, 
			void* user_data = nullptr
		) :
			scope_wrap(ImGui::InputText(extract(label), input, input_size, flags, callback, user_data))
		{}


		/// <summary>
		/// ImGui::InputTextMultiline
		/// </summary>
		template<typename _LabelTy, typename _InputTy>
		input(
			multiline,
			const _LabelTy& label, 
			_InputTy& input,
			ImVec2 size = {}, 
			ImGuiInputTextFlags flags = 0, 
			ImGuiInputTextCallback callback = nullptr,
			void* user_data = nullptr
		)
		{
			const char* label_str = extract(label);
			if constexpr (std::is_same_v<std::remove_pointer_t<std::decay_t<_InputTy>>, std::string>)
			{
				std::string* p_input = nullptr;
				if constexpr (std::is_pointer_v<_InputTy>)
					p_input = input;
				else
					p_input = &input;

				input_reforward_data data
				{
					p_input,
					callback,
					user_data
				};
				m_Result._Value = ImGui::InputTextMultiline(label_str, input, size, p_input->data(), p_input->capacity() + 1, size, flags, &input_text_reforward, &data);
			}
			else
			{
				static_assert(std::is_same_v<char*, std::decay_t<_InputTy>>, "Intput type is not a pointer to characters");
				m_Result._Value = ImGui::InputTextMultiline(label_str, input, std::extent_v<_InputTy>, size, flags, callback, user_data);
			}
		}

		template<typename _LabelTy>
		input(
			multiline, 
			const _LabelTy& label, 
			char* input, 
			size_t input_size, 
			ImVec2 size = {}, 
			ImGuiInputTextFlags flags = 0, 
			ImGuiInputTextCallback callback = nullptr, 
			void* user_data = nullptr) :
			scope_wrap(ImGui::InputTextMultiline(extract(label), input, input_size, size, flags, callback, user_data))
		{}


		/// <summary>
		/// ImGui::InputTextWithHint
		/// </summary>
		template<typename _LabelTy, typename _HintTy, typename _InputTy>
		input(
			hint, 
			const _LabelTy& label, 
			const _HintTy& hint,
			_InputTy& input, 
			ImGuiInputTextFlags flags = 0, 
			ImGuiInputTextCallback callback = nullptr, 
			void* user_data = nullptr)
		{
			const char* hint_str = extract(hint);
			const char* label_str = extract(label);

			if constexpr (std::is_same_v<std::remove_pointer_t<std::decay_t<_InputTy>>, std::string>)
			{
				std::string* p_input = nullptr;
				if constexpr (std::is_pointer_v<_InputTy>)
					p_input = input;
				else
					p_input = &input;

				input_reforward_data data
				{
					p_input,
					callback,
					user_data
				};
				m_Result._Value = ImGui::InputTextWithHint(label_str, hint_str, p_input->data(), p_input->capacity() + 1, flags, &input_text_reforward, &data);
			}
			else
			{
				static_assert(std::is_same_v<char*, std::decay_t<_InputTy>>, "Intput type is not a pointer to characters");
				m_Result._Value = ImGui::InputTextWithHint(label_str, hint_str, input, std::extent_v<_InputTy>, flags, callback, user_data);
			}
		}

		template<typename _LabelTy, typename _HintTy>
		input(
			hint,
			const _LabelTy& label, 
			const _HintTy& hint, 
			char* input,
			size_t input_size, 
			ImGuiInputTextFlags flags = 0,
			ImGuiInputTextCallback callback = nullptr, 
			void* user_data = nullptr) :
			scope_wrap(ImGui::InputTextWithHint(extract(label), extract(hint), input, input_size, flags, callback, user_data))
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

		static constexpr int input_text_reforward(ImGuiInputTextCallbackData* data)
		{
			input_reforward_data* user_data = static_cast<input_reforward_data*>(data->UserData);
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				// Resize string callback
				// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
				std::string* str = user_data->Str;
				IM_ASSERT(data->Buf == str->c_str());
				str->resize(data->BufTextLen);
				data->Buf = str->data();
			}
			else if (user_data->ChainCallback)
			{
				// Forward to user callback, if any
				data->UserData = user_data->ChainCallbackUserData;
				return user_data->ChainCallback(data);
			}
			return 0;
		}

		template<typename _Ty>
		static [[nodiscard]] const char* extract(const _Ty& str) noexcept
		{
			const char* actual_str;
			if constexpr (std::is_same_v<char*, std::decay_t<_Ty>> || std::is_same_v<const char*, std::decay_t<_Ty>>)
				actual_str = str;
			else
			{
				if constexpr (std::is_pointer_v<_Ty>)
					actual_str = str->data();
				else
					actual_str = str.data();
			}
			return actual_str;
		}
	};


	template<>
	constexpr const char* input::default_c_format<char> = "%d";
	template<>
	constexpr const char* input::default_c_format<uint8_t> = "%u";
	template<>
	constexpr const char* input::default_c_format<int16_t> = "%d";
	template<>
	constexpr const char* input::default_c_format<uint16_t> = "%u";
	template<>
	constexpr const char* input::default_c_format<int32_t> = "%d";
	template<>
	constexpr const char* input::default_c_format<uint32_t> = "%u";
#ifdef _MSC_VER
	template<>
	constexpr const char* input::default_c_format<int64_t> = "%I64d";
	template<>
	constexpr const char* input::default_c_format<uint64_t> = "%I64u";
#else
	template<>
	constexpr const char* input::default_c_format<int64_t> = "%lld";
	template<>
	constexpr const char* input::default_c_format<uint64_t> = "%llu";
#endif
	template<>
	constexpr const char* input::default_c_format<float> = "%.3f";
	template<>
	constexpr const char* input::default_c_format<double> = "%.6lf";
}