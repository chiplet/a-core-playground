// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

// https://github.com/openocd-org/openocd/blob/master/doc/manual/jtag/drivers/remote_bitbang.txt

class JtagRemoteBitbang {
public:
    JtagRemoteBitbang(uint16_t port, uint8_t* tck, uint8_t* tms, uint8_t* tdi, uint8_t* tdo, uint8_t* trstn);
    void tick();

private:
    void write_inputs(uint8_t tck, uint8_t tms, uint8_t tdi);
    void write_resets(uint8_t trst, uint8_t srst);

    uint16_t m_port;
    int m_sockfd;
    int m_clientfd;

    // verilator
    uint8_t* m_tck;
    uint8_t* m_tms;
    uint8_t* m_tdi;
    uint8_t* m_tdo;
    uint8_t* m_trstn;
};