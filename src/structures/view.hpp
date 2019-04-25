#pragma once


#include <cstdint>
#include <string_view>
#include <string>
#include <variant>
#include <iostream>


namespace lir {

	struct View {
		const char* ptr;
		const char* end;


		View()
			: ptr(nullptr), end(nullptr) {}

		View(const std::string& str)
			: ptr(str.c_str()), end(ptr + str.size()) {}

		View(const char* s)
			: ptr(s), end(ptr + 1) {}

		View(const char* s, const char* p)
			: ptr(s), end(p) {}



		const char* get() const {
			return ptr;
		}



		char operator*() const {
			return *ptr;
		}





		View& operator++() {
			++ptr;
			return *this;
		}


		View& operator--() {
			--ptr;
			return *this;
		}





		template <typename T>
		const char* operator+(const T& x) const {
			return ptr + x;
		}


		template <typename T>
		const char* operator-(const T& x) const {
			return ptr - x;
		}





		bool remaining() const {
			return ptr != end;
		}
	};
}





std::ostream& operator<<(std::ostream& os, const lir::View& v) {
	auto diff = static_cast<std::string_view::size_type>(v.end - v.ptr);

	std::string_view view{v.ptr, diff};

	return (os << view);
}
