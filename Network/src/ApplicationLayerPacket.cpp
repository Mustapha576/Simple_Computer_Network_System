#include "ApplicationLayerPacket.h"

ApplicationLayerPacket::ApplicationLayerPacket(int _layer_ID, const string &_sender_ID, const string &_receiver_ID, const string& _message_data) : Packet(_layer_ID) {
    sender_ID = _sender_ID;
    receiver_ID = _receiver_ID;
    message_data = _message_data;
}

void ApplicationLayerPacket::print() {
    // TODO: Override the virtual print function from Packet class to additionally print layer-specific properties.
    std::cout << "Sender ID: " + sender_ID + ", Receiver ID: " + receiver_ID << std::endl;
    string text;
    text = '"' + message_data + '"';
    std::cout << "Message chunk carried: " + text << std::endl;
}

void ApplicationLayerPacket::printforqueue(){
    string text;
    text = '"' + message_data + '"';
    std::cout << "Carried Message: " + text << std::endl;
    std::cout << "Layer 0 Info: Sender ID: " + sender_ID + ", Receiver ID: " + receiver_ID << std::endl;
}

string ApplicationLayerPacket::message_of_frame(){
    return message_data;
}
string ApplicationLayerPacket::ID_of_receiver(){
    return receiver_ID;
}
string ApplicationLayerPacket::ID_of_sender(){
    return sender_ID;
}

ApplicationLayerPacket::~ApplicationLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}
