#include <iostream>
#include "Network.h"

using namespace std;

int main(int argc, char *argv[]) {

    // Instantiate HUBBMNET
    Network* HUBBMNET = new Network();

    // Read from input files
    vector<Client> clients = HUBBMNET->read_clients("clients.dat");
    HUBBMNET->read_routing_tables(clients, "routing.dat");
    vector<string> commands = HUBBMNET->read_commands("commands.dat");


    // Get additional parameters from the cmd arguments
    int message_limit = stoi("10");
    string sender_port = "895";
    string receiver_port = "741";

    // Run the commands
    HUBBMNET->process_commands(clients, commands, message_limit, sender_port, receiver_port);

    // Delete HUBBMNET
    delete HUBBMNET;

    return 0;
}


