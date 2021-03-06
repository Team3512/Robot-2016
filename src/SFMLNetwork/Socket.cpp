////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2012 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include "Socket.hpp"

#include <cstring>
#include <iostream>

#include <fcntl.h>

namespace sf {
Socket::Socket(Type sockType) :
    m_socketType(sockType) {
}

Socket::~Socket() {
    // Close the socket before it gets destructed
    close();
}

void Socket::setBlocking(bool blocking) {
    int status = fcntl(m_socket, F_GETFL);
    if (blocking) {
        fcntl(m_socket, F_SETFL, status & ~O_NONBLOCK);
    }
    else {
        fcntl(m_socket, F_SETFL, status | O_NONBLOCK);
    }

    m_isBlocking = blocking;
}

bool Socket::isBlocking() const {
    return m_isBlocking;
}


////////////////////////////////////////////////////////////
int Socket::getHandle() const {
    return m_socket;
}

void Socket::create() {
    // Don't create the socket if it already exists
    if (m_socket == -1) {
        int handle = socket(PF_INET,
                            m_socketType == Tcp ? SOCK_STREAM : SOCK_DGRAM,
                            0);
        create(handle);
    }
}

void Socket::create(int handle) {
    // Don't create the socket if it already exists
    if (m_socket == -1) {
        // Assign the new handle
        m_socket = handle;

        // Set the current blocking state
        setBlocking(m_isBlocking);

        if (m_socketType == Tcp) {
            // Disable the Nagle algorithm (ie. removes buffering of TCP packets)
            int yes = 1;
            if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
                           reinterpret_cast<char*>(&yes),
                           sizeof(yes)) == -1) {
                std::cerr << "Failed to set socket option \"TCP_NODELAY\" ; " <<
                    "all your TCP packets will be buffered\n";
            }
        }
        else {
            // Enable broadcast by default for UDP sockets
            int yes = 1;
            if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST,
                           reinterpret_cast<char*>(&yes),
                           sizeof(yes)) == -1) {
                std::cerr << "Failed to enable broadcast on UDP socket\n";
            }
        }
    }
}

void Socket::close() {
    // Close the socket
    if (m_socket != -1) {
        ::close(m_socket);
        m_socket = -1;
    }
}

sockaddr_in Socket::createAddress(uint32_t address, unsigned short port) {
    sockaddr_in addr;
    std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    addr.sin_addr.s_addr = htonl(address);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);

    return addr;
}

Socket::Status Socket::getErrorStatus() {
    // The followings are sometimes equal to EWOULDBLOCK,
    // so we have to make a special case for them in order
    // to avoid having double values in the switch case
    if ((errno == EAGAIN) || (errno == EINPROGRESS)) {
        return Socket::NotReady;
    }

    switch (errno) {
    case EWOULDBLOCK:  return Socket::NotReady;
    case ECONNABORTED: return Socket::Disconnected;
    case ECONNRESET:   return Socket::Disconnected;
    case ETIMEDOUT:    return Socket::Disconnected;
    case ENETRESET:    return Socket::Disconnected;
    case ENOTCONN:     return Socket::Disconnected;
    default:           return Socket::Error;
    }
}
} // namespace sf
