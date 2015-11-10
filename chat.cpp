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
#define target_ip "192.168.1.26"
//#define target_ip "127.0.0.1"

using namespace std;

mutex mtx1, mtx2;


class receiver {
	int re_sock = 0;
	int re_new_sock = 0;
	sockaddr_in read_addr;
	sockaddr_in write_addr;
	socklen_t write_addr_len = sizeof(write_addr);

public:
	receiver() {
		bzero((char *)&read_addr, sizeof(read_addr));
		bzero((char *)&write_addr, sizeof(write_addr));
		re_sock = socket(AF_INET, SOCK_STREAM, 0);
		read_addr.sin_family = AF_INET;
		read_addr.sin_port = htons(4000);
		inet_aton("INADDR_ANY", &read_addr.sin_addr);

		bind(re_sock, (sockaddr *)&read_addr, sizeof(read_addr));
		//listen(re_sock, 1);
		//re_new_sock = accept(re_sock, (sockaddr *)&write_addr, &write_addr_len);
	}

	void operator() () {
		listen(re_sock, 1);
		re_new_sock = accept(re_sock, (sockaddr *)&write_addr, &write_addr_len);
		while(1) {
			char re_message[32];
			if((recv(re_new_sock, re_message, sizeof(re_message), 0)) < 0) {
				cout << "recv error" << endl;
			}

			mtx2.lock();
			cout << "receive message >> " << re_message << endl;
			mtx2.unlock();
		}
	}

	~receiver() { close(re_sock); close(re_new_sock); }
};


class transmitter {
	int tr_sock = 0;
	sockaddr_in tr_addr;

public:
	transmitter() {
		bzero((char *)&tr_addr, sizeof(tr_addr));
		tr_sock = socket(AF_INET, SOCK_STREAM, 0);
		tr_addr.sin_family = AF_INET;
		tr_addr.sin_port = htons(4000);
		tr_addr.sin_addr.s_addr = inet_addr(target_ip);
		//connect(tr_sock, (sockaddr *)&tr_addr, sizeof(tr_addr));
	}

	void operator () () {
		int flag = -1;
		while(flag == -1) {
			flag = connect(tr_sock, (sockaddr *)&tr_addr, sizeof(tr_addr));
		}

		while(1) {
			char tr_message[32];
			//lock_guard<mutex> lock(mtx1);
			mtx1.lock();
			cout << "input message >> ";
			cin >> tr_message;
			mtx1.unlock();

			//mtx1.lock();
			send(tr_sock, tr_message, sizeof(tr_message), 0);
			//mtx1.unlock();
		}
	}

	~transmitter() { close(tr_sock); }
};


int main()
{
	receiver re_obj;
	thread th1(ref(re_obj));

	cout << "target ip >> " << endl;
	//cin >> 

	transmitter tr_obj;
	thread th2(ref(tr_obj));

	th1.join();
	th2.join();
}













