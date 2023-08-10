#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath>
#include <iomanip>

int main(int argc, char* argv[]) {
    std::ostringstream formattedOutput;
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP> <PORT>\n";
        return 1;
    }

    std::string ipspace = "192.168.10.";
    std::string dest = argv[1];
    int port = std::stoi(argv[2]);
    std::vector<std::tuple<std::string, int, double, double>> l;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);
    myaddr.sin_addr.s_addr = inet_addr((ipspace + dest).c_str());

    if (bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        std::cerr << "Binding failed.\n";
        return 1;
    }

    const int size = 5000;
    char data[size];
    auto start_time = std::chrono::high_resolution_clock::now();
    try {
        while (l.size() < 100000) {
            sockaddr_in addr;
            socklen_t addrlen = sizeof(addr);
            int rs = recvfrom(sock, data, size, MSG_WAITALL, (struct sockaddr *)&addr, &addrlen);
            auto now = std::chrono::high_resolution_clock::now();
            double t = std::chrono::duration<double, std::milli>(now - start_time).count() / 1000.0;
            rs *= 8;

            std::string receivedData(data, rs / 8);
            std::stringstream ss(receivedData);
            std::string c, rt_str;
            ss >> c >> rt_str;
            double rt = std::stod(rt_str);

            l.push_back(std::make_tuple(c, rs, t, rt));

            double dr = 0.0;
            if (l.size() > 100) {
                double interval = std::get<2>(l.back()) - std::get<2>(l[l.size() - 101]);
                dr = std::round(size * 100 / interval * 8 / 1000);//, 5);
            }

            std::string losses = "--";
            if (l.size() > 100) {
                std::vector<int> seqs;
                for (size_t i = l.size() - 100; i < l.size(); ++i) {
                    seqs.push_back(std::stoi(std::get<0>(l[i])));
                }
                std::vector<int> diffs;
                for (size_t i = 0; i < seqs.size() - 1; ++i) {
                    diffs.push_back(seqs[i + 1] - seqs[i]);
                }
                int loss_count = 0;
                for (int d : diffs) {
                    if (d > 1) loss_count++;
                }
                losses = std::to_string(loss_count);
            }

            formattedOutput << c << "] " << t << " " << rt << " ["
                << std::fixed << std::setprecision(5) << dr << " kbps] (" << losses << "/100\n";
            std::cout << formattedOutput.str() << '\r';
            std::cout.flush();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }

    close(sock);
    std::ofstream file("congestion.txt");
    for (const auto &d : l) {
        file << std::get<0>(d) << '\t' << std::get<1>(d) << '\t' << std::get<2>(d) << '\t' << std::get<3>(d) << '\n';
    }
    file.close();

    return 0;
}
