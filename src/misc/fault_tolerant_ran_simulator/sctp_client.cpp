#include "sctp_client.h"

SctpClient::SctpClient() {
	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	g_utils.handle_type1_error(conn_fd, "Socket error: sctpclient_sctpclient");
}

void SctpClient::conn(string arg_server_ip_addr, int arg_server_port) {
	init(arg_server_ip_addr, arg_server_port);
	g_nw.set_rcv_timeout(conn_fd, 1);
	connect_with_server();
}
void SctpClient::conn_refresh() {

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	g_utils.handle_type1_error(conn_fd, "Socket error: sctpclient_sctpclient");
	conn(server_ip_addr,server_port);

}
void SctpClient::init(string arg_server_ip_addr, int arg_server_port) {
	int status;

	server_port = arg_server_port;
	server_ip_addr = arg_server_ip_addr;
	g_nw.set_inet_sock_addr(server_ip_addr, server_port, server_sock_addr);
}

void SctpClient::connect_with_server() {
	int status;
	int retCount = 10;
	while (1) {//cout<<".";
		status = connect(conn_fd, (struct sockaddr *)&server_sock_addr, sizeof(server_sock_addr));
		if (errno == ECONNREFUSED || errno == ETIMEDOUT) {
			errno = 0;
			cout<<"ccc"<<endl;
			usleep(1000);
			continue;
		}
	else if(errno==EISCONN){
		
			release();
			retCount--;
			usleep(1000);
			
			break;
		}
		else if(errno == EBADF){
			release();
			usleep(1000);
			break;
		}

		else {
			retCount = 3;
			break;
		}
	}
	
	g_utils.handle_type1_error(status, "Connect error: sctpclient_connectwithserver");
}

int SctpClient::snd(Packet pkt) {
	int status;
	int err_code = 0;
	while (1) {cout<".";
		status = g_nw.write_sctp_pkt(conn_fd, pkt);
		if (errno == EPERM) {
			errno = 0;
			usleep(1000);
			continue;
		}
		else if(errno == EAGAIN || errno == EPIPE || errno == EBADF || errno == ECONNRESET){  //ECONNRESET 
			err_code = -1;
			release();
			errno = 0;
			sleep(1);
		
			break;	 
		}
		else {
			
			if(errno!=0)cout<<"ERROR at snd occuredes"<<pkt.len<<endl;
			break;
		}		
	}

	if(status <=0)cout<<"ERROR at snd.."<<err_code<<"|"<<status<<endl;
	
	return err_code;
}

int SctpClient::rcv(Packet &pkt) {
	int status;
	int err_code = 0;
	status = g_nw.read_sctp_pkt(conn_fd, pkt);
	if(errno == EAGAIN || errno == EPIPE || errno == EBADF || errno == ECONNRESET){
			err_code = -1;
			release();
			errno = 0;	//
			sleep(1);
	
			
		}else {
			
			if(errno!=0)cout<<"ERROR at snd occuredes"<<pkt.len<<endl;
		}

	return err_code;
}

SctpClient::~SctpClient() {
	close(conn_fd);
}
void SctpClient::release(){
	close(conn_fd);
}
