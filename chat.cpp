/* 
 * Name			: chat.cpp
 * Auther		: sameragi
 * Copyright	: Copyright sameragi 2015
 * License		: 
 */

#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define log_file_name "log"
//#define other_ip "192.168.1.26"
#define other_ip "127.0.0.1"

using namespace std;

mutex mtx1, mtx2;


class receiver {
	int re_sock = 0;
	int re_new_sock = 0;
	sockaddr_in read_addr;
	sockaddr_in write_addr;
	socklen_t write_addr_len = sizeof(write_addr);
public:
	receiver(int sock, sockaddr_in &addr) {

		re_sock = sock;
		read_addr = addr;
		inet_aton("INADDR_ANY", &read_addr.sin_addr);

		bind(re_sock, (sockaddr *)&read_addr, sizeof(read_addr));
		listen(re_sock, 1);
		//re_new_sock = accept(re_sock, (sockaddr *)&write_addr, &write_addr_len);
	}

	void read_message() {
		re_new_sock = accept(re_sock, (sockaddr *)&write_addr, &write_addr_len);

	//void operator() () {
		//while(1) {
			char re_message[32];
			mtx2.lock();
			recv(re_new_sock, re_message, sizeof(re_message), 0);
			mtx2.unlock();
			cout << "receive message >> " << re_message << endl;
		//}
	}
};


class transmitter {
	int tr_sock = 0;
	sockaddr_in tr_addr;
public:
	transmitter(int &sock, sockaddr_in &addr) {
		tr_sock = sock;
		tr_addr = addr;
		tr_addr.sin_addr.s_addr = inet_addr(other_ip);
		//connect(tr_sock, (sockaddr *)&tr_addr, sizeof(tr_addr));
	}

	//void write_message() {
	void operator () () {
		connect(tr_sock, (sockaddr *)&tr_addr, sizeof(tr_addr));

		while(1) {
		char tr_message[32];
		cout << "input message >> ";
		cin >> tr_message;
		//lock_guard<mutex> lock(mtx);
		mtx1.lock();
		send(tr_sock, tr_message, sizeof(tr_message), 0);
		mtx1.unlock();
		}
	}
};

void func(receiver &re_obj) {
	while(1) {
		re_obj.read_message();
	}
}


int main()
{
	int sock;
	sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4000);
	
	receiver re_obj(sock, addr);
	cout << "check" << endl;
	transmitter tr_obj(sock, addr);

	thread th1(func, ref(re_obj));
	thread th2(ref(tr_obj));

	th1.join();
	th2.join();

	close(sock);
}













