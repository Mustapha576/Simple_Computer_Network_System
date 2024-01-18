#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

using namespace std;

class Packet {
public:
    Packet(int layerID);
    virtual ~Packet();

    int layer_ID;

    friend ostream &operator<<(ostream &os, const Packet &packet);
    virtual void print() {};
    virtual void printforqueue() {};
    virtual string MAC_address_receiver() { return std::string(); };
    virtual string MAC_address_sender() { return std::string(); };
    virtual string message_of_frame() { return std::string(); };
    virtual string ID_of_receiver() { return std::string(); };
    virtual string ID_of_sender() { return std::string(); };
};

#endif // PACKET_H
