#include "order_book.h"
typedef std::unordered_map<uint64_t,uint64_t > MapOfOrderTime;
class OrderFeedProcessor
{
    OrderBook m_ord_book;
    MapOfOrderTime m_orderEntryTimestampMap;
    public:
    /* Read the data from the file. */
    /* The logic is to read the input csv file line by line
     * if order is a sell order , look in to the book of Buy orders, if match happens,output the matched order
     * if no match then the current sell order goes to Sell order book
     * if order is a Buy order , look in to the book of Sell orders, if match happens,output the matched order
     * if no match then the current Buy order goes to Sell order book
     * Buy Orders sorted on price/time as :
     * 1.Descending on Price (High to Low)2. If two buy orders price are equal, ascending on time(first come first serve)
     * Sell orders sorted on price/time as :
     * 1.Ascending on price (low to high)2. If two sell orders price are equal , ascending on time (first come first serve)
     */
    bool ParseInputFileToOrders(const std::string& input_file)
    {
        ifstream input(input_file.c_str());
        if(!input.is_open())
        {   
            cout<<"Unable to open input/output files"<<endl;
            return false;
        }   
        std::string currValue;
        bool is_parsing_successful = true;
        while (input >> currValue && is_parsing_successful)
        {
            is_parsing_successful = ParseInputStringToOrder(currValue);
        }
        return is_parsing_successful;
    }

    //m_orderEntryTimestampMap is used for order id to time mapping
    bool ParseInputStringToOrder(const std::string& input_string)
    {
        string side, order_action;
        uint64_t order_id, price;
        uint32_t quantity;
        size_t pos = 0;
        std::vector<std::string> tokens;
        std::string delimiter = ",";
        size_t last = 0; 
        size_t next = 0; 
        while ((next = input_string.find(delimiter, last)) != string::npos) 
        { 
            tokens.push_back(input_string.substr(last, next-last)); 
            last = next + 1; 
        } 
        tokens.push_back(input_string.substr(last));

        if (tokens.size() == 5) //order_action, order_id, side, quantity, price
        {
            try
            {
                order_action = tokens[0];
                order_id = std::stoull(tokens[1]);
                side = tokens[2];
                quantity = std::stoul(tokens[3]);
                price = std::stoull(tokens[4]);
            }
            catch(std::exception& e)
            {
                std::cout << "Parsing input failed due to error=" << e.what() << std::endl;
                return false;
            }
        }
        else
        {
            std::cout << "Parsing input failed due to invalid number of parsed tokens=" << tokens.size() << std::endl;
            return false;
        }

        order ord(order_id, quantity, price, side); //default company XYZ if not provided
        auto timestamp = duration_cast< nanoseconds >(system_clock::now().time_since_epoch()).count(); // timestamp of order entry for sorting or price/time
        ord.setTimestampOrderPlaced(timestamp);
        if ( order_action == "A" )
        {
            if (side == "B" || side == "S") 
            {
                m_orderEntryTimestampMap[order_id] = timestamp;
                return (side == "B") ? m_ord_book.AddBuyOrder(ord) : m_ord_book.AddSellOrder(ord) ;
            }
            else
            {
                std::cout << "Invalid Order Side=" << side << std::endl;
                return false;
            }
        }

        else if ( order_action == "X" )
        {
            if (side == "B" || side == "S")
            {   
                auto iter_oets = m_orderEntryTimestampMap.find(order_id); 
                if (iter_oets != m_orderEntryTimestampMap.end())
                {
                    auto timestamp = iter_oets->second;
                    ord.setTimestampOrderPlaced(timestamp);
                    auto retval = (side == "B") ? m_ord_book.DeleteBuyOrder(ord) : m_ord_book.DeleteSellOrder(ord) ;
                    if (retval)
                    {
                        m_orderEntryTimestampMap.erase(iter_oets);
                    }
                    return retval;
                }
                else
                {
                    std::cout << "Delete Order=" << ord << " not found in order book" << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid Order Side=" << side << std::endl;
                return false;
            } 
        }

        else
        {
            std::cout << "Invalid Order Action=" << order_action << std::endl;
            return false;
        }
    }

    void PrintBuyOrderBook()
    {
        for (const auto& comp_ord_bk : m_ord_book.m_perCompanyBuyOrderBook)
        {
            for (const auto& order : comp_ord_bk.second)
            {
                std::cout << order << std::endl;
            }
        }
    }

    void PrintSellOrderBook()
    {
        for (const auto& comp_ord_bk : m_ord_book.m_perCompanySellOrderBook)
        {
            for (const auto& order : comp_ord_bk.second)
            {
                std::cout << order << std::endl;
            }
        }
    }
};
