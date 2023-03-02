
#include <functional>
#include <utility>
#include <memory>
#pragma once
#ifndef _REST_H
#define _REST_H

namespace RESTAPI {

	using namespace std;
	typedef vector<char> DATA;


	inline std::vector<std::wstring> &split(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems) {
		std::wstringstream ss(s);
		std::wstring item;
		while (std::getline(ss, item, delim))
			{
			elems.push_back(item);
			}
		return elems;
		}


	inline std::vector<std::wstring> split(const std::wstring &s, wchar_t delim) {
		std::vector<std::wstring> elems;
		split(s, delim, elems);
		return elems;
		}

	inline std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}


	inline std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}


	class data_provider
		{
		public:
			
			virtual size_t s() = 0;
			virtual size_t Read(char* Buff, size_t sz) = 0;
			virtual bool CanOnce() = 0;
			virtual std::tuple<const char*, size_t> Once() = 0;

		};

	class memory_data_provider : public data_provider
		{
		private:
			size_t size;
			const char* ptr;
			size_t fp = 0;
		public:

			memory_data_provider(const char* a, size_t sz)
				{
				fp = 0;
				ptr = a;
				size = sz;
				}

			virtual size_t s()
				{
				return size;
				}

			virtual size_t Read(char* Buff, size_t sz)
				{
				size_t av = size - fp;
				if (av < sz)
					sz = av;
				memcpy(Buff, ptr + fp, sz);
				fp += sz;
				return sz;
				}

			virtual bool CanOnce()
				{
				return true;
				}
			virtual std::tuple<const char*, size_t> Once()
				{
				return make_tuple<const char*, size_t>((const char*&&)ptr, (size_t&&)size);
				}


		};

	class file_data_provider : public data_provider
		{
		private:
			HANDLE hY;
		public:

			file_data_provider(HANDLE h)
				{
				hY = h;
				}

			virtual size_t s()
				{
				LARGE_INTEGER li;
				GetFileSizeEx(hY, &li);
				return (size_t)li.QuadPart;
				}

			virtual size_t Read(char* Buff, size_t sz)
				{
				DWORD A = 0;
				if (sz > 4294967296LL)
					sz = 4*1024*1024;
				if (!ReadFile(hY, Buff, (DWORD)sz, &A, 0))
					return 0;
				return A;
				}


			virtual bool CanOnce()
				{
				return false;
				}
			virtual std::tuple<const char*, size_t> Once()
				{
				return make_tuple<const char*, size_t>(0,0);
				}
		};


	class data_writer
		{
		public:

			virtual DWORD Write(const char* Buff, DWORD sz) = 0;
			virtual size_t s() = 0;

		};

	class memory_data_writer : public data_writer
		{
		private:
			vector<char> p;
		public:

			virtual size_t s()
				{
				return p.size();
				}

			virtual DWORD Write(const char* Buff, DWORD sz)
				{
				auto s1 = p.size();
				p.resize(s1 + sz);
				memcpy(p.data() + s1, Buff, sz);
				return sz;
				}

			vector<char>& GetP() { return p; }

		};

	class file_data_writer : public data_writer
		{
		private:
			HANDLE hY;
		public:

			file_data_writer(HANDLE h)
				{
				hY = h;
				}

			virtual size_t s()
				{
				LARGE_INTEGER li;
				GetFileSizeEx(hY, &li);
				return (size_t)li.QuadPart;
				}

			virtual DWORD Write(const char* Buff, DWORD sz)
				{
				DWORD A = 0;
				if (!WriteFile(hY, Buff, sz, &A, 0))
					return 0;
				return A;
				}
		};



	class ihandle
		{
		private:
			HINTERNET hX = 0;
			std::shared_ptr<size_t> ptr = std::make_shared<size_t>();

		public:

			// Closing items
			void Close()
				{
				if (!ptr || ptr.use_count() > 1)
					{
					ptr.reset();
					return;
					}
				ptr.reset();
				if (hX != 0)
					InternetCloseHandle(hX);
				hX = 0;
				}

			ihandle()
				{
				hX = 0;
				}
			~ihandle()
				{
				Close();
				}
			ihandle(const ihandle& h)
				{
				Dup(h);
				}
			ihandle(ihandle&& h)
				{
				Move(std::forward<ihandle>(h));
				}
			ihandle(HINTERNET hY)
				{
				hX = hY;
				}
			ihandle& operator =(const ihandle& h)
				{
				Dup(h);
				return *this;
				}
			ihandle& operator =(ihandle&& h)
				{
				Move(std::forward<ihandle>(h));
				return *this;
				}

			void Dup(const ihandle& h)
				{
				Close();
				hX = h.hX;
				ptr = h.ptr;
				}
			void Move(ihandle&& h)
				{
				Close();
				hX = h.hX;
				ptr = h.ptr;
				h.ptr.reset();
				h.hX = 0;
				}
			operator HINTERNET() const
				{
				return hX;
				}


		};

	class REST
		{
		private:

			wstring Agent = L"REST";
			ihandle hI;
			ihandle hI2;
			wstring Host;
bool ssl = false;

		public:

			REST(const wchar_t* ag = 0)
				{
				if (ag)
					Agent = ag;
				}

			void Disconnect()
				{
				hI.Close();
				hI2.Close();
				}

			HRESULT Connect(const wchar_t* host, bool SSL = false, unsigned short Port = 0, DWORD flg = 0,const wchar_t* user= 0,const wchar_t* pass = 0)
				{
				if (_wcsicmp(Host.c_str(), host) != 0)
					Disconnect();
				Host = host;
				ssl = SSL;
				if (!hI)
					hI = InternetOpen(Agent.c_str(), INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
				if (!hI)
					return E_UNEXPECTED;
				if (!hI2)
					{
					if (SSL)
						hI2 = InternetConnect(hI, host, Port ? Port : INTERNET_DEFAULT_HTTPS_PORT, user, pass, INTERNET_SERVICE_HTTP, INTERNET_FLAG_SECURE | flg, 0);
					else
						hI2 = InternetConnect(hI, host, Port ? Port : INTERNET_DEFAULT_HTTP_PORT, user, pass, INTERNET_SERVICE_HTTP, flg, 0);
					}
				if (!hI2)
					return E_UNEXPECTED;
				InternetSetStatusCallback(hI2, 0);
				return S_OK;
				}

			ihandle RequestWithBuffer(const wchar_t* url, const wchar_t* verb = L"POST", std::initializer_list<wstring> hdrs = {}, const char * d = 0, size_t sz = 0, std::function<HRESULT(size_t sent, size_t tot, void*)> fx = nullptr, void* lp = 0, bool Once = true,DWORD ExtraSecurityFlags = 0)
				{
				memory_data_provider m(d, sz);
				return Request2(url, m, Once, verb, hdrs, fx, lp, 0, 0, 0, 0, ExtraSecurityFlags);
				}

			ihandle RequestWithFile(const wchar_t* url, const wchar_t* verb = L"POST", std::initializer_list<wstring> hdrs = {}, HANDLE hX = INVALID_HANDLE_VALUE, std::function<HRESULT(size_t sent, size_t tot, void*)> fx = nullptr, void* lp = 0, DWORD ExtraSecurityFlags = 0)
				{
				file_data_provider m(hX);
				return Request2(url, m, true, verb, hdrs, fx, lp,0,0,0,0,ExtraSecurityFlags);
				}

			ihandle Request2(const wchar_t* url, data_provider& dp, bool Once = true, const wchar_t* verb = L"POST", std::initializer_list<wstring> hdrs = {}, std::function<HRESULT(size_t sent, size_t tot, void*)> fx = nullptr, void* lp = 0, const char* extradata1 = 0, DWORD extradatasize1 = 0,const char* extradata2 = 0,DWORD extradatasize2 = 0,DWORD ExtraSecurityFlags = 0)
				{
				if (!url)
					return 0;
				wstring nurl;
				ihandle hI3;
				if (_wcsnicmp(url, L"http://", 7) == 0 || _wcsnicmp(url, L"https://", 8) == 0)
					{
					Disconnect();
					hI = InternetOpen(Agent.c_str(), INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
					if (_wcsicmp(verb,L"GET") == 0)
						{
						hI3 = InternetOpenUrl(hI, url, 0, 0, ssl ? INTERNET_FLAG_SECURE : 0, 0);
						return hI3;
						}
					else
						{
						URL_COMPONENTS u = { 0 };
						size_t ms = wcslen(url);
						u.dwStructSize = sizeof(u);

						unique_ptr<TCHAR> hn(new TCHAR[ms]);
						unique_ptr<TCHAR> un(new TCHAR[ms]);
						unique_ptr<TCHAR> pwd(new TCHAR[ms]);
						unique_ptr<TCHAR> dir(new TCHAR[ms]);
						unique_ptr<TCHAR> ex(new TCHAR[ms]);

						u.lpszHostName = hn.get();
						u.dwHostNameLength = (DWORD)ms;
						u.lpszUserName = un.get();
						u.dwUserNameLength = (DWORD)ms;
						u.lpszPassword = pwd.get();
						u.dwPasswordLength = (DWORD)ms;
						u.lpszUrlPath = dir.get();
						u.dwUrlPathLength = (DWORD)ms;
						u.lpszExtraInfo = ex.get();
						u.dwExtraInfoLength = (DWORD)ms;
						InternetCrackUrl(url, 0, 0, &u);

						nurl = u.lpszUrlPath;
						nurl += u.lpszExtraInfo;
						url = nurl.c_str();

						const TCHAR* acct[] = { L"*/*",0 };
						if (_wcsnicmp(url, L"http://", 7) == 0)
							Connect(u.lpszHostName, false);
						else
							Connect(u.lpszHostName, true);

						hI3 = HttpOpenRequest(hI2, verb, url, 0, 0, (LPCTSTR*)acct, ssl ? INTERNET_FLAG_SECURE : 0, 0);
						}
					}
				else
					{
					const TCHAR* acct[] = { L"*/*",0 };
					hI3 = HttpOpenRequest(hI2, verb, url, 0, 0, (LPCTSTR*)acct, ssl ? INTERNET_FLAG_SECURE : 0, 0);
					}
				
				if (!hI3)
					return 0;


				if (ExtraSecurityFlags)
					{
					DWORD dwFlags = 0;
					DWORD dwBuffLen = sizeof(dwFlags);
					InternetQueryOption(hI3, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
					dwFlags |= ExtraSecurityFlags;
					InternetSetOption(hI3, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
					}


				for (size_t i = 0; i < hdrs.size(); i++)
					{
					auto h = (hdrs.begin() + i);
					if (h->length())
						{
						BOOL fx2 = HttpAddRequestHeaders(hI3, (LPCWSTR)h->c_str(), (DWORD)-1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
						if (!fx2)
							continue;
						}
					}

				if (((Once && dp.CanOnce()) || dp.s() == 0) && !extradata1 && !extradata2)
					{
					auto o = dp.Once();
					BOOL ldr = HttpSendRequest(hI3, 0, 0, (LPVOID)get<0>(o),(DWORD) get<1>(o));
					if (!ldr)
						{
						if (GetLastError() == 12044)
							ldr = HttpSendRequest(hI3, 0, 0, (LPVOID)get<0>(o), (DWORD)get<1>(o));
						if (!ldr)
							return 0;
						}

					if (fx)
						fx(get<1>(o), get<1>(o),lp);
					return hI3;
					}

				BOOL ldr = HttpSendRequestEx(hI3, 0, 0, 0, 0);
				if (!ldr)
					return 0;
				auto tot = dp.s();
				size_t Sent = 0;
				vector<char> b(4096);
				DWORD a = 0;
				if (extradata1)
					InternetWriteFile(hI3, extradata1, extradatasize1, &a);
				for (;;)
					{
					memset(b.data(), 0, 4096);
					auto ri = dp.Read(b.data(), 4096);
					if (!ri)
						break;
					if (!InternetWriteFile(hI3, b.data(), (DWORD)ri, &a))
						{
						DWORD dwe = 0;
						wchar_t rep[1000] = { 0 };
						DWORD dwb = 1000;
						InternetGetLastResponseInfo(&dwe, rep, &dwb);
						return 0;
						}
					if (a != ri)
						return 0;
					Sent += ri;
					if (fx)
						{
						auto rst = fx(Sent, tot, lp);
						if (FAILED(rst))
							return 0;
						}
					}
				if (extradata2)
					InternetWriteFile(hI3, extradata2, extradatasize2, &a);
				HttpEndRequest(hI3, 0, 0, 0);
				return hI3;
				}

			long Headers(ihandle& hI3,std::map<wstring,wstring>& t)
				{
				vector<wchar_t> lpOutBuffer(10000);
				DWORD dwSize = 10000;
				if (!HttpQueryInfo(hI3, HTTP_QUERY_RAW_HEADERS_CRLF,
					(LPVOID)lpOutBuffer.data(), &dwSize, NULL))
					return E_FAIL;
				vector<wstring> str;
				wstring s = lpOutBuffer.data();
				split(s, '\n',str);
				for (auto& a : str)
					{
					vector<wstring> str2;
					wcscpy_s(lpOutBuffer.data(), 10000, a.c_str());
					wchar_t* a1 = wcschr(lpOutBuffer.data(), ':');
					if (!a1)
						continue;
					*a1 = 0;
					a1++;
					if (wcslen(a1) && a1[wcslen(a1) - 1] == '\r')
						a1[wcslen(a1) - 1] = 0;
					while (wcslen(a1) && *a1 == ' ')
						a1++;
					t[lpOutBuffer.data()] = a1;
					}

				dwSize = 10000;
				if (!HttpQueryInfo(hI3, HTTP_QUERY_STATUS_CODE,
					(LPVOID)lpOutBuffer.data(), &dwSize, NULL))
					return E_FAIL;
				return _wtoi(lpOutBuffer.data());
				}

			HRESULT ReadToFile(ihandle& hI3, HANDLE hX, std::function<HRESULT(unsigned long long, unsigned long long, void*)> fx = nullptr, void* lp = 0)
				{
				file_data_writer w(hX);
				return Read2(hI3, w, fx, lp);
				}

			HRESULT ReadToMemory(ihandle& hI3, vector<char>& m, std::function<HRESULT(unsigned long long, unsigned long long, void*)> fx = nullptr, void* lp = 0)
				{
				memory_data_writer w;
				auto e = Read2(hI3, w, fx, lp);
				m = w.GetP();
				return e;
				}

			HRESULT Read2(ihandle& hI3, data_writer& dw, std::function<HRESULT(unsigned long long, unsigned long long, void*)> fx = nullptr, void* lp = 0)
				{
				size_t Size = 0;
				unsigned long bfs = 10000;
				TCHAR ss[10000] = { 0 };
				if (!HttpQueryInfo(hI3, HTTP_QUERY_CONTENT_LENGTH, ss, &bfs, 0))
					Size = (size_t)-1;
				else
					Size = (size_t)_wtoi64(ss);
				BOOL ld = TRUE;

				unsigned long long TotalTransferred = 0;
				for (;;)
					{
					DWORD n;
					unique_ptr<char> Buff(new char[10100]);
					BOOL F = InternetReadFile(hI3, Buff.get(), 10000, &n);
					if (F == false && ld == TRUE)
						{
						ld = FALSE;
						InternetSetOption(hI, INTERNET_OPTION_HTTP_DECODING, (void*)&ld, sizeof(BOOL));
						F = InternetReadFile(hI3, Buff.get(), 10000, &n);
						}
					if (F == false)
						return E_FAIL;
					if (n == 0)
						break;

					TotalTransferred += n;

					if (dw.Write(Buff.get(), n) != n)
						return E_FAIL;
					if (fx)
						{
						auto rst = fx(TotalTransferred, Size, lp);
						if (FAILED(rst))
							return E_ABORT;
						}
					}
				return S_OK;
				}


			inline vector<char> datareturn(ihandle& r)
				{
				vector<char> out;
				ReadToMemory(r, out);
				return out;
				}

			inline string textreturn(ihandle& r)
				{
				vector<char> out;
				ReadToMemory(r, out);
				out.resize(out.size() + 1);
				char* p = (char*)out.data();
				return p;
				}


		};



}

#endif