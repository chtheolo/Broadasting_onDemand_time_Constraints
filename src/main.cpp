#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include "client.h"
#include "webserver.h"

#define DataRange 15


struct CA {
    int data;
    int slack_time;
    int U;
};

void printClients_Request(int numberOfClients, client * client) {
    int i;
    unsigned u;

    for(i=0; i<numberOfClients; i++) {
        std::cout << "\nThe U(" << i <<") is: " << client[i].UnservedDataRequests << std::endl;
    }

    for(i=0; i<numberOfClients; i++) {
        std::cout << "\nClient[" << i << "] request: ";
        for (u=0; u<client[i].Request.size(); u++) {
            std::cout << client[i].Request[u] << " " ;
        }
        std::cout << "--> " << "[ " << client[i].DeadLines << " ] ";
        std::cout << "\n";

    }
}

void RemovePhase(client *client, int numberOfclients , std::list<CA>::iterator it, std::vector<int> R_ca) {
    int i;
    std::vector<int>::iterator it_del_vec;
    unsigned u;

    u=0;
    for (i=0; i<numberOfclients; i++) {
        it_del_vec = client[i].Request.begin();
        client[i].UnservedDataRequests--;
        it_del_vec = std::find(client[i].Request.begin(), client[i].Request.end(), R_ca[u]);
        if(it_del_vec != client[i].Request.end()) {
            client[i].Request.erase(it_del_vec);
        }
        u++;
    }
}

void initial(int *p_item_popularity) {
    for (int i=0; i<DataRange; i++) {
        *(p_item_popularity + i) = 0;
    }
}

std::vector<int> AggregationPhase(int *p_item_popularity, client *Client, int numberOfClients) {
    unsigned u;
    int t_count=0, i, data, max, x;
    std::vector<int> ca;

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

    return(ca);    
}


int main (int argc, char *argv[]) {
    int numberOfClients, i, Item_Popularity[DataRange], t_count=0;
    unsigned u,x;
    int *p_item_popularity;

    std::vector<int> ca, buffer, min_buf;
    std::vector<int>::iterator it_vector;

    std::list<CA> CAlist, broadcastList;
    std::list<CA>::iterator it, it_buf, it2;

    srand(time(NULL));

    // point to the beggining of the item_pop array
    p_item_popularity = Item_Popularity; 
    
    // call the initial function for Item_Popularity array
    initial(p_item_popularity);

    
    std::cout << "Select the number of clients: " << std::endl;
    std::cin >> numberOfClients;

    client* Client = new client[numberOfClients];

    webserver myWebServer;
    std::cout << "Select the number of broadcast channels((Braodcast Channels should less than the number of clients): " << std::endl;
    std::cin >> myWebServer.channels;

    for(i=0; i<numberOfClients; i++) {
        Client[i].generateRequest();
        Client[i].calculateUDR();
        //std::cout << "The U(" << i <<") is: " << Client[i].UnservedDataRequests << std::endl;
    }
    
    printClients_Request(numberOfClients, Client);

    ca = AggregationPhase(p_item_popularity, Client, numberOfClients);

    CA ca_final;


    /***** Make CA list with merging the same data items  ******/
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
    /***************************************************************/
    
    std::cout << "\nData Popularity : " << std::endl;
    for(i=0; i<DataRange; i++) {
        std::cout << "\t\t  data_item_id[" << i << "]" << " = " << Item_Popularity[i] << std::endl;
    }

    std::cout <<"\nThe candidates from every request: { ";
    std::cout << ca[0] ;
    for(u=1; u<ca.size(); u++) { 
        std::cout << ", ";
        std::cout << ca[u];
    }
    std::cout << " }" << std::endl;

    std::cout << "\nThe list (after merging the same data_items) CA: { " << std::endl;
    it = CAlist.begin();
    std::cout << "\ndata_item_id: " << it->data << std::endl;
    std::cout << "slack_time: " << it->slack_time << std::endl;
    std::cout << "Unserved_DataItems: " << it->U << std::endl;
    it++;
    while(it != CAlist.end()) {
        std::cout << "\n";
        std::cout << "data_item_id: " << it->data << std::endl;
        std::cout << "slack_time: " << it->slack_time << std::endl;
        std::cout << "Unserved_DataItems: " << it->U << std::endl;
        it++;
    }
    std::cout << "\n} " << std::endl;

    int count=0;
    it = CAlist.begin();
    do{
        count++;
        it++;
    }
    while(it != CAlist.end());

    if( count < myWebServer.channels) {
        it = CAlist.begin();
        i=1;
        for(it=CAlist.begin(); it!=CAlist.end(); it++) {
            std::cout << "channel[" << i << "] ->";
            std::cout << "| " << it->data << "|"<< std::endl;
            i++;
        }

        it = CAlist.begin();


        RemovePhase(Client, numberOfClients, it, ca);

        printClients_Request(numberOfClients, Client);

    }
    else { /*****************   Conversion Phase  ********************/

        CA min;

        for(i=0; i<myWebServer.channels; i++) {
            //std::cout << "enter the for-loop\n" << std::endl;
            it = CAlist.begin();
            it_buf = CAlist.begin();
            min.data = it->data;
            min.slack_time = it->slack_time;
            min.U = it->U;
            it++;

            //std::cout << "\n" << CAlist.size() << "\n" << std::endl;

            while(it != CAlist.end()){
                if(it->slack_time < min.slack_time) { // data with min(slack_time)
                    min.data = it->data;
                    min.slack_time = it->slack_time;
                    min.U = it->U;
                    std::advance(it_buf, std::distance(CAlist.begin(), it));
                }
                else if(it->slack_time == min.slack_time) { // if min(slack_time) tie then ->
                    if(it->U < min.U) {         // min(Unserved items)
                        min.data = it->data;
                        min.slack_time = it->slack_time;
                        min.U = it->U;
                    }
                    else if(it->U == min.U) {     // if min(Userved_items) tie then ->
                        if(it->data < min.data) {  // min(data_id)
                            min.data = it->data;
                            min.slack_time = it->slack_time;
                            min.U = it->U;
                            std::advance(it_buf, std::distance(CAlist.begin(), it));
                        }
                    }
                }
                it++;
            }

            broadcastList.push_back(min);
            CAlist.erase(it_buf, std::next(it_buf,1));
        }


        it = CAlist.begin();
        std::cout << "\nThe CAlist with the deleted data :" << std::endl;
        while(it != CAlist.end()) {
            std::cout << "\ndata item: ";
            std::cout << it->data ;
            std::cout << "\nslack time: ";
            std::cout << it->slack_time ;
            std::cout << "\nUserved DataItems: ";
            std::cout << it->U << std::endl;
            it++;
        }

        it = broadcastList.begin();
        std::cout << "\nThe list with the broadcast data :" << std::endl;
        while(it != broadcastList.end()) {
            std::cout << "\ndata item: ";
            std::cout << it->data ;
            std::cout << "\nslack time: ";
            std::cout << it->slack_time ;
            std::cout << "\nUserved DataItems: ";
            std::cout << it->U << std::endl;
            it++;
        }

    }


    delete [] Client;
    
    return 0;
}