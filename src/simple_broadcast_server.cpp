#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>
#include <format>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

class broadcast_server {
public:
	broadcast_server() {
		m_server.init_asio();

		m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
		m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
		m_server.set_message_handler(bind(&broadcast_server::on_message, this, ::_1, ::_2));
	}

	void on_open(connection_hdl hdl) {
		lock_guard<mutex> guard(m_connection_lock);
		//m_connections.insert(hdl);
	}

	void on_close(connection_hdl hdl) {
		lock_guard<mutex> guard(m_connection_lock);
		m_connections.erase(hdl);
	}

	void on_message(connection_hdl hdl, server::message_ptr msg) {
		if (msg->get_opcode() == websocketpp::frame::opcode::TEXT
			&& msg->get_payload() == "Register") {
			lock_guard<mutex> guard(m_connection_lock);
			m_connections.insert(hdl);
		}
	}

	void run(uint16_t port) {
		m_server.listen(port);
		m_server.start_accept();
		m_server.run();
	}
	void sendMessage(std::string msg) {
		lock_guard<mutex> guard(m_connection_lock);

		con_list::iterator it;
		for (it = m_connections.begin(); it != m_connections.end(); ++it) {
			m_server.send(*it, msg, websocketpp::frame::opcode::TEXT);
		}
	}

private:
	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

	server m_server;
	mutex m_connection_lock;
	con_list m_connections;
};

int main() {
	broadcast_server server;
	thread t1([&]() {
		uint64_t loop = 0;
		while (1) {
			Sleep(1000);
			std::string x = std::format("hhaa_{}\n", loop++);
			server.sendMessage(x);
			printf(x.data());
		}
		});

	server.run(9002);
}