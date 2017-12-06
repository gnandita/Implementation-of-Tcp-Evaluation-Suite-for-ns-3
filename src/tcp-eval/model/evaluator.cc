/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 * Authors: Ankit Deepak <adadeepak8@gmail.com>
 *          Shravya K. S. <shravya.ks0@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "evaluator.h"
#include "eval-ts.h"
#include <fstream>
#include <iostream>
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "ns3/tcp-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Evaluator");

NS_OBJECT_ENSURE_REGISTERED (Evaluator);

TypeId
Evaluator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Evaluator")
    .SetGroupName ("TcpEvaluationSuite")
  ;
  return tid;
}

Evaluator::Evaluator (std::string ScenarioName, uint32_t numFlows,
                      std::string queueDisc,std::string tcp_varient, uint32_t pktSize,Ptr<QueueDisc> queue,Ptr<Node> node)
{
  m_numFlows = numFlows;
  m_flowsAdded = 0;
  m_packetSize = pktSize;
  m_currentAQM = queueDisc;
  m_queue = queue;
  std::ifstream infile;
  infile.open ("latest_dir.dat");
  std::string default_directory;
  infile >> default_directory;
  m_device = node->GetDevice (0)->GetObject<PointToPointNetDevice> ();
  std::string a = default_directory + '/' + ScenarioName + "/dat_files/enqueue_drop/" + tcp_varient + '-' + m_currentAQM;
  m_queue->TraceConnectWithoutContext ("Enqueue", MakeBoundCallback (&Evaluator::PacketEnqueue,a));
  a = default_directory + "/" + ScenarioName + "/dat_files/queue_delay/" + tcp_varient + "-" + m_currentAQM;
  m_queue->TraceConnectWithoutContext ("Dequeue", MakeBoundCallback (&Evaluator::PacketDequeue,a));
  a = default_directory + '/' + ScenarioName + "/dat_files/enqueue_drop/" + tcp_varient + '-' + m_currentAQM;
  m_queue->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&Evaluator::PacketDrop, a));
  a = default_directory + '/' + ScenarioName + "/dat_files/throughput/" + tcp_varient + '-' + m_currentAQM;
  m_device->TraceConnectWithoutContext ("PhyTxBegin", MakeBoundCallback (&Evaluator::PayloadSize, a));
  m_currentAQM.replace (m_currentAQM.begin (), m_currentAQM.begin () + 5, "");
}

Evaluator::~Evaluator (void)
{
}

void
Evaluator::DestroyConnection ()
{
  std::string a;
  m_queue->TraceDisconnectWithoutContext ("Enqueue", MakeBoundCallback (&Evaluator::PacketEnqueue, a));
  m_queue->TraceDisconnectWithoutContext ("Dequeue", MakeBoundCallback (&Evaluator::PacketDequeue,a));
  m_queue->TraceDisconnectWithoutContext ("Drop", MakeBoundCallback (&Evaluator::PacketDrop, a));
  m_device->TraceDisconnectWithoutContext ("PhyTxBegin", MakeBoundCallback (&Evaluator::PayloadSize,a));
}

void
Evaluator::PacketEnqueue (std::string a,Ptr<const QueueDiscItem> item)
{
  Ptr<Packet> p = item->GetPacket ();
  EvalTimestampTag tag;
  p->AddPacketTag (tag);
  Ptr<const QueueDiscItem> qdi = Ptr<const QueueDiscItem> (dynamic_cast<const QueueDiscItem *> (PeekPointer (item)));
  Ptr<const Ipv4QueueDiscItem> iqdi = Ptr<const Ipv4QueueDiscItem> (dynamic_cast<const Ipv4QueueDiscItem *> (PeekPointer (qdi)));
  Ptr<const Ipv4QueueDiscItem> ipv4Item = DynamicCast<const Ipv4QueueDiscItem> (item);
  Ipv4Header hdr = ipv4Item->GetHeader ();
  Ipv4Address srcIpv4 = hdr.GetSource ();
  Ipv4Address destIpv4 = hdr.GetDestination ();
  uint32_t src = srcIpv4.Get ();
  uint32_t dest = destIpv4.Get ();
  Packet pp = *p;
  TcpHeader h3;
  pp.RemoveHeader (h3);
  uint32_t srcPort = h3.GetSourcePort ();
  uint32_t destPort = h3.GetDestinationPort ();
  Ptr<OutputStreamWrapper> enqueueTimeFile;
  AsciiTraceHelper asciienqueueTime;
  enqueueTimeFile = asciienqueueTime.CreateFileStream (std::string (a + "-" + std::to_string (srcPort) + std::to_string (destPort) +          std::to_string (src) + std::to_string (dest)  + "-enqueue.dat").c_str (),std::ios::out | std::ios::app);
  *enqueueTimeFile->GetStream () << (iqdi->GetHeader ()).GetDestination ()
                                 << " "
                                 << Simulator::Now ().GetSeconds ()
                                 << "\n";
}

