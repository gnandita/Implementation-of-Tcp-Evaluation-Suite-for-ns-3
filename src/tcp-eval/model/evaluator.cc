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
  m_device = node->GetDevice (0)->GetObject<PointToPointNetDevice> ();;
  //std::cout << "reached"<<'\n';
  m_queue->TraceConnectWithoutContext ("Enqueue", MakeCallback (&Evaluator::PacketEnqueue, this));
  m_queue->TraceConnectWithoutContext ("Dequeue", MakeCallback (&Evaluator::PacketDequeue, this));
  m_queue->TraceConnectWithoutContext ("Drop", MakeCallback (&Evaluator::PacketDrop, this));
  m_device->TraceConnectWithoutContext ("PhyTxBegin", MakeCallback (&Evaluator::PayloadSize, this));
  
  m_QDrecord = 0;
  m_numQDrecord = 0;
  m_lastQDrecord = Time::Min ();
  m_currentAQM.replace (m_currentAQM.begin (), m_currentAQM.begin () + 5, "");
  AsciiTraceHelper asciiQD;
  std::ifstream infile;
  infile.open("latest_dir.dat");
 std::string default_directory ;
  infile >> default_directory;
  std::cout << default_directory << '\n';

  m_QDfile = asciiQD.CreateFileStream (std::string (default_directory +'/'+ ScenarioName+ "/dat_files/queue_delay/" +tcp_varient+'-' + m_currentAQM + "-qdel.dat").c_str ());
  m_TPrecord = 0;
  m_lastTPrecord = Time::Min ();
  AsciiTraceHelper asciiTP;
  m_TPfile = asciiTP.CreateFileStream (std::string (default_directory +'/'+ ScenarioName+ "/dat_files/throughput/" +tcp_varient+'-' + m_currentAQM + "-throughput.dat").c_str ());
  //AsciiTraceHelper asciiGP;
  //m_GPfile = asciiGP.CreateFileStream (std::string (default_directory + ScenarioName + m_currentAQM + "-goodput.dat").c_str ());
  //AsciiTraceHelper asciiMD;
  //m_metaData = asciiMD.CreateFileStream (std::string (default_directory + ScenarioName + m_currentAQM + "-metadata.dat").c_str ());
  AsciiTraceHelper asciiDT;
  m_dropTime = asciiDT.CreateFileStream (std::string (default_directory +'/'+ ScenarioName+ "/dat_files/enqueue_drop/"+tcp_varient+'-' + m_currentAQM + "-drop.dat").c_str ());
  AsciiTraceHelper asciiET;
  m_enqueueTime = asciiET.CreateFileStream (std::string (default_directory +'/'+ ScenarioName+ "/dat_files/enqueue_drop/"+tcp_varient+'-' + m_currentAQM + "-enqueue.dat").c_str ());
std::cout << "donee" << '\n';
}

Evaluator::~Evaluator (void)
{
}

void
Evaluator::DestroyConnection ()
{
  m_queue->TraceDisconnectWithoutContext ("Enqueue", MakeCallback (&Evaluator::PacketEnqueue, this));
  m_queue->TraceDisconnectWithoutContext ("Dequeue", MakeCallback (&Evaluator::PacketDequeue, this));
  m_queue->TraceDisconnectWithoutContext ("Drop", MakeCallback (&Evaluator::PacketDrop, this));
  m_device->TraceDisconnectWithoutContext ("PhyTxBegin", MakeCallback (&Evaluator::PayloadSize, this));
 /* for (uint32_t i = 0; i < m_sinks.size (); i++)
    {
      m_sinks[i]->TraceDisconnectWithoutContext ("Rx", MakeCallback (&Evaluator::PayloadSize, this));
    }
  for (uint32_t i = 0; i < m_sources.size (); i++)
    {
      *m_metaData->GetStream () << "The flow completion time of flow " << (i + 1)
                                << " = "
                                << (m_sources[i]->GetFlowCompletionTime ()).GetSeconds ()
                                << "\n";
    }*/
}

void
Evaluator::PacketEnqueue (Ptr<const QueueDiscItem> item)
{


 Ptr<Packet> p = item->GetPacket ();

  EvalTimestampTag tag;
  p->AddPacketTag (tag);
  Ptr<const QueueDiscItem> qdi = Ptr<const QueueDiscItem> (dynamic_cast<const QueueDiscItem *> (PeekPointer (item)));
  Ptr<const Ipv4QueueDiscItem> iqdi = Ptr<const Ipv4QueueDiscItem> (dynamic_cast<const Ipv4QueueDiscItem *> (PeekPointer (qdi)));
  *m_enqueueTime->GetStream () << (iqdi->GetHeader ()).GetDestination ()
                               << " "
                               << Simulator::Now ().GetSeconds ()
                               << "\n";
                            //   std::cout << iqdi->GetHeader ().GetDestination () << '\n';

}

void
Evaluator::PacketDequeue (Ptr<const QueueDiscItem> item)
{
  Ptr<Packet> p = item->GetPacket ();
  EvalTimestampTag tag;
  p->RemovePacketTag (tag);
  Time delta = Simulator::Now () - tag.GetTxTime ();
  if (m_lastQDrecord == Time::Min () || Simulator::Now () - m_lastQDrecord > MilliSeconds (10))
    {
      m_lastQDrecord = Simulator::Now ();
      if (m_numQDrecord > 0)
        {
          *m_QDfile->GetStream () << Simulator::Now ().GetSeconds ()
                                  << " "
                                  << (m_QDrecord * 1.0) / (m_numQDrecord * 1.0)
                                  << "\n";
        }
      m_QDrecord = 0;
      m_numQDrecord = 0;
    }
  m_numQDrecord++;
  m_QDrecord += delta.GetMilliSeconds ();
}

void
Evaluator::PacketDrop (Ptr<const QueueDiscItem> item)
{
	//std::cout<<"yes i am in drop"<<'\n';
  Ptr<const QueueDiscItem> qdi = Ptr<const QueueDiscItem> (dynamic_cast<const QueueDiscItem *> (PeekPointer (item)));
  Ptr<const Ipv4QueueDiscItem> iqdi = Ptr<const Ipv4QueueDiscItem> (dynamic_cast<const Ipv4QueueDiscItem *> (PeekPointer (qdi)));
  *m_dropTime->GetStream () << (iqdi->GetHeader ()).GetDestination ()
                            << " "
                            << Simulator::Now ().GetSeconds ()
                            << "\n";
}

void
Evaluator::PayloadSize (Ptr<const Packet> packet/*, const Address & address*/)
{
  //std::cout<<"yes i am in payload"<<'\n';
  /**m_GPfile->GetStream () << address
                          << " "
                          << Simulator::Now ().GetSeconds ()
                          << " "
                          << packet->GetSize ()
                          <<  "\n";*/
  if (m_lastTPrecord == Time::Min () || Simulator::Now () - m_lastTPrecord > MilliSeconds (10))
    {
      if (m_TPrecord > 0)
        {
          *m_TPfile->GetStream () << Simulator::Now ().GetSeconds ()
                                  << " "
                                  << (m_TPrecord * 1.0) / (Simulator::Now () - m_lastTPrecord).GetSeconds ()
                                  << "\n";
        }
      m_lastTPrecord = Simulator::Now ();
      m_TPrecord = 0;
    }
  m_TPrecord += packet->GetSize ();
}



} //namespace ns3
