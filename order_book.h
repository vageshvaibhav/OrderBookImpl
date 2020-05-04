#include"order.h"
#include <unordered_map>
//All the typedefs used in main program
typedef std::set<order,std::less<order> > SetOfSellOrder;
typedef std::set<order,std::greater<order> > SetOfBuyOrder;
typedef std::unordered_map<std::string,SetOfSellOrder > MapOfSellOrder;
typedef std::unordered_map<std::string,SetOfBuyOrder > MapOfBuyOrder;
class OrderBook
{
    MapOfBuyOrder m_perCompanyBuyOrderBook;
    MapOfSellOrder m_perCompanySellOrderBook;
    public:

    /*Buy and Sell Order Books are maintained as Set of sorted price/time records.
     * Buy Orders sorted on price/time as :
     * 1.Descending on Price (High to Low)2. If two buy orders price are equal, ascending on time(first come first serve)
     * Sell orders sorted on price/time as :
     * 1.Ascending on price (low to high)2. If two sell orders price are equal , ascending on time (first come first serve)
     * Any AddOrder operation would involve 
     * O(1) const unordered_map lookup to find order book for particular company
     * O(log n) emplace complexity if no trade
     * Less than O(n) in case of Trade due to iteration till full order is processed
     */
    bool AddBuyOrder(const order& ord)
    {
        //std::cout << "AddBuyOrder : Buy order received with order details=" << ord << std::endl;
        bool enriched = false;
        SetOfBuyOrder  tempSetBuy;
        auto itSell = m_perCompanySellOrderBook.find(ord.getCompany());
        if(itSell != m_perCompanySellOrderBook.end() && (ord.getPrice() >= itSell->second.begin()->getPrice()))
        {   
            auto tradeTimeStamp = duration_cast< nanoseconds >(system_clock::now().time_since_epoch()).count(); // timestamp of trade Ers
            for(auto itrset = itSell->second.begin(); itrset != itSell->second.end() ;)
            {   
                if((ord.getPrice() >= (*itrset).getPrice()) && (*itrset).getQuantity()!= 0)
                {   
                    if(ord.getQuantity() >= (*itrset).getQuantity())
                    {   
                        order buyTradeExecutionReport(ord.getOrderId(),(*itrset).getQuantity(),(*itrset).getPrice(),"BOUGHT",ord.getCompany());
                        order sellTradeExecutionReport((*itrset).getOrderId(),(*itrset).getQuantity(),(*itrset).getPrice(),"SOLD",(*itrset).getCompany());
                        buyTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        sellTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        std::cout <<sellTradeExecutionReport<<endl;
                        std::cout <<buyTradeExecutionReport<<endl;
                        if ((ord.getQuantity() - (*itrset).getQuantity()) == 0) 
                        {
                            itrset = itSell->second.erase(itrset);
                            ord.setQuantity(0);
                            enriched = true;
                            break;
                        }
                        ord.setQuantity(ord.getQuantity() - (*itrset).getQuantity());

                        itrset = itSell->second.erase(itrset);
                        enriched=true;

                    }
                    else if(ord.getQuantity() < (*itrset).getQuantity())
                    {   
                        order buyTradeExecutionReport(ord.getOrderId(),ord.getQuantity(),(*itrset).getPrice(),"BOUGHT",ord.getCompany());
                        order sellTradeExecutionReport((*itrset).getOrderId(),ord.getQuantity(),(*itrset).getPrice(),"SOLD",(*itrset).getCompany());
                        buyTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        sellTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        std::cout <<sellTradeExecutionReport<<endl;
                        std::cout <<buyTradeExecutionReport<<endl;
                        itrset->setQuantity((*itrset).getQuantity() - ord.getQuantity());
                        ord.setQuantity(0);
                        enriched =true;
                        break;
                    }
                }
                else
                {
                    //Now All Sell orders are > current Buy order, so break from here
                    break;
                }
            }

            if (ord.getQuantity() > 0 )
            {
                auto itBuy = m_perCompanyBuyOrderBook.find(ord.getCompany());
                if (itBuy == m_perCompanyBuyOrderBook.end())
                {   
                    tempSetBuy.clear();
                    tempSetBuy.emplace(ord);
                    m_perCompanyBuyOrderBook.emplace(std::pair<std::string,SetOfBuyOrder>(ord.getCompany(),tempSetBuy));
                }
                else
                {   
                    itBuy->second.emplace(ord);
                }
            }
        }
        if((itSell == m_perCompanySellOrderBook.end()) || (!enriched))
        {   
            auto itBuy = m_perCompanyBuyOrderBook.find(ord.getCompany());
            if (itBuy == m_perCompanyBuyOrderBook.end())
            {   
                tempSetBuy.clear();
                tempSetBuy.emplace(ord);
                m_perCompanyBuyOrderBook.emplace(std::pair<std::string,SetOfBuyOrder>(ord.getCompany(),tempSetBuy));
            }
            else
            {   
                itBuy->second.emplace(ord);
            }

        }
        return true;
    }