void
Evaluator::PacketDequeue (std::string a,Ptr<const QueueDiscItem> item)
{
  Ptr<Packet> p = item->GetPacket ();
  EvalTimestampTag tag;
  p->RemovePacketTag (tag);
  Time delta = Simulator::Now () - tag.GetTxTime ();
  Ptr<const Ipv4QueueDiscItem> ipv4Item = DynamicCast<const Ipv4QueueDiscItem> (item);
  Ipv4Header hdr = ipv4Item->GetHeader ();
  Ipv4Address srcIpv4 = hdr.GetSource ();
  Ipv4Address destIpv4 = hdr.GetDestination ();
  uint32_t src = srcIpv4.Get ();
  uint32_t dest = destIpv4.Get ();
  Packet pp = *p;
  TcpHeader h3;
  pp.RemoveHeader (h3);
  uint32_t srcPort = h3.GetSourcePort ();
  uint32_t destPort = h3.GetDestinationPort ();
  Ptr<OutputStreamWrapper> QDfile;
  AsciiTraceHelper asciiQD;
  QDfile = asciiQD.CreateFileStream (std::string (a + "-" + std::to_string (srcPort) + std::to_string (destPort) + std::to_string (src) + std::to_string (dest)  + "-delay.dat").c_str (),std::ios::out | std::ios::app);

  *QDfile->GetStream () << Simulator::Now ().GetSeconds ()
                        << " "
                        << delta.GetMilliSeconds ()
                        << "\n";

}

void
Evaluator::PacketDrop (std::string a,Ptr<const QueueDiscItem> item)
{
  Ptr<Packet> p = item->GetPacket ();
  Ptr<const Ipv4QueueDiscItem> ipv4Item = DynamicCast<const Ipv4QueueDiscItem> (item);
  Ptr<const QueueDiscItem> qdi = Ptr<const QueueDiscItem> (dynamic_cast<const QueueDiscItem *> (PeekPointer (item)));
  Ptr<const Ipv4QueueDiscItem> iqdi = Ptr<const Ipv4QueueDiscItem> (dynamic_cast<const Ipv4QueueDiscItem *> (PeekPointer (qdi)));
  Ipv4Header hdr = ipv4Item->GetHeader ();
  Ipv4Address srcIpv4 = hdr.GetSource ();
  Ipv4Address destIpv4 = hdr.GetDestination ();
  uint32_t src = srcIpv4.Get ();
  uint32_t dest = destIpv4.Get ();
  Packet pp = *p;
  TcpHeader h3;
  pp.RemoveHeader (h3);
  uint32_t srcPort = h3.GetSourcePort ();
  uint32_t destPort = h3.GetDestinationPort ();
  Ptr<OutputStreamWrapper> dropTimefile;
  AsciiTraceHelper asciiDropTime;
  dropTimefile = asciiDropTime.CreateFileStream (std::string (a + "-" + std::to_string (srcPort) + std::to_string (destPort) + std::to_string (src) + std::to_string (dest)  + "-drop.dat").c_str (),std::ios::out | std::ios::app);
  *dropTimefile->GetStream () << (iqdi->GetHeader ()).GetDestination ()
                              << " "
                              << Simulator::Now ().GetSeconds ()
                              << "\n";
}

void
Evaluator::PayloadSize (std::string a,Ptr<const Packet> packet /*, const Address & address*/)
{
  Packet pp = *packet;
  PppHeader h1;
  pp.RemoveHeader (h1);
  Ipv4Header h2;
  pp.RemoveHeader (h2);
  TcpHeader h3;
  pp.RemoveHeader (h3);
  uint32_t srcPort = h3.GetSourcePort ();
  uint32_t destPort = h3.GetDestinationPort ();
  Ipv4Address ip = h2.GetSource ();
  uint32_t src = ip.Get ();
  Ipv4Address ipDest = h2.GetDestination ();
  uint32_t dest = ipDest.Get ();
  Ptr<OutputStreamWrapper> TPfile;
  AsciiTraceHelper asciiTP;
  TPfile = asciiTP.CreateFileStream (std::string (a + "-" + std::to_string (srcPort) + std::to_string (destPort) + std::to_string (src) + std::to_string (dest)  + "-throughput.dat").c_str (),std::ios::out | std::ios::app);
  *TPfile->GetStream () << Simulator::Now ().GetSeconds ()
                        << " "
                        <<  packet->GetSize ()
                        << "\n";

}

} //namespace ns3
