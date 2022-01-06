#include "pch.h"
#include <codecvt>
#include <GameEngine/Util.h>
#include <GameEngine\Util3D.h>
#include <GameEngine\Math.h>
#include <stringapiset.h>

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
			/*std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.from_bytes(str);*/
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
			return wstrTo;
		}

		std::string ToStdString(String s) {
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
	}
}