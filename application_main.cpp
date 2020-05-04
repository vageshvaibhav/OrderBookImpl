#include "order_feed_processor.h"

int main(int argc, char* argv[]) 
{
    if ( argc != 2 ) 
    {
        cout<<"Wrong number of arguments passed,Input file name needed"<<endl; 
        return 1;
    }
	string inputFile(argv[1]);
	
    OrderFeedProcessor ofp;
    if (ofp.ParseInputFileToOrders(inputFile))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
