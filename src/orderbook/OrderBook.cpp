/**
 * @brief This module implements simulator of exchange
 * @file OrderBook.cpp
 *
 */


#include "orderbook/OrderBook.hpp"
#include "client/Trader.hpp"
#include "utils/Exception.hpp"
#include "utils/Constants.hpp"
#include "utils/Side.hpp"


namespace pyxchange
{


namespace py = boost::python;


/**
 * @brief Constructor
 *
 */
OrderBook::OrderBook( const ClientVectorConstPtr& clients_ ):
    clients{ clients_ }
{

}


/**
 * @brief Constructor
 *
 */
OrderBook::OrderBook( const ClientVectorConstPtr& clients_, const Logger& logger_ ):
      logger{ logger_ }
    , clients{ clients_ }
{

}


/**
 * @brief FIXME
 *
 */
void OrderBook::createOrder( const TraderPtr& trader, const py::dict& decoded )
{
    OrderPtr order;

    try
    {
        order = std::make_shared<Order>( trader, decoded, false );
    }
    catch( const side::WrongSide& )
    {
        logger.warning( boost::format( format::f1::logWrongSide ) % trader->toString() );

        trader->notifyError( format::f0::wrongSide.str() );
    }
    catch( const pyexc::OrderIdError& )
    {
        logger.warning( boost::format( format::f1::logWrongOrderId ) % trader->toString() );

        trader->notifyError( format::f0::wrongOrderId.str() );
    }
    catch( const pyexc::PriceError& )
    {
        logger.warning( boost::format( format::f1::logWrongPrice ) % trader->toString() );

        trader->notifyError( format::f0::wrongPrice.str() );
    }
    catch( const pyexc::QuantityError& )
    {
        logger.warning( boost::format( format::f1::logWrongQuantity ) % trader->toString() );

        trader->notifyError( format::f0::wrongQuantity.str() );
    }

    if( order && side::isBid( order->side ) )
    {
        insertOrder<BidOrderContainer, AskOrderContainer>( bidOrders, askOrders, trader, order );
    }
    else if( order && side::isAsk( order->side ) )
    {
        insertOrder<AskOrderContainer, BidOrderContainer>( askOrders, bidOrders, trader, order );
    }
}


/**
 * @brief FIXME
 *
 */
void OrderBook::marketOrder( const TraderPtr& trader, const py::dict& decoded )
{
    OrderPtr order;

    try
    {
        order = std::make_shared<Order>( trader, decoded, true );
    }
    catch( const side::WrongSide& )
    {
        logger.warning( boost::format( format::f1::logWrongSide ) % trader->toString() );

        trader->notifyError( format::f0::wrongSide.str() );
    }
    catch( const pyexc::QuantityError& )
    {
        logger.warning( boost::format( format::f1::logWrongQuantity ) % trader->toString() );

        trader->notifyError( format::f0::wrongQuantity.str() );
    }

    if( order && side::isBid( order->side ) )
    {
        handleExecution<AskOrderContainer>( askOrders, order );
    }
    else if( order && side::isAsk( order->side ) )
    {
        handleExecution<BidOrderContainer>( bidOrders, order );
    }
}


} /* namespace pyxchange */



/* EOF */

