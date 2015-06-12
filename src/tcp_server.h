int run_tcp_server(const char* requested_host,
                   const char* requested_port,
                   void(*handle_accepted)(int));

