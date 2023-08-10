#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP> <PORT>\n";
        return 1;
    }

    std::string ipspace = "192.168.10.";
    std::string dest = argv[1];
    int port = std::stoi(argv[2]);
    int rate;
    std::cout << "datarate (must be same with server, Kbps): ";
    std::cin >> rate;

    int size = 5000; // in bytes. current = 1KB
    double delay = size * 8.0 / 1000.0 / rate;
    std::string destip = ipspace + dest;
    std::cout << "interval: " << delay << '\n';

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(port);
    destaddr.sin_addr.s_addr = inet_addr(destip.c_str());

    auto start_time = std::chrono::high_resolution_clock::now();
    int C = 0;
    try {
        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            double t = std::chrono::duration<double, std::milli>(now - start_time).count() / 1000.0;

            std::string msg = std::to_string(C) + " " + std::to_string(t);
            msg.resize(size, ' ');

            int ss = sendto(sock, msg.c_str(), size, 0, (struct sockaddr *)&destaddr, sizeof(destaddr));
            std::cout << C << " " << ss << '\r';
            std::cout.flush();
            C++;

            double tts = delay - (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start_time).count() / 1000.0 - t);
            if (tts > 0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(tts));
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }

    close(sock);
    return 0;
}

