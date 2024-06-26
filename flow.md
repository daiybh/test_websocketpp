
//registerCmd = { "cmd":"register", "key" : "frametotcp"};
//unregisterCmd = {"cmd":"unregister", "key" : "frametotcp"};

ws_server listen on 9002 and set a special Key for register and unregister message

ws_client  connect to "ws://127.0.0.1:9002"
ws_server  recive the connect  but don't push into a queue

ws_client need send a message "registerCmd" 
ws_server:
  recive the message 
  check the key
    push into a queue

module sendmessage call ws_server.sendMessage(message)
ws_server send this message to all clients in the queue


ws_client send a message "unregisterCmd"
ws_server:
  recive the message 
  check the key
    remove the client from the queue

so ws_server  can handle all mesage in on_message function