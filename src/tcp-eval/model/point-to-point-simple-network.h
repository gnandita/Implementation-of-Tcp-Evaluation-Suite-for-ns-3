/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Dharmendra Kumar Mishra <dharmendra.nitk@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// Define an object to create a parking-lot topology.

#ifndef POINT_TO_POINT_SIMPLE_NETWORK_HELPER_H
#define POINT_TO_POINT_SIMPLE_NETWORK_HELPER_H

#include <string>

#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/ipv6-interface-container.h"

namespace ns3 {

/**
 * \ingroup point-to-point-layout
 *
 * \brief A helper to make it easier to create a parking-lot topology
 * with p2p links
 */
class PointToPointSimpleNetworkHelper
{
public:
  /**
   * Create a PointToPointSimpleNetworkHelper in order to easily create
   * parking-lot topologies using p2p links
   *
   * \param nLeftLeaf number of left side leaf nodes in the parking-lot
   *
   * \param leftHelper PointToPointHelper used to install the links
   *                   between the left leaf nodes and the left-most
   *                   router
   *
   * \param nRightLeaf number of right side leaf nodes in the parking-lot
   *
   * \param rightHelper PointToPointHelper used to install the links
   *                    between the right leaf nodes and the right-most
   *                    router
   *
   * \param nBottleneck number of bottleneck links in the parking-lot
   *                    between the left leaf nodes and right leaf nodes
   *
   * \param nCrossFlowsAtRouter number of cross flows passing through a router
   *
   * \param crossHelper PointToPointHelper used to install the link
   *                    between the router and cross sources
   *
   * \param bottleneckHelper PointToPointHelper used to install the link
   *                         between the inner-routers, usually known as
   *                         the bottleneck link
   */
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
  void AssignIpv6Addresses (Ipv6Address network, Ipv6Prefix prefix);
  Ptr<Node> GetLeftAccessRouter () const;
  Ptr<Node> GetLeft (uint32_t i) const;
  Ptr<Node> GetRightAccessRouter () const;
  Ptr<Node> GetRight (uint32_t i) const;
  Ptr<Node> GetCoreRouter (uint32_t i) const;
  Ptr<Node> GetCrossFlow (uint32_t i) const;
  Ipv4Address GetLeftIpv4Address (uint32_t i) const;
  Ipv4Address GetRightIpv4Address (uint32_t i) const;
  Ipv4Address GetCrossFlowIpv4Address (uint32_t i) const;

  Ipv6Address GetLeftIpv6Address (uint32_t i) const;
  Ipv6Address GetRightIpv6Address (uint32_t i) const;
  Ipv6Address GetCrossFlowIpv6Address (uint32_t i) const;
  uint32_t  LeftCount () const;
  uint32_t  RightCount () const;
  uint32_t  CrossFlowsCount () const;

  Ipv4Address GetRouterToCrossFlowIpv4Address (uint32_t crossFlowIndex) const;
  Ipv4Address GetLeftAccessRouterToCoreRouterIpv4Address () const;
  Ipv4Address GetRightAccessRouterToCoreRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToLeftAccessRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToRightAccessRouterIpv4Address () const;
  Ipv4Address GetCoreRouterToCoreRouterIpv4Address (uint32_t i,uint32_t j) const;

  Ipv6Address GetRouterToCrossFlowIpv6Address (uint32_t crossFlowIndex) const;
  Ipv6Address GetLeftAccessRouterToCoreRouterIpv6Address () const;
  Ipv6Address GetRightAccessRouterToCoreRouterIpv6Address () const;
  Ipv6Address GetCoreRouterToLeftAccessRouterIpv6Address () const;
  Ipv6Address GetCoreRouterToRightAccessRouterIpv6Address () const;
  Ipv6Address GetCoreRouterToCoreRouterIpv6Address (uint32_t i,uint32_t j) const;

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
  Ipv4InterfaceContainer m_leftAccessToCoreRouterInterfaces;            //!< Router interfaces (IPv4)
  Ipv4InterfaceContainer m_rightAccessToCoreRouterInterfaces;
  Ipv4InterfaceContainer m_coreRouterToCrossFlowInterfaces;
  Ipv4InterfaceContainer m_crossFlowInterfaces;
  std::vector<Ipv4InterfaceContainer> m_coreRouterInterfaces;

  Ipv6InterfaceContainer m_leftLeafInterfaces6;
  Ipv6InterfaceContainer m_leftRouterInterfaces6;
  Ipv6InterfaceContainer m_rightLeafInterfaces6;
  Ipv6InterfaceContainer m_rightRouterInterfaces6;
  Ipv6InterfaceContainer m_leftAccessToCoreRouterInterfaces6;
  Ipv6InterfaceContainer m_rightAccessToCoreRouterInterfaces6;
  Ipv6InterfaceContainer m_coreRouterToCrossFlowInterfaces6;
  Ipv6InterfaceContainer m_crossFlowInterfaces6;
  std::vector<Ipv6InterfaceContainer> m_coreRouterInterfaces6;

};
}
#endif /* POINT_TO_POINT_PARKING_LOT_HELPER_H */
