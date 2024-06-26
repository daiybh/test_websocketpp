#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>
#include <format>
typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::condition_variable;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::thread;
using websocketpp::lib::unique_lock;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class broadcast_server
{
public:
	broadcast_server()
	{
		m_server.init_asio();

		m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
		m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
		m_server.set_message_handler(bind(&broadcast_server::on_message, this, ::_1, ::_2));
	}

	void on_open(connection_hdl hdl)
	{
		lock_guard<mutex> guard(m_connection_lock);
		// m_connections.insert(hdl);
	}

	void on_close(connection_hdl hdl)
	{
		lock_guard<mutex> guard(m_connection_lock);
		m_connections.erase(hdl);
	}

	
	std::string extractValue(const std::string& json, const std::string& key) {
		std::string searchKey = "\"" + key + "\"";
		std::size_t startPos = json.find(searchKey);
		if (startPos == std::string::npos) {
			return "";
		}
		startPos = json.find(":", startPos);
		if (startPos == std::string::npos) {
			return "";
		}
		startPos = json.find_first_of("\"", startPos);
		if (startPos == std::string::npos) {
			return "";
		}
		startPos++;
		std::size_t endPos = json.find_first_of("\"", startPos);
		if (endPos == std::string::npos) {
			return "";
		}
		return json.substr(startPos, endPos - startPos);
	}
	//@ret 0:nothing 1:register 2:unregister 3 unregister ALL
	int handleCmd(const std::string &_msg)
	{
		// registerCmd = { "cmd":"register", "key" : "frametotcp"};
		// unregisterCmd = {"cmd":"unregister", "key" : "frametotcp"};
		std::string msg_lower = _msg;
		std::transform(_msg.begin(), _msg.end(), msg_lower.begin(), ::tolower);
		std::string key, cmd;
		cmd = extractValue(msg_lower, "cmd");
		key = extractValue(msg_lower, "key");
		if (cmd == "register")
		{
			if (key == m_moduleKey)
			{
				std::cout << "register frametotcp" << std::endl;
				return 1;
			}
		}
		else if (cmd == "unregister")
		{
			if (key == m_moduleKey)
			{
				std::cout << "unregister frametotcp" << std::endl;
				return 2;
			}
		}
		else if (cmd == "unregisterall")
		{
			if (key == m_moduleKey)
			{
				std::cout << "unregister frametotcp" << std::endl;
				return 3;
			}
		}
		return 0;
	}
	// 关闭特定连接
	void close(connection_hdl hdl)
	{
		websocketpp::lib::error_code ec;
		m_server.close(hdl, websocketpp::close::status::going_away, "Server shutting down", ec);
		if (ec)
		{
			std::cout << "Error closing connection: " << ec.message() << std::endl;
		}
	}
	void on_message(connection_hdl hdl, server::message_ptr msg)
	{
		if (msg->get_opcode() == websocketpp::frame::opcode::TEXT)
		{
			int ret = handleCmd(msg->get_payload());
			if (ret == 1)
			{
				lock_guard<mutex> guard(m_connection_lock);
				m_connections.insert(hdl);
				return;
			}
			else if (ret == 2)
			{
				lock_guard<mutex> guard(m_connection_lock);
				m_connections.erase(hdl);
				return;
			}
			else if (ret == 3)
			{
				lock_guard<mutex> guard(m_connection_lock);
				m_connections.clear();
				return;
			}
			std::cout << "UnAccept connect " << msg->get_payload() << std::endl;
			close(hdl);
		}
	}

	void run(uint16_t port, std::string moduleKey)
	{
		m_moduleKey = moduleKey;
		std::transform(moduleKey.begin(), moduleKey.end(), m_moduleKey.begin(), ::tolower);
		m_server.listen(port);
		m_server.start_accept();
		m_server.run();
	}
	void sendMessage(std::string msg)
	{
		lock_guard<mutex> guard(m_connection_lock);

		con_list::iterator it;
		for (it = m_connections.begin(); it != m_connections.end(); ++it)
		{
			m_server.send(*it, msg, websocketpp::frame::opcode::TEXT);
		}
	}

private:
	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

	server m_server;
	mutex m_connection_lock;
	con_list m_connections;
	std::string m_moduleKey;
};

int main()
{
	broadcast_server server;
	thread t1([&]()
			  {
		uint64_t loop = 0;
		while (1) {
			Sleep(1000);
			std::string x = std::format("hhaa_{}\n", loop++);
			server.sendMessage(x);
			printf(x.data());
		} });
	thread t2([&]()
			  { server.run(9002, "frametotcp"); });
	t1.join();
	t2.join();
	return 0;
}