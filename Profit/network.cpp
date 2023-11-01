#include "network.h"

network::network(logger& log)
{
	debug = &log;
}

std::string network::send(const std::string& message, const std::string& target, http::verb method) {
	try {
		std::stringstream debugMsg;

		net::io_context ioc;
		tcp::resolver resolver{ ioc };
		tcp::socket socket{ ioc };

		auto const resolved = resolver.resolve(host, port);

		net::connect(socket, resolved.begin(), resolved.end());

		// Create request
		http::request<http::string_body> req{ static_cast<http::verb>(method), target, version };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		if (method == http::verb::post) {
			req.body() = message;
			req.set(http::field::content_type, "application/json");
			req.set(http::field::content_length, req.body().length());
		}
		else if (method == http::verb::get) {
			// Do nothing
		}

		// Send on its way
		//debugMsg << "Sending: " << req;
		//debug->print(debugMsg.str());
		http::write(socket, req);

		// Containers for response
		beast::flat_buffer buffer;
		http::response<http::string_body> res;

		// Read response
		http::read(socket, buffer, res);
		//debugMsg << "Received: " << res;
		//debug->print(debugMsg.str());

		// Shutdown
		beast::error_code ec;
		socket.shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		return res.body();
	}
	catch(std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return "err";
	}
}
