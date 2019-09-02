#include <vector>

class client
{
private:
    /* data */
public:
    std::vector<int> Request;
    std::vector<int> PopularData;
    int UnservedDataRequests;
    int DeadLines;
    void generateRequest();
    void calculateUDR();
    client();
    ~client();
};