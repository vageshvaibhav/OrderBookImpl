#include <gtest/gtest.h>
#include "order_feed_processor.h"


class OrderBookTest : public ::testing::Test {
    protected:
        OrderBookTest() : sbuf{nullptr} {
        }
        ~OrderBookTest() override = default;
        void SetUp() override {
            // Save cout's buffer...
            sbuf = std::cout.rdbuf();
            // Redirect cout to stringstream buffer 
            std::cout.rdbuf(buffer.rdbuf());
        }
        void TearDown() override {
            // When done redirect cout to its old self
            std::cout.rdbuf(sbuf);
            sbuf = nullptr;
        }
        std::stringstream buffer{};
        // Save cout's buffer here
        std::streambuf *sbuf;
};

TEST_F(OrderBookTest, AddSingleBuyOrder) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,100000,B,1,1075");
    ofp.PrintBuyOrderBook();
    std::string expected("XYZ,100000,B,1,1075\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);
}

TEST_F(OrderBookTest, AddSingleSellOrder) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,100005,S,2,1025");
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,100005,S,2,1025\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, AddSingleBuySellOrderNoTrade) {
    OrderFeedProcessor ofp; 
    ofp.ParseInputStringToOrder("A,200000,B,1,1175");
    ofp.ParseInputStringToOrder("A,200005,S,2,1225");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,200000,B,1,1175\nXYZ,200005,S,2,1225\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, AddSingleBuySellOrderWithTrade) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,200000,B,1,1275");
    ofp.ParseInputStringToOrder("A,200005,S,2,1225");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,200005,SOLD,1,1275\nXYZ,200000,BOUGHT,1,1275\nXYZ,200005,S,1,1225\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, AddMultiplBuySingleAggresorSellOrderWithTrade) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,200001,B,4,100");
    ofp.ParseInputStringToOrder("A,200002,B,6,100");
    ofp.ParseInputStringToOrder("A,200003,B,10,90");
    ofp.ParseInputStringToOrder("A,200004,B,2,90");
    ofp.ParseInputStringToOrder("A,200005,B,3,90");
    ofp.ParseInputStringToOrder("A,200006,S,5,110");
    ofp.ParseInputStringToOrder("A,200007,S,10,110");
    ofp.ParseInputStringToOrder("A,200008,S,6,105");
    ofp.ParseInputStringToOrder("A,200009,S,23,80");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,200009,SOLD,4,100\nXYZ,200001,BOUGHT,4,100\nXYZ,200009,SOLD,6,100\nXYZ,200002,BOUGHT,6,100\nXYZ,200009,SOLD,10,90\nXYZ,200003,BOUGHT,10,90\nXYZ,200009,SOLD,2,90\nXYZ,200004,BOUGHT,2,90\nXYZ,200009,SOLD,1,90\nXYZ,200005,BOUGHT,1,90\nXYZ,200005,B,2,90\nXYZ,200008,S,6,105\nXYZ,200006,S,5,110\nXYZ,200007,S,10,110\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, AddMultiplSellSingleAggresorBuyOrderWithTrade) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,300001,S,5,110");
    ofp.ParseInputStringToOrder("A,300002,S,10,110");
    ofp.ParseInputStringToOrder("A,300003,S,3,105");
    ofp.ParseInputStringToOrder("A,300004,S,7,105");
    ofp.ParseInputStringToOrder("A,200005,B,4,1275");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,300003,SOLD,3,105\nXYZ,200005,BOUGHT,3,105\nXYZ,300004,SOLD,1,105\nXYZ,200005,BOUGHT,1,105\nXYZ,300004,S,6,105\nXYZ,300001,S,5,110\nXYZ,300002,S,10,110\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, DeleteBuyOrderFromEmptyBook) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("X,200005,B,2,1225");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("Delete Order=XYZ,200005,B,2,1225 not found in order book\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}

TEST_F(OrderBookTest, AddMultiplBuyOrderDeleteOneOrder) {
    OrderFeedProcessor ofp;
    ofp.ParseInputStringToOrder("A,200001,B,2,1225");
    ofp.ParseInputStringToOrder("A,200002,B,4,1225");
    ofp.ParseInputStringToOrder("A,200003,B,6,1225");
    ofp.ParseInputStringToOrder("X,200002,B,4,1225");
    ofp.PrintBuyOrderBook();
    ofp.PrintSellOrderBook();
    std::string expected("XYZ,200001,B,2,1225\nXYZ,200003,B,6,1225\n");
    std::string actual{buffer.str()};
    EXPECT_EQ(expected, actual);

}


int main(int argc, char* argv[])
{   
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
