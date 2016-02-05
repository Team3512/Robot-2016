// =============================================================================
// Description: Receives Insight's processed target data
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#ifndef INSIGHT_HPP
#define INSIGHT_HPP

#include "SFML/Network/IpAddress.hpp"
#include "SFML/Network/UdpSocket.hpp"

#include <string>
#include <vector>
#include <utility>

#include <cstdint>

class Insight {
public:
    Insight(const Insight&) = delete;
    Insight& operator=(const Insight&) = delete;
    virtual ~Insight();

    static Insight& getInstance(unsigned short dsPort);

    // Receives control commands from Driver Station and processes them
    std::string receiveFromDS();

    // Returns true if new target data has been received
    bool hasNewData() const;

    // Provides access to target data
    const std::pair<char, char>& getTarget(size_t i);
    size_t getNumTargets() const;

private:
    Insight(unsigned short portNumber);

    sf::UdpSocket m_socket;
    sf::IpAddress m_recvIP; // stores IP address temporarily during receive
    unsigned short m_recvPort; // stores port temporarily during receive

    char m_recvBuffer[256]; // buffer for Insight packets
    size_t m_recvAmount; // holds number of bytes received from Driver Station

    std::vector<std::pair<char, char>> m_targets;
    bool m_hasNewData;
};

#endif // INSIGHT_HPP
