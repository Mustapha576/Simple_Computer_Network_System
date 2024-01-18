#include "PhysicalLayerPacket.h"

PhysicalLayerPacket::PhysicalLayerPacket(int _layer_ID, const string& _sender_MAC, const string& _receiver_MAC)
        : Packet(_layer_ID) {
    sender_MAC_address = _sender_MAC;
    receiver_MAC_address = _receiver_MAC;
}

void PhysicalLayerPacket::print() {
    // TODO: Override the virtual print function from Packet class to additionally print layer-specific properties.
    std::cout << "Sender MAC address: " + sender_MAC_address + ", Receiver MAC address: " + receiver_MAC_address << std::endl;
}

void PhysicalLayerPacket::printforqueue() {
    std::cout << "Layer 3 Info: Sender MAC address: " + sender_MAC_address + ", Receiver MAC address: " + receiver_MAC_address << std::endl;
}

string PhysicalLayerPacket::MAC_address_receiver() {
    return receiver_MAC_address;
}

string PhysicalLayerPacket::MAC_address_sender() {
    return sender_MAC_address;
}

PhysicalLayerPacket::~PhysicalLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}