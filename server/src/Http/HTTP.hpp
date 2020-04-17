#pragma once
#include <pch.hpp>

namespace HTTP
{
	const string LINE_END = "\r\n";
	enum class Method
	{
		GET,
		HEAD,
		POST,
		PUT,
		DEL,
		TRACE,
		OPTIONS,
		CONNECT,
		PATCH
	};
	enum class Version
	{
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0
	};
	template<typename T, typename R>
	std::map<R, T> create_reverse_map(R(*forward_func)(T), T first, T last)
	{
		using int_t = std::underlying_type_t<T>;
		std::map<R, T> m;
		auto insert = [forward_func, &m](int i) {
			auto t = T(i);
			auto r = forward_func(t);
			auto p = m.insert({ r, t });
			if (!p.second) {
				std::cerr << "Duplicate ignored:" << int_t(p.first->second) << "<=" << r << "=>" << i << '\n';
			}
		};

		for (int i = int_t(first); i <= int_t(last); ++i) {
			insert(i);
		}
		return m;
	}
	string to_string(Version version)
	{
		switch (version)
		{
		case Version::HTTP_1_0:
			return "HTTP/1.0";
		case Version::HTTP_1_1:
			return "HTTP/1.1";
		case Version::HTTP_2_0:
			return "HTTP/2.0";
		}
	}

	string to_string(Method method)
	{
		switch (method)
		{
		case Method::GET:
			return "GET";
		case Method::HEAD:
			return "HEAD";
		case Method::POST:
			return "POST";
		case Method::PUT:
			return "PUT";
		case Method::DEL:
			return "DELETE";
		case Method::TRACE:
			return "TRACE";
		case Method::OPTIONS:
			return "OPTIONS";
		case Method::CONNECT:
			return "CONNECT";
		case Method::PATCH:
			return "PATCH";
		}
	}

	Method method_from_string(const string& method)
	{
		string(*f)(Method) = to_string;
		static auto const m = create_reverse_map(f, Method::GET, Method::PATCH);
		auto it = m.find(method);
		return it == m.end() ? Method::GET : it->second;
	}

	Version version_from_string(const string& version)
	{
		string(*f)(Version) = to_string;
		static auto const m = create_reverse_map(f, Version::HTTP_1_0, Version::HTTP_2_0);
		auto it = m.find(version);
		return it == m.end() ? Version::HTTP_1_0 : it->second;
	}

	class Header
	{
		string key;
		string value;
	public:
		Header(const string& key, const string& value) noexcept : key(key), value(value)
		{
		}

		void set_value(const string& value) noexcept
		{
			this->value = value;
		}

		const string& get_key() const noexcept
		{
			return this->key;
		}

		const string& get_value() const noexcept
		{
			return this->value;
		}

		string serialize() const noexcept
		{
			string header;
			header += this->key;
			header += ": ";
			header += this->value;
			header += LINE_END;

			return header;
		}

		static Header deserialize(const string& header)
		{
			std::size_t  pos = header.find(':');
			if (pos == string::npos) {
				std::cout << "invalid header" << std::endl;;
				return Header("", "");
			}
			string key(header.substr(0, pos));
			toLower(key);

			std::size_t  first = header.find_first_not_of(" \t", pos + 1);
			std::size_t  last = header.find_last_not_of(" \t");
			string value(header.substr(first, last - first + 1));
			toLower(value);

			return Header(key, value);
		}
	};

	string GetValueFromHeader(std::vector<Header>& headers, const string & key)
	{
		for (auto& header : headers)
			if (header.get_key() == key)
				return header.get_value();
		return "";
	}

	class Request
	{
	private:
		Version version;
		Method method;
		string resource;
		std::vector<Header> headers;

	public:
		Request(Method method, const string& resource, const std::vector<Header>& headers, Version version = Version::HTTP_1_1) noexcept : version(version), method(method), resource(resource), headers(headers)
		{
		}

