#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include "client.h"
#include "webserver.h"

#define DataRange 15

void initial(int *p_item_popularity) {
    for (int i=0; i<DataRange; i++) {
        *(p_item_popularity + i) = 0;
    }
}

std::vector<int> AggregationPhase(int *p_item_popularity, client *Client, int numberOfClients) {
    unsigned u;
    int t_count=0, i, data, max, x;
    std::vector<int> ca;

    struct CA {
        int data;
        int slack_time;
        int U;
    };


    std::list<CA> CAlist;
    std::list<CA>::iterator it;


    //an array position corresponds to a data item, i.e. [i] -> i, data item
    //and the array value corresponds to popularity, i.e [i] = x , x popularity
    for(i=0; i<numberOfClients; i++) {
        for(u=0; u<Client[i].Request.size(); u++) {
            data = Client[i].Request[u];
            (*(p_item_popularity + data))++;
        }
    }

    for(i=0; i<numberOfClients; i++) {
        max = 0;
        for(u=0; u<Client[i].Request.size(); u++) {
            if(max < *(p_item_popularity + Client[i].Request[u])) {
                max = *(p_item_popularity + Client[i].Request[u]);
                x = Client[i].Request[u];
            }
        }
        ca.push_back(x); //find the candidates for every Rq
    }


    CA ca_final;

    /*i=0;
    it = CAlist.begin();
    for(u=0; u<ca.size(); u++) {
        ca_final.data= ca[u];
        ca_final.U= Client[i].UnservedDataRequests;
        ca_final.slack_time = Client[i].DeadLines - t_count - ca_final.U;
        CAlist.insert(it->data, ca[u]);   
        std::cout << it->data << std::endl;
        while(it != CAlist.end() && it->data != ca[u]) {
            it++;
        }
        if(it != CAlist.end()) {
            CAlist.push_back(ca_final);
        }
        else{
            if(it->slack_time > ca_final.slack_time) {
                it->slack_time = ca_final.slack_time;
            }
            if(it->U > ca_final.U) {
                it->U = ca_final.U;
            }
        }
        CAlist.push_back(ca_final);
        it++;
        i++;
    }


    std::cout << "ok" <<std::endl;
    it = CAlist.begin();
    while(it != CAlist.end()) {
        std::cout << it->data << std::endl;
        std::cout << it->slack_time<< std::endl;
        it++;
    }

    std::cout<< "end" << std::endl;*/

    return(ca);    
}


int main (int argc, char *argv[]) {
    int numberOfClients, i, Item_Popularity[DataRange], t_count=0;;
    unsigned u;
    int *p_item_popularity;
    std::vector<int> ca;

    struct CA {
        int data;
        int slack_time;
        int U;
    };

    std::list<CA> CAlist;
    std::list<CA>::iterator it;

    srand(time(NULL));

    // point to the beggining of the item_pop array
    p_item_popularity = Item_Popularity; 
    
    // call the initial function for Item_Popularity array
    initial(p_item_popularity);

    /*for(i=0; i<DataRange; i++) {
        std::cout << Item_Popularity[i] << std::endl;
    }*/
    
    std::cout << "Select the number of clients: " << std::endl;
    std::cin >> numberOfClients;

    client* Client = new client[numberOfClients];

    webserver myWebServer;
    std::cout << "Select the number of broadcast channels((Braodcast Channels should less than the number of clients): " << std::endl;
    std::cin >> myWebServer.channels;

    for(i=0; i<numberOfClients; i++) {
        Client[i].generateRequest();
        Client[i].calculateUDR();
        std::cout << "The U(" << i <<") is: " << Client[i].UnservedDataRequests << std::endl;
    }
    
    for(i=0; i<numberOfClients; i++) {
        std::cout << "Client[" << i << "] request: ";
        for (u=0; u<Client[i].Request.size(); u++) {
            std::cout << Client[i].Request[u] << " " ;
        }
        std::cout << "--> " << "[ " << Client[i].DeadLines << " ] ";
        std::cout << "\n";
    }

    

    ca = AggregationPhase(p_item_popularity, Client, numberOfClients);

    CA ca_final;

    i=0;
    for(u=0; u<ca.size(); u++) {
        it = CAlist.begin();
        while(it != CAlist.end() && it->data != ca[u]){
            it++;
        }
        if(it != CAlist.end()){
            if(it->slack_time > ca_final.slack_time) {
                it->slack_time = ca_final.slack_time;
            }
            if(it->U > ca_final.U) {
                it->U = ca_final.U;
            }
        }
        else {
            ca_final.data = ca[u];
            ca_final.U= Client[i].UnservedDataRequests;
            ca_final.slack_time = Client[i].DeadLines - t_count - ca_final.U;
            CAlist.push_back(ca_final);
        }
        i++;
    }

    for(u=0; u<ca.size(); u++) {
        std::cout << ca[u] << std::endl;
    }

    std::cout << "The list CA: " << std::endl;
    it = CAlist.begin();
    while(it != CAlist.end()) {
        std::cout << it->data << std::endl;
        it++;
    }

    std::cout << "Data Popularity : " << std::endl;
    for(i=0; i<DataRange; i++) {
        std::cout << "[" << i << "]" << " = " << Item_Popularity[i] << std::endl;
    }

    delete [] Client;
    
    return 0;
}