    bool AddSellOrder(const order& ord)
    {
        //std::cout << "AddSellOrder :: Sell order received with order details=" << ord << std::endl;
        SetOfSellOrder tempSetSell;
        bool enriched =false;
        auto itBuy = m_perCompanyBuyOrderBook.find(ord.getCompany());
        if(itBuy != m_perCompanyBuyOrderBook.end() && (ord.getPrice() <= itBuy->second.begin()->getPrice()))
        {   
            auto tradeTimeStamp = duration_cast< nanoseconds >(system_clock::now().time_since_epoch()).count(); // timestamp of trade Ers
            for(auto itrset=itBuy->second.begin();itrset!=itBuy->second.end();)
            {   
                if((ord.getPrice() <= (*itrset).getPrice()) && (*itrset).getQuantity()!= 0)
                {   
                    if(ord.getQuantity() >= (*itrset).getQuantity())
                    {   
                        order sellTradeExecutionReport(ord.getOrderId(),(*itrset).getQuantity(),(*itrset).getPrice(),"SOLD",ord.getCompany());
                        order buyTradeExecutionReport((*itrset).getOrderId(),(*itrset).getQuantity(),(*itrset).getPrice(),"BOUGHT",(*itrset).getCompany());
                        buyTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        sellTradeExecutionReport.setTimestampOrderPlaced(tradeTimeStamp);
                        std::cout <<sellTradeExecutionReport<<endl;
                        std::cout <<buyTradeExecutionReport<<endl;
                        if((ord.getQuantity() - (*itrset).getQuantity()) == 0)
                        {
                            itrset = itBuy->second.erase(itrset);
                            ord.setQuantity(0);
                            enriched = true;
                            break;
                        }
                        ord.setQuantity(ord.getQuantity() - (*itrset).getQuantity());

                        itrset = itBuy->second.erase(itrset);
                        enriched=true;

                    }
                    else if(ord.getQuantity() < (*itrset).getQuantity())
                    {   
                        order sellTradeExecutionReport(ord.getOrderId(),ord.getQuantity(),(*itrset).getPrice(),"SOLD",ord.getCompany());
                        order buyTradeExecutionReport((*itrset).getOrderId(),ord.getQuantity(),(*itrset).getPrice(),"BOUGHT",(*itrset).getCompany());
                        std::cout <<sellTradeExecutionReport<<endl;
                        std::cout <<buyTradeExecutionReport<<endl;
                        itrset->setQuantity((*itrset).getQuantity() - ord.getQuantity());
                        ord.setQuantity(0);
                        enriched =true;
                        break;
                    }
                }
                else
                {
                    //Now All Buy orders are < current sell order, so break from here
                    break;
                }
            }

            if (ord.getQuantity() > 0) 
            {
                auto itSell = m_perCompanySellOrderBook.find(ord.getCompany());
                if(itSell == m_perCompanySellOrderBook.end())
                {   
                    tempSetSell.clear();
                    tempSetSell.emplace(ord);
                    m_perCompanySellOrderBook.emplace(std::pair<std::string,SetOfSellOrder>(ord.getCompany(),tempSetSell));
                }
                else
                {   
                    itSell->second.emplace(ord);
                }
            }
        }

        if((itBuy == m_perCompanyBuyOrderBook.end()) || (!enriched))
        {   

            auto itSell = m_perCompanySellOrderBook.find(ord.getCompany());
            if(itSell == m_perCompanySellOrderBook.end())
            {   
                tempSetSell.clear();
                tempSetSell.emplace(ord);
                m_perCompanySellOrderBook.emplace(std::pair<std::string,SetOfSellOrder>(ord.getCompany(),tempSetSell));
            }
            else
            {   
                itSell->second.emplace(ord);
            }

        }
        return true;
    }

     /* Any DeleteOrder  operation would involve 
     * O(1) const unordered_map lookup to find order book for particular company
     * O(log n) find/erase complexity 
     */
    bool DeleteBuyOrder(const order& ord)
    {
        //std::cout << "DeleteBuyOrder :: Buy order received with order details=" << ord << std::endl;
        auto company = ord.getCompany();
        auto itBuy = m_perCompanyBuyOrderBook.find(company);
        if (itBuy != m_perCompanyBuyOrderBook.end())
        {
            auto iter = itBuy->second.find(ord);
            if (iter != itBuy->second.end())
            {
                itBuy->second.erase(iter);
                return true;
            }
            else
            {
                std::cout << "Delete failed as Order =" << ord << " not found in buy side order book" << std::endl;
            }
        }
        else
        {
            std::cout << "Delete failed as Order with company=" << company << " not found in buy side order book" << std::endl;
            return false;
        }
        return true;
    }

    bool DeleteSellOrder(const order& ord)
    {
        //std::cout << "DeleteSellOrder :: Sell order received with order details=" << ord << std::endl;
        auto company = ord.getCompany();
        auto itSell = m_perCompanySellOrderBook.find(company);
        if (itSell != m_perCompanySellOrderBook.end())
        {   
            auto iter = itSell->second.find(ord);
            if (iter != itSell->second.end())
            {
                itSell->second.erase(iter);
                return true;
            }
            else
            {
                std::cout << "Delete failed as Order =" << ord << " not found in sell side order book" << std::endl;
                return false;
            }
        }   
        else
        {   
            std::cout << "Delete failed as Order with company=" << company << " not found in sell side order book" << std::endl;
            return false;
        }  
        return true;
    }

    friend class OrderFeedProcessor;
};
