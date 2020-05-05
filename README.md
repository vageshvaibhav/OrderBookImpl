1) For the order matching engine, i have taken the below approach for matching and book keeping the orders :
  
     /*Read each order from input file inturn
	 * if order is a sell order , look in to the book of Buy orders, if match happens,output the matched order
	 * if no match then the current sell order goes to Sell order book
	 * if order is a Buy order , look in to the book of Sell orders, if match happens,output the matched order
	 * if no match then the current Buy order goes to Sell order book*/

     /*
	 * Buy Orders Set is sorted on price/time as :
	 * 1.Descending on Price (High to Low)
         * 2. If two buy orders price are equal, ascending on time(first come first serve)
         *
	 * Sell orders Set is sorted on price/time as :
	 * 1.Ascending on price (low to high)
         * 2. If two sell orders price are equal , ascending on time (first come first serve)
	 */


Performance:
     * Any AddOrder operation would involve
     * O(1) const unordered_map lookup to find order book for particular company
     * O(log n) emplace complexity if no trade
     * Less than O(n) in case of Trade due to iteration till full order is processed 
     /* Any DeleteOrder  operation would involve
     * O(1) const unordered_map lookup to find order book for particular company
     * O(log n) find/erase complexity
     */

2) I have taken the assumption that order of fields separated by "," in the input file will be as per input.csv.

3) input.csv is attached along with other header and CPP file as mentioned.

4) If order book crosses, then I am logging both legs of the trade which got executed due to crossing orders.

Company,OrderId,transaction,quantity,price
XYZ,100005,SOLD,2,1025
XYZ,100008,BOUGHT,2,1025


Compilation and Application Usage:
For simplicity, I have kept most of the things in header file so we will have to just compile main source file containing main function
$ g++ --version
g++ (GCC) 4.9.1 20140922 (Red Hat 4.9.1-10)

Application Compilation:
$ g++ application_main.cpp -std=c++14 -o OrderBookApplication

Application Execution:
$ ./OrderBookApplication input.csv 
XYZ,100005,SOLD,2,1025
XYZ,100008,BOUGHT,2,1025
XYZ,100007,SOLD,1,1025
XYZ,100008,BOUGHT,1,1025
Delete failed as Order =XYZ,100008,B,3,1050 not found in buy side order book
Delete failed as Order =XYZ,100005,S,2,1025 not found in sell side order book


Unit TestCases with google test framework:

I have added few basic test scenarios for simple order book tests. Final console output is validated for expected results

Unit Test compilation:
$ g++ test_main.cpp -std=c++14 -lgtest -lgtest_main -o OrderBookApplicationTest

Unit Test execution: 
$ ./OrderBookApplicationTest
[==========] Running 8 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 8 tests from OrderBookTest
[ RUN      ] OrderBookTest.AddSingleBuyOrder
[       OK ] OrderBookTest.AddSingleBuyOrder (0 ms)
[ RUN      ] OrderBookTest.AddSingleSellOrder
[       OK ] OrderBookTest.AddSingleSellOrder (0 ms)
[ RUN      ] OrderBookTest.AddSingleBuySellOrderNoTrade
[       OK ] OrderBookTest.AddSingleBuySellOrderNoTrade (0 ms)
[ RUN      ] OrderBookTest.AddSingleBuySellOrderWithTrade
[       OK ] OrderBookTest.AddSingleBuySellOrderWithTrade (1 ms)
[ RUN      ] OrderBookTest.AddMultiplBuySingleAggresorSellOrderWithTrade
[       OK ] OrderBookTest.AddMultiplBuySingleAggresorSellOrderWithTrade (0 ms)
[ RUN      ] OrderBookTest.AddMultiplSellSingleAggresorBuyOrderWithTrade
[       OK ] OrderBookTest.AddMultiplSellSingleAggresorBuyOrderWithTrade (0 ms)
[ RUN      ] OrderBookTest.DeleteBuyOrderFromEmptyBook
[       OK ] OrderBookTest.DeleteBuyOrderFromEmptyBook (0 ms)
[ RUN      ] OrderBookTest.AddMultiplBuyOrderDeleteOneOrder
[       OK ] OrderBookTest.AddMultiplBuyOrderDeleteOneOrder (0 ms)
[----------] 8 tests from OrderBookTest (1 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 1 test case ran. (1 ms total)
[  PASSED  ] 8 tests.


Google Test installation:
1. Get the googletest framework
wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz

2. Unpack and build google test
tar xf release-1.8.0.tar.gz
cd googletest-release-1.8.0
cmake -DBUILD_SHARED_LIBS=ON .
make

3. "Install" the headers and libs on your system.
This step might differ from distro to distro, so make sure you copy the headers and libs in the correct directory. I accomplished this by checking where Debians former gtest libs were located. But I'm sure there are better ways to do this. Note: make install is dangerous and not supported
sudo cp -a include/gtest /usr/include
sudo cp -a libgtest_main.so libgtest.so /usr/lib/

4. Update the cache of the linker
... and check if the GNU Linker knows the libs
sudo ldconfig -v | grep gtest
If the output looks like this:
libgtest.so.0 -> libgtest.so.0.0.0
libgtest_main.so.0 -> libgtest_main.so.0.0.0
, everything is fine.
gTestframework is now ready to use. Just don't forget to link your project against the library by setting -lgtest as linker flag and optionally, if you did not write your own test mainroutine, the explicit -lgtest_main flag.
