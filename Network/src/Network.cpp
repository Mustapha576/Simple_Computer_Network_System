#include "Network.h"
#include <iostream>
#include <fstream>
#include <algorithm>
Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    int hops = 0;

    for(int i = 1; i < commands.size(); i++){
        if(commands[i].find("MESSAGE") != string::npos){
            istringstream iss(commands[i]);
            string message_values;
            string sender_ID;
            string receiver_ID;
            iss >> message_values >> sender_ID;
            iss >> receiver_ID;

            string sender_IP;
            string receiver_IP;
            string sender_MAC_address;
            string receiver_MAC_adress;

            int message_sender;

            for(int j = 0; j < clients.size(); j++){
                if(clients[j].client_id == sender_ID){
                    for(int k = 0; k < clients.size(); k++){
                        if(clients[k].client_id == receiver_ID){
                            receiver_IP = clients[k].client_ip;
                            break;
                        }
                    }
                    sender_IP = clients[j].client_ip;
                    sender_MAC_address = clients[j].client_mac;
                    message_sender = j;

                    for(auto& routes : clients[j].routing_table){
                        if(routes.first == receiver_ID){
                            for(int k = 0; k < clients.size(); k++){
                                if(clients[k].client_id == routes.second){
                                    receiver_MAC_adress = clients[k].client_mac;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }

            string message;
            for(int j = 0; j < commands[i].size(); j++){
                if(commands[i][j] == '#'){
                    message = commands[i].substr(j + 1, commands[i].size() - j - 2);
                    break;
                }
            }

            int frame_count;

            if(int(message.size()) % message_limit != 0){
                frame_count = (int(message.size()) / message_limit) + 1;
            }
            else{
                frame_count = int(message.size()) / message_limit;
            }


            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "COMMAND: " + commands[i] << endl;
            cout << string(9 + int(commands[i].size()), '-') << endl;

            string text = '"' + message + '"';
            cout << "Message to be sent: " + text << endl;
            cout << endl;

            for(int j = 0; j < frame_count; j++){

                string sub_message = message.substr(j * message_limit, message_limit);
                cout << "Frame #" + to_string(j + 1) << endl;

                Packet* physical = new PhysicalLayerPacket(3, sender_MAC_address, receiver_MAC_adress);
                physical->print();

                Packet* network = new NetworkLayerPacket(2, sender_IP, receiver_IP);
                network->print();

                Packet* transport = new TransportLayerPacket(1, sender_port, receiver_port);
                transport->print();

                Packet* application = new ApplicationLayerPacket(0, sender_ID, receiver_ID, sub_message);
                application->print();

                frame.push(application);
                frame.push(transport);
                frame.push(network);
                frame.push(physical);

                clients[message_sender].outgoing_queue.push(frame);

                cout << "Number of hops so far: 0" << endl;
                cout << "--------" << endl;

            }
        }
        else if(commands[i].find("SHOW_FRAME_INFO") != string::npos){
            istringstream iss(commands[i]);
            string message_values;
            string ID;
            string queue_info;
            string frame;

            iss >> message_values >> ID;
            iss >> queue_info;
            iss >> frame;

            if(queue_info == "out"){
                for(int j = 0; j < clients.size(); j++){
                    if(clients[j].client_id == ID){
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        cout << "COMMAND: " + commands[i] << endl;
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        if(clients[j].outgoing_queue.size() < stoi(frame)){
                            cout << "No such frame." << endl;
                            break;
                        }
                        cout << "Current Frame #" + frame + " on the outgoing queue of client " + ID << endl;

                        queue<stack<Packet*>> copy_outgoing = clients[j].outgoing_queue;
                        queue<stack<Packet*>> getting_frames;
                        for(int k = 0; k < stoi(frame) - 1; k++){
                            getting_frames.push(copy_outgoing.front());
                            copy_outgoing.pop();
                        }

                        getting_frames.push(copy_outgoing.front());
                        stack<Packet*> reverse;
                        for(int k = 0; k < 4; k++){
                            reverse.push(copy_outgoing.front().top());
                            copy_outgoing.front().pop();
                        }
                        copy_outgoing.pop();

                        for(int k = 0; k < 4; k++){
                            reverse.top()->printforqueue();
                            reverse.pop();
                        }
                        cout << "Number of hops so far: " + to_string(hops) << endl;
                    }

                }

            }
            else if(queue_info == "in"){
                for(int j = 0; j < clients.size(); j++){
                    if(clients[j].client_id == ID){
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        cout << "COMMAND: " + commands[i] << endl;
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        if(clients[j].incoming_queue.size() < stoi(frame)){
                            cout << "No such frame." << endl;
                            break;
                        }
                        cout << "Current Frame #" + frame + " on the incoming queue of client " + ID << endl;

                        queue<stack<Packet*>> copy_incoming = clients[j].incoming_queue;
                        queue<stack<Packet*>> getting_frames;
                        for(int k = 0; k < stoi(frame) - 1; k++){
                            getting_frames.push(copy_incoming.front());
                            copy_incoming.pop();
                        }

                        getting_frames.push(copy_incoming.front());
                        stack<Packet*> reverse;
                        for(int k = 0; k < 4; k++){
                            reverse.push(copy_incoming.front().top());
                            copy_incoming.front().pop();
                        }
                        copy_incoming.pop();

                        for(int k = 0; k < 4; k++){
                            reverse.top()->printforqueue();
                            reverse.pop();
                        }
                        cout << "Number of hops so far: " + to_string(hops) << endl;
                    }

                }
            }

        }
        else if(commands[i].find("SHOW_Q_INFO") != string::npos){
            istringstream iss(commands[i]);
            string message_values;
            string ID;
            string queue_info;

            iss >> message_values >> ID;
            iss >> queue_info;

            if(queue_info == "out"){
                for(int j = 0; j < clients.size(); j++){
                    if(clients[j].client_id == ID){
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        cout << "COMMAND: " + commands[i] << endl;
                        cout << string(9 + int(commands[i].size()), '-') << endl;

                        cout << "Client " + ID + " Outgoing Queue Status" << endl;
                        int size = clients[j].outgoing_queue.size();
                        cout << "Current total number of frames: " + to_string(size) << endl;
                        break;
                    }
                }
            }
            else if(queue_info == "in"){
                for(int j = 0; j < clients.size(); j++){
                    if(clients[j].client_id == ID){
                        cout << string(9 + int(commands[i].size()), '-') << endl;
                        cout << "COMMAND: " + commands[i] << endl;
                        cout << string(9 + int(commands[i].size()), '-') << endl;

                        cout << "Client " + ID + " Incoming Queue Status" << endl;
                        int size = clients[j].incoming_queue.size();
                        cout << "Current total number of frames: " + to_string(size) << endl;
                        break;
                    }
                }
            }
        }
        else if(commands[i].find("SEND") != string::npos){
            istringstream iss(commands[i]);
            string message_values;

            iss >> message_values;

            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "COMMAND: " + commands[i] << endl;
            cout << string(9 + int(commands[i].size()), '-') << endl;

            int frame_count = 1;
            for(int j = 0; j < clients.size(); j++){
                if(!clients[j].outgoing_queue.empty()){
                    for(int h = 0; h < clients.size(); h++){
                        if(clients[j].outgoing_queue.front().top()->MAC_address_receiver() == clients[h].client_mac){
                            int client_outgoing_queue_size = clients[j].outgoing_queue.size();
                            string text;
                            int number_of_frames = 0;
                            for(int k = 0; k < client_outgoing_queue_size; k++){
                                cout << "Client " + clients[j].client_id + " sending frame #" + to_string(frame_count) + " to client " + clients[h].client_id << endl;
                                clients[h].incoming_queue.push(clients[j].outgoing_queue.front());
                                for(int l = 0; l < 4; l++){
                                    clients[j].outgoing_queue.front().top()->print();
                                    if(l == 3){
                                        text += clients[j].outgoing_queue.front().top()->message_of_frame();
                                        if(clients[j].outgoing_queue.front().top()->message_of_frame().find('.') != string::npos ||
                                           clients[j].outgoing_queue.front().top()->message_of_frame().find('!') != string::npos ||
                                           clients[j].outgoing_queue.front().top()->message_of_frame().find('?') != string::npos){
                                            number_of_frames = frame_count;
                                            string text_deneme;
                                            text_deneme += '"' + text + '"';
                                            /*
                                            Log log_entry = *new Log("deneme", text_deneme, frame_count, hops, clients[j].outgoing_queue.front().top()->ID_of_sender(), clients[j].outgoing_queue.front().top()->ID_of_receiver(), true, ActivityType::MESSAGE_SENT);
                                            clients[j].log_entries.emplace_back(log_entry);
                                            */
                                            int temp_hop;
                                            for(int h = 0; h < clients.size(); h++){
                                                for(int l = 0; l < clients[h].log_entries.size(); l++){
                                                    if(clients[h].log_entries[l].message_content == text_deneme &&
                                                       clients[h].log_entries[l].activity_type == ActivityType::MESSAGE_SENT){
                                                        temp_hop = clients[h].log_entries[l].number_of_hops;
                                                    }
                                                }
                                            }
                                            clients[j].log_entries.emplace_back("deneme", text_deneme, frame_count, temp_hop, clients[j].outgoing_queue.front().top()->ID_of_sender(), clients[j].outgoing_queue.front().top()->ID_of_receiver(), true, ActivityType::MESSAGE_SENT);
                                            text = "";
                                            frame_count = 1;
                                        }
                                        else{
                                            frame_count++;
                                        }

                                    }
                                    clients[j].outgoing_queue.front().pop();
                                }
                                cout << "Number of hops so far: " + to_string(hops) << endl;
                                cout << "--------" << endl;
                                clients[j].outgoing_queue.pop();
                            }
                            string message = '"' + text + '"';
                            break;

                        }
                    }
                }
            }
        }
        else if(commands[i].find("RECEIVE") != string::npos){
            istringstream iss(commands[i]);
            string message_values;

            iss >> message_values;
            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "COMMAND: " + commands[i] << endl;
            cout << string(9 + int(commands[i].size()), '-') << endl;

            string text;
            int frame_count = 1;
            for(int j = 0; j < clients.size(); j++){
                if(!clients[j].incoming_queue.empty()){
                    queue<stack<Packet*>> copy_incoming = clients[j].incoming_queue;

                    while(!copy_incoming.empty()){
                        stack<Packet*> reverse;
                        for(int k = 0; k < 4; k++){
                            reverse.push(copy_incoming.front().top());
                            copy_incoming.front().pop();
                        }
                        stack<Packet*> copy_reverse;
                        stack<Packet*> copy_IDs;
                        int no_second_routing_table = 0;
                        if(reverse.top()->ID_of_receiver() != clients[j].client_id){
                            for(auto& routes : clients[j].routing_table){
                                if(routes.first == reverse.top()->ID_of_receiver()){
                                    for(int k = 0; k < clients.size(); k++){
                                        if(clients[k].client_id == routes.second){
                                            for(int l = 0; l < 4; l++){
                                                if(l == 0){
                                                    copy_reverse.push(reverse.top());
                                                    copy_IDs.push(reverse.top());
                                                    reverse.pop();
                                                }
                                                else if(l < 3){
                                                    copy_reverse.push(reverse.top());
                                                    reverse.pop();
                                                }
                                                else if(l == 3){
                                                    Packet* new_physical = new PhysicalLayerPacket(3, clients[j].client_mac, clients[k].client_mac);
                                                    copy_reverse.push(new_physical);
                                                }
                                            }
                                            clients[j].outgoing_queue.push(copy_reverse);
                                            clients[j].incoming_queue.pop();
                                            if(frame_count == 1){
                                                for(int h = 0; h < clients.size(); h++){
                                                    if(clients[h].client_mac == reverse.top()->MAC_address_sender()){
                                                        cout << "Client " + clients[j].client_id + " receiving a message from client " + clients[h].client_id + ", but intended for client " + copy_IDs.top()->ID_of_receiver() + ". Forwarding..." << endl;
                                                        break;
                                                    }
                                                }

                                            }
                                            stack<Packet*> another_copy_outgoing_queue = clients[j].outgoing_queue.back();
                                            stack<Packet*> deneme;
                                            for(int l = 0; l < 4; l++){
                                                deneme.push(another_copy_outgoing_queue.top());
                                                another_copy_outgoing_queue.pop();
                                            }
                                            text += copy_IDs.top()->message_of_frame();

                                            if(deneme.top()->message_of_frame().find('.') != string::npos ||
                                               deneme.top()->message_of_frame().find('!') != string::npos ||
                                               deneme.top()->message_of_frame().find('?') != string::npos){
                                                cout << "Frame #" + to_string(frame_count) + " MAC address change: New sender MAC " + clients[j].outgoing_queue.back().top()->MAC_address_sender() + ", new receiver MAC " + clients[j].outgoing_queue.back().top()->MAC_address_receiver() << endl;
                                                cout << "--------" << endl;
                                                string text_deneme;
                                                text_deneme += '"' + text + '"';
                                                /*
                                                Log log_entry = *new Log("deneme", text_deneme, frame_count, hops, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), true, ActivityType::MESSAGE_FORWARDED);
                                                clients[j].log_entries.emplace_back(log_entry);
                                                */
                                                int temp_hop;
                                                for(int h = 0; h < clients.size(); h++){
                                                    for(int l = 0; l < clients[h].log_entries.size(); l++){
                                                        if(clients[h].log_entries[l].message_content == text_deneme &&
                                                        clients[h].log_entries[l].activity_type == ActivityType::MESSAGE_SENT){
                                                            temp_hop = clients[h].log_entries[l].number_of_hops + 1;
                                                        }
                                                    }
                                                }
                                                clients[j].log_entries.emplace_back("deneme", text_deneme, frame_count, temp_hop, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), true, ActivityType::MESSAGE_FORWARDED);
                                                text = "";
                                                frame_count = 1;
                                            }
                                            else{
                                                cout << "Frame #" + to_string(frame_count) + " MAC address change: New sender MAC " + clients[j].outgoing_queue.back().top()->MAC_address_sender() + ", new receiver MAC " + clients[j].outgoing_queue.back().top()->MAC_address_receiver() << endl;
                                                frame_count++;
                                            }
                                            copy_incoming.pop();
                                            no_second_routing_table++;
                                            break;
                                        }
                                    }
                                    if(no_second_routing_table == 0){
                                        for(int l = 0; l < 4; l++){
                                            if(l == 0){
                                                copy_reverse.push(reverse.top());
                                                copy_IDs.push(reverse.top());
                                                reverse.pop();
                                            }
                                            else{
                                                copy_reverse.push(reverse.top());
                                                reverse.pop();
                                            }
                                        }

                                        for(int h = 0; h < clients.size(); h++){
                                            if(clients[h].client_mac == copy_reverse.top()->MAC_address_sender()){
                                                cout << "Client " + clients[j].client_id + " receiving frame #" + to_string(frame_count) + " from client " + clients[h].client_id + ", but intended from client " + copy_IDs.top()->ID_of_receiver() + ". Forwarding..." << endl;
                                            }
                                        }
                                        text += copy_IDs.top()->message_of_frame();
                                        cout << "Error: Unreachable destination. Packets are dropped after " +
                                                     to_string(hops) + " hops!" << endl;
                                        if(copy_IDs.top()->message_of_frame().find('.') != string::npos ||
                                           copy_IDs.top()->message_of_frame().find('!') != string::npos ||
                                           copy_IDs.top()->message_of_frame().find('?') != string::npos){
                                            string text_deneme;
                                            text_deneme += '"' + text + '"';
                                            /*
                                            Log log_entry = *new Log("deneme", text_deneme, frame_count, hops, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), false, ActivityType::MESSAGE_DROPPED);
                                            clients[j].log_entries.emplace_back(log_entry);
                                            */
                                            int temp_hop;
                                            for(int h = 0; h < clients.size(); h++){
                                                for(int l = 0; l < clients[h].log_entries.size(); l++){
                                                    if(clients[h].log_entries[l].message_content == text_deneme &&
                                                       clients[h].log_entries[l].activity_type == ActivityType::MESSAGE_SENT){
                                                        temp_hop = clients[h].log_entries[l].number_of_hops + 1;
                                                    }
                                                }
                                            }
                                            clients[j].log_entries.emplace_back("deneme", text_deneme, frame_count, temp_hop, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), false, ActivityType::MESSAGE_DROPPED);
                                            frame_count = 1;
                                            text = "";
                                            cout << "--------" << endl;
                                        }
                                        else{
                                            frame_count++;
                                        }
                                        clients[j].incoming_queue.pop();
                                        copy_incoming.pop();

                                    }
                                    break;
                                }
                            }
                        }
                        else if(reverse.top()->ID_of_receiver() == clients[j].client_id){
                            for(int l = 0; l < 4; l++){
                                if(l == 0){
                                    copy_reverse.push(reverse.top());
                                    copy_IDs.push(reverse.top());
                                    reverse.pop();
                                }
                                else{
                                    copy_reverse.push(reverse.top());
                                    reverse.pop();
                                }
                            }
                            for(int h = 0; h < clients.size(); h++){
                                if(clients[h].client_mac == copy_reverse.top()->MAC_address_sender()){
                                    cout << "Client " + copy_IDs.top()->ID_of_receiver() + " receiving frame #" + to_string(frame_count) + " from client " + clients[h].client_id + ", originating from client " + copy_IDs.top()->ID_of_sender() << endl;
                                }
                            }
                            for(int l = 0; l < 4; l++){
                                clients[j].incoming_queue.front().top()->print();
                                clients[j].incoming_queue.front().pop();
                            }
                            text += copy_IDs.top()->message_of_frame();

                            cout << "Number of hops so far: " + to_string(hops) << endl;
                            cout << "--------" << endl;

                            if(copy_IDs.top()->message_of_frame().find('.') != string::npos ||
                               copy_IDs.top()->message_of_frame().find('!') != string::npos ||
                               copy_IDs.top()->message_of_frame().find('?') != string::npos){
                                string text_with_bracket;
                                text_with_bracket = '"' + text + '"';
                                cout << "Client " + copy_IDs.top()->ID_of_receiver() + " received the message " + text_with_bracket + " from client " + copy_IDs.top()->ID_of_sender() + "." << endl;
                                for(int m = 0; m < clients.size(); m++){
                                    for(int n = 0; n < clients[m].log_entries.size(); n++){
                                        if(clients[m].log_entries[n].message_content == text_with_bracket && clients[m].log_entries[n].activity_type == ActivityType::MESSAGE_FORWARDED){
                                            for(int t = 0; t < clients[m].log_entries.size(); t++){
                                                if(clients[m].log_entries[t].message_content == text_with_bracket && clients[m].log_entries[t].activity_type == ActivityType::MESSAGE_SENT){
                                                    clients[m].log_entries.pop_back();
                                                }
                                            }
                                        }
                                    }
                                }
                                /*
                                Log log_entry = *new Log("deneme", text_with_bracket, frame_count, hops, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), true, ActivityType::MESSAGE_RECEIVED);
                                clients[j].log_entries.push_back(log_entry);
                                 */
                                clients[j].log_entries.emplace_back("deneme", text_with_bracket, frame_count, hops, copy_IDs.top()->ID_of_sender(), copy_IDs.top()->ID_of_receiver(), true, ActivityType::MESSAGE_RECEIVED);
                                frame_count = 1;
                                text = "";
                                cout << "--------" << endl;

                            }
                            else{
                                frame_count++;
                            }

                            clients[j].incoming_queue.pop();
                            copy_incoming.pop();

                        }
                    }


                }

            }


        }
        else if(commands[i].find("PRINT_LOG") != string::npos){
            istringstream iss(commands[i]);
            string message_values;
            string id;

            iss >> message_values >> id;

            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "COMMAND: " + commands[i] << endl;
            cout << string(9 + int(commands[i].size()), '-') << endl;

            for(int j = 0; j < clients.size(); j++){
                if(clients[j].client_id == id && clients[j].log_entries.size() != 0){
                    cout << "Client " + id + " Logs:" << endl;
                    cout << "--------------" << endl;
                    for(int h = 0; h < clients[j].log_entries.size(); h++){
                        cout << "Log Entry #" + to_string(h + 1) + ":" << endl;
                        if(clients[j].log_entries[h].activity_type == ActivityType::MESSAGE_RECEIVED){
                            cout << "Activity: Message Received" << endl;
                        }
                        else if(clients[j].log_entries[h].activity_type == ActivityType::MESSAGE_FORWARDED){
                            cout << "Activity: Message Forwarded" << endl;
                        }
                        else if(clients[j].log_entries[h].activity_type == ActivityType::MESSAGE_SENT){
                            cout << "Activity: Message Sent" << endl;
                        }
                        else if(clients[j].log_entries[h].activity_type == ActivityType::MESSAGE_DROPPED){
                            cout << "Activity: Message Dropped" << endl;
                        }
                        cout << "Timestamp: " + clients[j].log_entries[h].timestamp << endl;
                        cout << "Number of frames: " + to_string(clients[j].log_entries[h].number_of_frames) << endl;
                        cout << "Number of hops: " + to_string(clients[j].log_entries[h].number_of_hops) << endl;
                        cout << "Sender ID: " + clients[j].log_entries[h].sender_id << endl;
                        cout << "Receiver ID: " + clients[j].log_entries[h].receiver_id << endl;
                        if(clients[j].log_entries[h].success_status == true){
                            cout << "Success: Yes" << endl;
                            if(clients[j].log_entries[h].activity_type == ActivityType::MESSAGE_FORWARDED){
                            }
                            else{
                                cout << "Message: " + clients[j].log_entries[h].message_content << endl;
                            }


                        }
                        else if(clients[j].log_entries[h].success_status == false){
                            cout << "Success: No" << endl;
                        }
                        if(j == clients.size() - 1 && h == clients[j].log_entries.size() - 1){

                        }
                        else{
                            cout << "--------------" << endl;
                        }

                    }
                }
            }
        }
        else{
            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "COMMAND: " + commands[i] << endl;
            cout << string(9 + int(commands[i].size()), '-') << endl;
            cout << "Invalid command." << endl;
        }
    }

}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    fstream file;
    file.open(filename, ios::in);
    if(file.is_open()){
        string line;
        while(getline(file, line)){
            if(line.find('.') == string::npos){
                continue;
            }
            else{
                string client_id;
                string client_ip;
                string client_mac;
                int count = 0;
                for(int i = 0; i < line.size(); i++){
                    if(line.at(i) != ' '){
                        if(count == 0){
                            client_id += line.at(i);
                        }
                        else if(count == 1){
                            client_ip += line.at(i);
                        }
                        else if(count == 2){
                            client_mac += line.at(i);
                        }
                    }
                    else{
                        count++;
                    }
                }
                clients.emplace_back(client_id, client_ip, client_mac);
            }
        }
    }
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    fstream file;
    file.open(filename, ios::in);
    if(file.is_open()){
        string line;
        int client_numbers = 0;
        while(getline(file, line)){
            if(line.at(0) == '-'){
                client_numbers++;
            }
            else{
                string receiver_ID;
                string nexthop_ID;
                int id_count = 0;
                for(int i = 0; i < line.size(); i++){
                    if(line.at(i) == ' '){
                        id_count++;
                    }
                    else{
                        if(id_count == 0){
                            receiver_ID += line.at(i);
                        }
                        else if(id_count == 1){
                            nexthop_ID += line.at(i);
                        }
                    }
                }
                clients[client_numbers].routing_table[receiver_ID] = nexthop_ID;
            }
        }
    }
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    fstream file;
    file.open(filename, ios::in);
    if(file.is_open()) {
        string line;
        while(getline(file, line)){
            commands.push_back(line);
        }
    }
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
    while(!frame.empty()) {
        Packet* x = frame.top();
        frame.pop();
        delete x;
    }
}
