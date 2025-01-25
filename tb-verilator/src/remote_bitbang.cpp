// SPDX-License-Identifier: Apache-2.0
#include "remote_bitbang.hpp"

void exit_err(const char* msg) {
    perror(msg);
    exit(-1);
}

JtagRemoteBitbang::JtagRemoteBitbang(uint16_t port, uint8_t* tck, uint8_t* tms, uint8_t* tdi, uint8_t* tdo, uint8_t* trstn) :
    m_port(port), m_tck(tck), m_tms(tms), m_tdi(tdi), m_tdo(tdo), m_trstn(trstn)
{
    // assert that required signals exist
    assert(m_tck != nullptr);
    assert(m_tms != nullptr);
    assert(m_tdi != nullptr);

    // 1. create socket
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1)
        exit_err("socket");
    const int enable = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        exit_err("setsockopt");
    }

    // 2. bind socket
    struct sockaddr_in ip_addr;
    ip_addr.sin_family = AF_INET;
    ip_addr.sin_port = htons(m_port);
    ip_addr.sin_addr.s_addr = 0;
    printf("Binding to port %u\n", m_port);
    if (bind(m_sockfd, (sockaddr*)&ip_addr, sizeof(ip_addr)) == -1) {
        exit_err("bind");
    }

    // 3. listen for connections
    if (listen(m_sockfd, 0) == -1) {
        exit_err("listen");
    }

    // 4. accept connection
    struct sockaddr_in client_ip;
    socklen_t client_ip_size = sizeof(client_ip);
    int conn = accept(m_sockfd, (sockaddr*)&client_ip, (socklen_t*)&client_ip);
    if (conn == -1) {
        exit_err("accept");
    }
    m_clientfd = conn;
    printf("new connection from %s\n", inet_ntoa(client_ip.sin_addr));
}

void JtagRemoteBitbang::write_inputs(uint8_t tck, uint8_t tms, uint8_t tdi) {
    //printf("%d %d %d\n", tck, tms, tdi);
    *this->m_tck = tck;
    *this->m_tms = tms;
    *this->m_tdi = tdi;
}

void JtagRemoteBitbang::write_resets(uint8_t trst, uint8_t srst) {
    if (m_trstn != nullptr) {
        *this->m_trstn = trst;
    }
    // FIXME: srst not implemented
}

void JtagRemoteBitbang::tick() {
    // read incoming bytes from socket
    uint8_t recv_byte;
    int recv_result = recv(m_clientfd, (void*)&recv_byte, 1, MSG_DONTWAIT);
    if (recv_result == 1) {
        // B - Blink on
        // b - Blink off
        // R - Read request
        // Q - Quit request
        //printf("%c", recv_byte);
        //fflush(stdout);
        switch (recv_byte)
        {

        /* inputs */
        case '0':
            write_inputs(0, 0, 0);
            break;
        case '1':
            write_inputs(0, 0, 1);
            break;
        case '2':
            write_inputs(0, 1, 0);
            break;
        case '3':
            write_inputs(0, 1, 1);
            break;
        case '4':
            write_inputs(1, 0, 0);
            break;
        case '5':
            write_inputs(1, 0, 1);
            break;
        case '6':
            write_inputs(1, 1, 0);
            break;
        case '7':
            write_inputs(1, 1, 1);
            break;
        
        /* resets */
        case 'r':
            write_resets(0, 0);
            break;
        case 's':
            write_resets(0, 1);
            break;
        case 't':
            write_resets(1, 0);
            break;
        case 'u':
            write_resets(1, 1);
            break;

        default:
            printf("JtagRemoteBitbang: received unknown character: '%c'\n", recv_byte);
            break;
        }
    }
}