# netperf
UDP delay, jitter, loss checker (C++)



<COM1> - server

1. compile
   g++ -o ser2 server2.cpp -std=c++11 -lpthrea

2. start
   ./ser2 <IP> <P_No>   ex) ./ser2 10 5555
   (default set 192.168.10.<IP>)



<COM2> - client

1. complie
g++ -o cli2 client2.cpp -std=c++11 -lpthrea

2. start
   ./ser2 <IP> <P_No>   ex) ./ser2 10 5555
   (default set 192.168.10.<IP>)

3. set traffic
   <500> + enter
   (500 kbps)

<COM1> - server (for checking log)

1. sequence of msg] rx_time tx_time [throughput kbps] (loss rate/100)
   2] 1240 1239 [120.0 kbps] (0/100)
   
   : second msg, received at 1240 sec, sended at 1239 sec, throuput : 120.0 kbps, loss rate : 0%
   

