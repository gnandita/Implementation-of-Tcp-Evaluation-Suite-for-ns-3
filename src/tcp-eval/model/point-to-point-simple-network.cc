#include <cmath>
#include <iostream>
#include <sstream>

// ns3 includes
#include "ns3/log.h"
#include "ns3/point-to-point-simple-network.h"

#include "ns3/node-list.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/string.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PointToPointSimpleNetworkHelper");

PointToPointSimpleNetworkHelper:: PointToPointSimpleNetworkHelper (uint32_t nLeftLeaf,
                                                                   PointToPointHelper leftHelper,
                                                                   uint32_t nRightLeaf,
                                                                   PointToPointHelper rightHelper,
                                                                   PointToPointHelper leftAccessToCoreRouterHelper,
                                                                   PointToPointHelper rightAccessToCoreRouterHelper,
                                                                   PointToPointHelper coreRouterHelper,
                                                                   uint32_t nCrossFlows,
                                                                   PointToPointHelper coreRouterToCrossFlowHelper)

{
  // Create the leaf nodes
  m_leftLeaf.Create (nLeftLeaf);
  m_rightLeaf.Create (nRightLeaf);
  m_accessRouters.Create (2);
  m_coreRouters.Create (4);
  m_crossFlow.Create (nCrossFlows);

  m_leftAccessToCoreRouterDevices = leftAccessToCoreRouterHelper.Install (m_accessRouters.Get (0),m_coreRouters.Get (1));
  m_rightAccessToCoreRouterDevices = rightAccessToCoreRouterHelper.Install (m_accessRouters.Get (1),m_coreRouters.Get (3));

  // Add the left side links
  for (uint32_t i = 0; i < nLeftLeaf; ++i)
    {
      NetDeviceContainer c = leftHelper.Install (m_accessRouters.Get (0),
                                                 m_leftLeaf.Get (i));
      m_leftRouterDevices.Add (c.Get (0));
      m_leftLeafDevices.Add (c.Get (1));
    }

  // Add the right side links
  for (uint32_t i = 0; i < nRightLeaf; ++i)
    {
      NetDeviceContainer c = rightHelper.Install (m_accessRouters.Get (1),
                                                  m_rightLeaf.Get (i));
      m_rightRouterDevices.Add (c.Get (0));
      m_rightLeafDevices.Add (c.Get (1));
    }

  for (uint32_t i = 0; i < nCrossFlows; ++i)
    {
      NetDeviceContainer c = coreRouterToCrossFlowHelper.Install (m_coreRouters.Get (2),
                                                                  m_crossFlow.Get (i));
      m_coreRouterToCrossFlowDevices.Add (c.Get (0));
      m_crossFlowDevices.Add (c.Get (1));
    }

  for (uint32_t i = 0; i < 4; ++i)
    {
      NetDeviceContainer c = coreRouterHelper.Install (m_coreRouters.Get (i),
                                                       m_coreRouters.Get ((i + 1) % 4));
      m_coreRouterDevices.push_back (c);
    }
}

PointToPointSimpleNetworkHelper:: ~PointToPointSimpleNetworkHelper ()
{
}
void
PointToPointSimpleNetworkHelper::InstallStack (InternetStackHelper stack)
{
  stack.Install (m_leftLeaf);
  stack.Install (m_rightLeaf);
  stack.Install (m_accessRouters);
  stack.Install (m_coreRouters);
  stack.Install (m_crossFlow);
}