		std::vector<Header>& get_headers()
		{
			return this->headers;
		}

		string& get_resource()
		{
			return this->resource;
		}

		string serialize() const noexcept
		{
			string request;
			request += to_string(this->method);
			request += " ";
			request += this->resource;
			request += " ";
			request += to_string(this->version);
			request += LINE_END;

			for (auto& header : this->headers)
			{
				request += header.serialize();
			}

			request += LINE_END;
			return request;
		}

		static Request deserialize(const string& request)
		{
			std::vector<string> lines = split(request, string(LINE_END));

			if (lines.size() < 1)
			{
				throw std::runtime_error("HTTP Request ('" + string(request) + "') consisted of " + std::to_string(lines.size()) + " lines, should be >= 1.");
			}

			std::vector<string> segments = split(lines[0], " ");

			if (segments.size() != 3)
			{
				throw std::runtime_error("First line of HTTP request ('" + string(request) + "') consisted of " + std::to_string(segments.size()) + " space separated segments, should be 3.");
			}

			const Method method = method_from_string(segments[0]);
			const string resource = segments[1];
			const Version version = version_from_string(segments[2]);

			std::vector<Header> headers;

			for (std::size_t i = 1; i < lines.size(); i++)
			{
				if (lines[i].size() > 0)
					headers.push_back(Header::deserialize(lines[i]));
			}

			return Request(method, resource, headers, version);
		}
	};

	static string OK = "200";
	static string CREATED = "201";
	static string ACCEPTED = "202";
	static string NO_CONTENT = "203";
	static string MOVED = "301";
	static string REDIRECT = "307";
	static string BAD_REQUEST = "400";
	static string FORBIDDEN = "403";
	static string NOT_FOUND = "404";
	static string REQUEST_TIMEOUT = "408";
	static string INTERNAL_SERVER_ERROR = "500";
	static string BAD_GATEWAY = "502";
	static string SERVICE_UNAVAILABLE = "503";

	class Response
	{
		string responseCode;
		Version version;
		std::vector<Header> headers;
		string body;
	public:
		Response(int responseCode, const std::vector<Header>& headers, const string& body, Version version = Version::HTTP_2_0)
		{
			this->responseCode = std::to_string(responseCode);
			this->version = version;
			this->headers = headers;
			this->body = body;
		}
		Response(const string& responseCode, const std::vector<Header>& headers, const string& body, Version version = Version::HTTP_2_0)
		{
			this->responseCode = responseCode;
			this->version = version;
			this->headers = headers;
			this->body = body;
		}

		const string& get_response_code() const noexcept
		{
			return this->responseCode;
		}

		const string& get_body() const noexcept
		{
			return this->body;
		}

		const std::vector<Header> get_headers() const noexcept
		{
			return this->headers;
		}

		string serialize(bool auto_content = true) const noexcept
		{
			string request;
			request += to_string(this->version);
			request += " ";
			request += this->responseCode;
			request += LINE_END;

			for (auto& header : this->headers)
				request += header.serialize();
			if (auto_content)
				request += Header("Content-Length", std::to_string(body.size())).serialize();

			request += LINE_END;
			if (body.size() > 0)
				request += body;

			return request;
		}

		static Response deserialize(const string& response) noexcept
		{
			std::vector<string> segments = split(response, string(LINE_END) + string(LINE_END));

			string headerSegment = segments[0];
			segments.erase(segments.begin());

			string body = concat(segments);

			std::vector<string> headerLines = split(headerSegment, string(LINE_END));

			const string& responseCodeLine = headerLines[0];

			std::vector<string> responseCodeSegments = split(responseCodeLine, " ");

			Version version = version_from_string(responseCodeSegments[0]);

			headerLines.erase(headerLines.begin());

			std::vector<Header> headers;

			for (const string& line : headerLines)
				headers.push_back(Header::deserialize(line));

			return Response(responseCodeSegments[1], headers, body, version);
		}
	};
}