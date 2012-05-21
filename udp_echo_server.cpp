//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;

class AsyncUDP
{
public:
    AsyncUDP(const std::string & host, short port)
        : m_ioService(),
          m_socket(m_ioService, udp::endpoint(udp::v4(), port)),
          m_senderEndpoint(udp::v4(),port),
          m_destinationEndpoint(udp::v4(),port),
          m_backgroundThread()
    {
        m_ioService.post(boost::bind(&AsyncUDP::read,this));
        boost::thread t(boost::bind(&boost::asio::io_service::run, &m_ioService));
        m_backgroundThread.swap(t);
    }

    void write(const char * buf, const size_t len) {
        m_socket.async_send_to(
            boost::asio::buffer(buf, len), m_destinationEndpoint,
            boost::bind(&AsyncUDP::handleSend, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void write(const std::string & str) {
        const char * buf = str.c_str();
        size_t len = str.length();
        write(buf,len);
    }

    void close() {
    }

private:

    void handleReceive(const boost::system::error_code& error,
                             size_t bytes_recvd)
    {
        if (!error && bytes_recvd > 0)
        {
            std::cout << "received:  " << m_readBuffer << std::endl;
        }
        else
        {
            read();
        }
    }

    void handleSend(const boost::system::error_code& error,
                        size_t bytes_sent)
    {
        //std::cout << "handle send" << std::endl;
        read();
    }

    void read() {
        m_socket.async_receive_from(
            boost::asio::buffer(m_readBuffer, m_maxLength), m_senderEndpoint,
            boost::bind(&AsyncUDP::handleReceive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    boost::asio::io_service m_ioService;
    udp::socket m_socket;
    udp::endpoint m_senderEndpoint;
    udp::endpoint m_destinationEndpoint;
    boost::thread m_backgroundThread;
    static const uint16_t m_maxLength = 1024;
    char m_readBuffer[m_maxLength];
    boost::mutex m_readMutex;
    boost::mutex m_writeMutex;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
            return 1;
        }

        using namespace std; // For atoi.
        AsyncUDP s(argv[1], atoi(argv[2]));

        while(1) {
            char helloBuf[] = "buf write test";
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            s.write("string write test");
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            s.write(helloBuf,sizeof(helloBuf));
        };

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