void
PointToPointSimpleNetworkHelper::AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                                                      Ipv4AddressHelper rightIp,
                                                      Ipv4AddressHelper leftAccesstoCoreIp,
                                                      Ipv4AddressHelper rightAccesstoCoreIp,
                                                      Ipv4AddressHelper coreRouterIp,
                                                      Ipv4AddressHelper crossFlowIp)
{
  // Assign to left side
  for (uint32_t i = 0; i < LeftCount (); ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (m_leftLeafDevices.Get (i));
      ndc.Add (m_leftRouterDevices.Get (i));
      Ipv4InterfaceContainer ifc = leftIp.Assign (ndc);
      m_leftLeafInterfaces.Add (ifc.Get (0));
      m_leftRouterInterfaces.Add (ifc.Get (1));
      leftIp.NewNetwork ();
    }

  // Assign to right side
  for (uint32_t i = 0; i < RightCount (); ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (m_rightLeafDevices.Get (i));
      ndc.Add (m_rightRouterDevices.Get (i));
      Ipv4InterfaceContainer ifc = rightIp.Assign (ndc);
      m_rightLeafInterfaces.Add (ifc.Get (0));
      m_rightRouterInterfaces.Add (ifc.Get (1));
      rightIp.NewNetwork ();
    }

  m_leftAccessToCoreRouterInterfaces = leftAccesstoCoreIp.Assign (m_leftAccessToCoreRouterDevices);
  m_rightAccessToCoreRouterInterfaces = rightAccesstoCoreIp.Assign (m_rightAccessToCoreRouterDevices);

  // Assign to router network
  for (uint32_t i = 0; i < CrossFlowsCount (); ++i)
    {
      NetDeviceContainer ndc;
      ndc.Add (m_coreRouterToCrossFlowDevices.Get (i));
      ndc.Add (m_crossFlowDevices.Get (i));
      Ipv4InterfaceContainer ifc = crossFlowIp.Assign (ndc);
      m_coreRouterToCrossFlowInterfaces.Add (ifc.Get (0));
      m_crossFlowInterfaces.Add (ifc.Get (1));
      crossFlowIp.NewNetwork ();
    }

  for (uint32_t i = 0; i < 4; ++i)
    {
      Ipv4InterfaceContainer ifc = coreRouterIp.Assign (m_coreRouterDevices[i]);
      m_coreRouterInterfaces.push_back (ifc);
      coreRouterIp.NewNetwork ();
    }
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetLeftAccessRouter () const
{ // Get the right side bottleneck router
  return m_accessRouters.Get (0);
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetLeft (uint32_t i) const
{ // Get the i'th left side leaf
  return m_leftLeaf.Get (i);
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetRightAccessRouter () const
{ // Get the right side bottleneck router
  return m_accessRouters.Get (1);
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetRight (uint32_t i) const
{ // Get the i'th right side leaf
  return m_rightLeaf.Get (i);
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetCoreRouter (uint32_t i) const
{ // Get the right side bottleneck router
  return m_coreRouters.Get (i);
}

Ptr<Node> PointToPointSimpleNetworkHelper::GetCrossFlow (uint32_t i) const
{ // Get the i'th right side leaf
  return m_crossFlow.Get (i);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetLeftIpv4Address (uint32_t i) const
{
  return m_leftLeafInterfaces.GetAddress (i);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetRightIpv4Address (uint32_t i) const
{
  return m_rightLeafInterfaces.GetAddress (i);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetCrossFlowIpv4Address (uint32_t i) const
{
  return m_crossFlowInterfaces.GetAddress (i);
}

uint32_t  PointToPointSimpleNetworkHelper::LeftCount () const
{ // Number of left side nodes
  return m_leftLeaf.GetN ();
}

uint32_t  PointToPointSimpleNetworkHelper::RightCount () const
{ // Number of right side nodes
  return m_rightLeaf.GetN ();
}

uint32_t  PointToPointSimpleNetworkHelper::CrossFlowsCount () const
{ // Number of right side nodes
  return m_crossFlow.GetN ();
}

Ipv4Address PointToPointSimpleNetworkHelper::GetRouterToCrossFlowIpv4Address (uint32_t crossFlowIndex) const
{
  return m_coreRouterToCrossFlowInterfaces.GetAddress (crossFlowIndex);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetLeftAccessRouterToCoreRouterIpv4Address () const
{
  return m_leftAccessToCoreRouterInterfaces.GetAddress (0);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetRightAccessRouterToCoreRouterIpv4Address () const
{
  return m_rightAccessToCoreRouterInterfaces.GetAddress (0);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetCoreRouterToLeftAccessRouterIpv4Address () const
{
  return m_leftAccessToCoreRouterInterfaces.GetAddress (1);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetCoreRouterToRightAccessRouterIpv4Address () const
{
  return m_rightAccessToCoreRouterInterfaces.GetAddress (1);
}

Ipv4Address PointToPointSimpleNetworkHelper::GetCoreRouterToCoreRouterIpv4Address (uint32_t i,uint32_t j) const
{
  if (i < j)
    {
      return m_coreRouterInterfaces[i].GetAddress (0);
    }
  else
    {
      return m_coreRouterInterfaces[j].GetAddress (1);
    }
}

}

