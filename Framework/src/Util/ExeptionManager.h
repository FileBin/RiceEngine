#ifdef _DEBUG
catch (nullptr_t){

}
#else
catch (Exception& e) {
	Log::log(Log::ERR, L"Not handled exception occured: {}", Util::Utf8ToWstring(typeid(e).name()));
	Log::log(Log::ERR, L"Line {}", std::to_wstring(e.GetLine()));
	Log::log(Log::ERR, L"File: {}", Util::Utf8ToWstring(e.GetFile()));
	Log::log(Log::ERR, L"What: {}", Util::Utf8ToWstring(e.what()));
	Log::log(Log::ERR, L"AdditionalInfo: {}\n", Util::Utf8ToWstring(e.GetInfo()));
	Log::log(Log::ERR, L"Stack trace:\n{}", Util::Utf8ToWstring(e.GetStack()));
	Log::Close();
	throw e;
} catch (std::exception& e) {
	Log::log(Log::ERR, L"std::exception occured: {}", Util::Utf8ToWstring(typeid(e).name()));
	Log::log(Log::ERR, L"What: {}", Util::Utf8ToWstring(e.what()));
	Log::Close();
	throw e;
} catch (HRESULT hr) {
	_com_error err(hr);
	Log::log(Log::ERR, L"HRESULT Error occured: {}", hr);
	Log::log(Log::ERR, L"Message: {}", err.ErrorMessage());
	Log::Close();
	throw hr;
}
#endif