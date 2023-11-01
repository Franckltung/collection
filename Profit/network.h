#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <thread>
#include "logger.h"

// This class is used for networking
// Send messages to poe website, receive messages back

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

const std::string host = "www.pathofexile.com";
const std::string port = "80";
const int version = 11;

class network
{
public:
	network(logger& log);

	std::string send(const std::string& message, const std::string& target, http::verb method); // Send message to host with target as path

private:
	logger* debug;

};

