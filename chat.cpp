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

#define LOG_FILE_NAME "log"
#define PORT_NUMBER 40000
//#define target_ip "192.168.1.26"
#define TARGET_IP "127.0.0.1"
#define MESSAGE_SIZE 3000

using namespace std;

class chat {
	int recv_sock = 0, send_sock = 0, client_sock = 0;
	sockaddr_in base_addr, read_addr, write_addr, send_addr;
	socklen_t write_addr_len = sizeof(write_addr);
	string recv_message, stock_message;

public :
	chat(string target_ip) {
		recv_sock = socket(AF_INET, SOCK_STREAM, 0);
		send_sock = socket(AF_INET, SOCK_STREAM, 0);
		client_sock = socket(AF_INET, SOCK_STREAM, 0);

		bzero((char *)&base_addr, sizeof(base_addr));
		base_addr.sin_family = AF_INET;
		base_addr.sin_port = htons(PORT_NUMBER);

		read_addr = send_addr = base_addr; 
		inet_aton("INADDR_ANY", &read_addr.sin_addr);
		bind(recv_sock, (sockaddr *)&read_addr, sizeof(read_addr));

		send_addr.sin_addr.s_addr = inet_addr(target_ip.c_str());

		listen(recv_sock, 1);
	}


	~chat() {
		close(recv_sock);
		close(send_sock);
		close(client_sock);
	}

	//戻り値stringにして受信メッセージ返した方がいいかも。
	void receiver() {
		recv_message.resize(MESSAGE_SIZE);

		if((client_sock = accept(recv_sock, (sockaddr *)&write_addr, &write_addr_len)) < 0) {
			cerr << "accept error" << endl;
		}

		if((recv(client_sock, (void *)recv_message.data(), recv_message.capacity(), 0)) < 0) {
			cerr << "recv error" << endl;
		}

		//return recv_message;
		if(recv_message != stock_message) {
			cout << "\treceive message >> " << recv_message << endl << endl;
			stock_message = recv_message;
			recv_message.clear();
		}
	}


	void transmitter(string send_message) {
		if((connect(send_sock, (sockaddr *)&send_addr, sizeof(send_addr))) < 0) {
			cerr << "connect error" << endl;
		}

		if((send(send_sock, send_message.data(), send_message.length() * sizeof(char), 0)) < 0) {
			cerr << "send error" << endl;
		}
	}


	void recv_roop() {
		while(1) {
			receiver();
			/*
			if(recv_message != stock_message) {
				cout << "\treceive message >> " << recv_message << endl << endl;
				stock_message = recv_message;
				recv_message.clear();
			}
			*/
		}
	}


	void th_chat() {
		thread th1(&chat::recv_roop, this);

		while(1) {
			string send_message;
			do {
				getline(cin, send_message);
			} while(send_message.length() == 0);
			transmitter(send_message);
		}

		th1.join();
	}
};


int main()
{
	string target_ip;
	cout << "input target ip >> ";
	//cin >> target_ip;
	target_ip = TARGET_IP;
	cout << "chat start" << endl << endl;

	chat obj(target_ip);
	obj.th_chat();
}




