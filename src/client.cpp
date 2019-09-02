#include "client.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <iostream>

#define RequestSize 10
#define DataRange 15 //DataRange is also used in main.cpp
#define maxDLine 5

client::client(/* args */) {

}

client::~client()
{
}

void client::generateRequest() {
    int size, max, min, data;
    
    size = (rand() % RequestSize) + 1; 

    for (int i=0; i<size; i++) {
        // checking for data replicas
        do {
            data = rand() % DataRange ;
        }while( std::find(Request.begin(), Request.end(), data) != Request.end());
        Request.push_back(data);
    }

    max = size + maxDLine;
    min = size;
    DeadLines = (rand() % (max - min) + 1) + min;
}

void client::calculateUDR() {
    UnservedDataRequests = (int) Request.size();
}