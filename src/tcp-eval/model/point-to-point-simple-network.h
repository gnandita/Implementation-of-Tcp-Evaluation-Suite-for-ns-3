#ifndef POINT_TO_POINT_SIMPLE_NETWORK_HELPER_H
#define POINT_TO_POINT_SIMPLE_NETWORK_HELPER_H

#include <string>

#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-interface-container.h"

namespace ns3 {

/**
 * \ingroup point-to-point-layout
 *
 * \brief A helper to make it easier to create a simple network topology
 * with p2p links
 */
class PointToPointSimpleNetworkHelper
{
public:
  PointToPointSimpleNetworkHelper (uint32_t nLeftLeaf,
                                PointToPointHelper leftHelper,
                                uint32_t nRightLeaf,
                                PointToPointHelper rightHelper,
                                PointToPointHelper leftAccessToCoreRouterHelper,
                                PointToPointHelper rightAccessToCoreRouterHelper,
                                PointToPointHelper coreRouterHelper,
                                uint32_t nCrossFlows,                                
                                PointToPointHelper coreRouterToCrossFlowHelper);

  ~PointToPointSimpleNetworkHelper ();
  void InstallStack (InternetStackHelper stack);
  void AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                                                   Ipv4AddressHelper rightIp,
                                                   Ipv4AddressHelper leftAccesstoCoreIp,
                                                   Ipv4AddressHelper rightAccesstoCoreIp,
						   Ipv4AddressHelper coreRouterIp,
                                                   Ipv4AddressHelper crossFlowIp);
  Ptr<Node> GetLeftAccessRouter () const;
  Ptr<Node> GetLeft (uint32_t i) const;
  Ptr<Node> GetRightAccessRouter () const;
  Ptr<Node> GetRight (uint32_t i) const;
  Ptr<Node> GetCoreRouter (uint32_t i) const;
  Ptr<Node> GetCrossFlow (uint32_t i) const;
  Ipv4Address GetLeftIpv4Address (uint32_t i) const;
  Ipv4Address GetRightIpv4Address (uint32_t i) const;
  Ipv4Address GetCrossFlowIpv4Address (uint32_t i) const;
  uint32_t  LeftCount () const;
  uint32_t  RightCount () const;
  uint32_t  CrossFlowsCount () const;
  Ipv4Address GetRouterToCrossFlowIpv4Address (uint32_t crossFlowIndex) const;
  Ipv4Address GetLeftAccessRouterToCoreRouterIpv4Address () const;
  Ipv4Address GetRightAccessRouterToCoreRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToLeftAccessRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToRightAccessRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToCoreRouterIpv4Address (uint32_t i,uint32_t j) const;

private:

  NodeContainer          m_leftLeaf;                                    //!< Left Leaf nodes
  NetDeviceContainer     m_leftLeafDevices;                             //!< Left Leaf NetDevices
  NodeContainer          m_rightLeaf;                                   //!< Right Leaf nodes
  NetDeviceContainer     m_rightLeafDevices;                            //!< Right Leaf NetDevices
  NodeContainer          m_accessRouters;
  NodeContainer          m_coreRouters;   
  NodeContainer          m_crossFlow;
  NetDeviceContainer     m_crossFlowDevices;
                          
  NetDeviceContainer     m_routerDevices;                               //!< Routers NetDevices
  NetDeviceContainer     m_leftRouterDevices;                           //!< Left router NetDevices
  NetDeviceContainer     m_rightRouterDevices;                          //!< Right router NetDevices
  NetDeviceContainer     m_leftAccessToCoreRouterDevices;
  NetDeviceContainer     m_rightAccessToCoreRouterDevices;
  std::vector<NetDeviceContainer> m_coreRouterDevices;
  NetDeviceContainer     m_coreRouterToCrossFlowDevices;

  Ipv4InterfaceContainer m_leftLeafInterfaces;                          //!< Left Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_leftRouterInterfaces;                        //!< Left router interfaces (IPv4)
  Ipv4InterfaceContainer m_rightLeafInterfaces;                         //!< Right Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_rightRouterInterfaces;                       //!< Right router interfaces (IPv4)
  Ipv4InterfaceContainer m_leftAccessToCoreRouterInterfaces;            
  Ipv4InterfaceContainer m_rightAccessToCoreRouterInterfaces;
  Ipv4InterfaceContainer m_coreRouterToCrossFlowInterfaces;
  Ipv4InterfaceContainer m_crossFlowInterfaces;
  std::vector<Ipv4InterfaceContainer> m_coreRouterInterfaces;
  
};

}
#endif 
