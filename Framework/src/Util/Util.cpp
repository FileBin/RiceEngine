#include "pch.h"
#include <codecvt>
#include <GameEngine/Util.h>
#include <GameEngine\Util3D.h>
#include <GameEngine\Math.h>
#include <stringapiset.h>
#include <GameEngine/Util/SmartPointer.h>



namespace Game {

	namespace Util {
		void ThrowIfFailed(HRESULT hr) {
			if (FAILED(hr))
				throw hr;
		}

		data_t ReadFile(String filename) {
			std::ifstream infile(filename, std::ios_base::binary);
			auto data = data_t(std::istreambuf_iterator<char>(infile),
				std::istreambuf_iterator<char>());
			return data;
		}

		String Utf8ToWstring(const std::string& str) {
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
			return wstrTo;
		}

		std::string ToStdString(const String& s) {
			std::string out{};
			for (int i = 0; s[i]; ++i) {
				wchar_t wc = s[i];
				char c = (char)wc;
				out.push_back(c);
			}
			return out;
		}

		Vector2 GetWindowScreenSize(HWND hwnd) {
			Vector2 size;
			RECT rc;
			GetClientRect(hwnd, &rc);
			size.x = rc.right - rc.left;
			size.y = rc.bottom - rc.top;
			return size;
		}
		D2D1::ColorF GetColByHex(UINT hex) {
			byte a = hex;
			byte b = hex >> 8;
			byte g = hex >> 16;
			byte r = hex >> 24;
			return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		}
	}
}