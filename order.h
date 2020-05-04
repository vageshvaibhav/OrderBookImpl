#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Order class with overloaded operators. To keep the class declaration simple, only required members has been added
class order
{
	
	uint64_t m_orderId;
	mutable uint32_t m_quantity;
	uint64_t m_price; //Assuming we are taking m_prices in unsigned integers by multiplying some factor as per points of precision
	std::string m_company, m_side;
    uint64_t m_timestamp_order_placed;
	public:
    order(uint64_t ordId,uint32_t quantity,uint64_t price,const std::string& side
            , const std::string& company = std::string("XYZ")) 
        : m_orderId(ordId) , m_quantity(quantity), m_price(price), m_side(side), m_company(company), m_timestamp_order_placed(0) 
	{
	}
	//Getters
	uint64_t getOrderId() const {return m_orderId;} 
	uint32_t getQuantity()const {return m_quantity;} 
	uint64_t getPrice() const {return m_price;}
	string getCompany() const {return m_company;}
	string getSide() const{return m_side;}
	
	//Setters
	void setQuantity(uint64_t quantity) const {m_quantity=quantity;}
	void setPrice(uint64_t price){m_price=price;}
    void setTimestampOrderPlaced(uint64_t time) {m_timestamp_order_placed = time;}
	
	//Overload operators
	friend bool operator < (const order& lhs,const order& rhs);
	friend bool operator > (const order& lhs,const order& rhs);
	friend bool operator == (const order& lhs,const order& rhs);
	friend bool operator != (const order& lhs,const order& rhs);
	friend ostream & operator<< (ostream & ost,const order & ord);
	friend istream & operator>> (istream & ist,order & ord);
	
};

    bool operator< (const order& lhs,const order& rhs)
	{
		if(lhs.m_price < rhs.m_price) return true;
		else if (lhs.m_price == rhs.m_price && lhs.m_timestamp_order_placed < rhs.m_timestamp_order_placed){return true;} //To Sort Sell orders Ascending on m_price,time
		else return false;
	}
	
	bool operator> (const order& lhs,const order& rhs)
	{
		if(lhs.m_price > rhs.m_price) return true;
		else if (lhs.m_price == rhs.m_price && lhs.m_timestamp_order_placed < rhs.m_timestamp_order_placed){return true;}  //To sort Buy order on 1. Descending on Price
		else return false;                                                           //2.if Price are same , Ascending on m_orderId(time)
	}
	
	bool operator== (const order& lhs,const order& rhs)
	{
		return (lhs.m_orderId == rhs.m_orderId && lhs.m_quantity == rhs.m_quantity && lhs.m_price == rhs.m_price && lhs.m_side == rhs.m_side );
	}
	
	bool operator!= (const order& lhs,const order& rhs)
	{
		return !(lhs == rhs);
	}
	
	//Modify less and greater functor for order to sort the set of set of orders on m_price/time
	namespace std
    {
		template <>
		struct less<order>
		{
			bool operator() (const order& lhs, const order& rhs)
			{
			  return lhs < rhs;
			}
		};
		template<>
		struct greater<order>
		{
			bool operator() (const order& lhs, const order& rhs)
			{
			  return lhs > rhs;
			}
		};
    }

	ostream & operator<< (ostream& ost,const order& ord)
	{
		ost << ord.m_company <<","<<ord.m_orderId<<","<<ord.m_side<<","<<ord.m_quantity<<","<<ord.m_price;
		return ost;
	}
	
	istream & operator>> (istream & ist,order & ord)
	{
		ist>> ord.m_company >>ord.m_orderId >>ord.m_side>>ord.m_quantity>>ord.m_price;
		return ist;
	}
