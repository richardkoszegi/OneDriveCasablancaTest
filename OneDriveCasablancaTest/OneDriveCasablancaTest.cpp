// OneDriveCasablancaTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char* argv[])
{
	char* clientId = "000000004818EA82";

	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(U("https://login.live.com"));

		// Build request URI and start the request.
		//uri_builder builder(U("/search"));
		uri_builder builder(U("/oauth20_authorize.srf"));
		builder.append_query(U("client_id"), clientId);
		builder.append_query(U("scope"), U("wl.signin wl.offline_access onedrive.readwrite"));
		builder.append_query(U("response_type"), U("code"));
		builder.append_query(U("display"), U("popup"));
		builder.append_query(U("redirect_uri"), U("https://login.live.com/oauth20_desktop.srf"));
		http_request request(methods::GET);
		request.headers().add(L"Authorization", L"bearer");
		request.set_request_uri(builder.to_uri());


		//return client.request(methods::GET, builder.to_string());
		return client.request(request);
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());

		std::cout << response.headers().has(L"authentication_token");

		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}
	std::cin.get();
	return 0;
}

