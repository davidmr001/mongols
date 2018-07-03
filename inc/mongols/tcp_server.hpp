#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP


#include <netinet/in.h>   
#include <string>
#include <utility>
#include <unordered_map>
#include <mutex>


#include "thead_pool.hpp"
#include "epoll.hpp"

#define CLOSE_CONNECTION true
#define KEEPALIVE_CONNECTION false

namespace mongols {

    class tcp_server {
    public:
        typedef std::function<bool(const std::pair<size_t, size_t>&) > filter_handler_function;
        typedef std::function<std::pair<std::string, bool>(
                const std::string&
                , bool&
                , std::pair<size_t, size_t>&
                , filter_handler_function&) > handler_function;

    public:
        tcp_server() = delete;
        tcp_server(const std::string& host, int port, int timeout = 5000
                , size_t buffer_size = 1024
                , size_t thread_size = 0
                , int max_event_size = 64);
        virtual~tcp_server() = default;


    public:
        void run(const handler_function&);

        size_t get_buffer_size()const {
            return this->buffer_size;
        }
    private:
        mongols::epoll epoll;
        std::string host;
        int port, listenfd, timeout;
        struct sockaddr_in serveraddr;
        size_t buffer_size;
    private:
        std::unordered_map<int, std::pair<size_t, size_t>> clients;
#ifdef MONGOLS_USE_MULTITHREAD
        std::mutex main_mtx;
        mongols::thread_pool work_pool;
#endif     
        static bool done;
        static void signal_normal_cb(int sig);

    private:
        void setnonblocking(int fd);
        void add_client(int);
        void del_client(int);
        bool send_to_all_client(int, const std::string&, const filter_handler_function&);
        bool work(int, const handler_function&);
        void main_loop(struct epoll_event *, const handler_function&);
    };
}


#endif /* TCP_SERVER_HPP */

