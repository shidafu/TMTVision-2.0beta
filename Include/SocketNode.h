///////////////////////////////////////////////////
/** \file SocketNode.h
 *  \brief Implement socket operations in INode object
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/13 19:34 build\n
 */
///////////////////////////////////////////////////


#pragma once
#include "INode.h"
#include <boost/asio.hpp>
using namespace boost;

///////////////////////////////////////////////////
/** \class SocketNode
*  \brief Implement socket operations in INode object
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/06/13 19:34
*/
#pragma once
template <typename T1, typename T2>
class SocketNode : virtual public IProducer<T1>, virtual public IConsumer<T2>
{
public:
	SocketNode();
	~SocketNode();
};

