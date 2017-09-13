#include <stdlib.h>
#include <string>
#include <iostream>
#include "ns3/core-module.h"

using namespace ns3;
	
int main(int argc, char *argv[])
{
 std::string scenarioName = "dumbbell";
 std::string useAqm = "false"; //use bool
 std::string queueName;
 std::string Aqm_Name;
 CommandLine cmd;
 cmd.AddValue ("scenarioName", "Scenario Name ", scenarioName);
 cmd.AddValue ("useAqm", "use Aqm", useAqm);
 cmd.AddValue ("Aqm_Name", "Name of the AQM if use Aqm is true", Aqm_Name);
 cmd.Parse (argc,argv);
 if(useAqm=="true")
 {
   if(Aqm_Name=="RedQueueDisc" ||  Aqm_Name=="PieQueueDisc" || Aqm_Name=="CoDelQueueDisc"||  	  Aqm_Name=="FqCoDelQueueDisc")
   {
     queueName=Aqm_Name;
   }
  else 
   { std::cout<<"Invalid AQM Name";
     exit (1); 
   }
 }
else
 {
  queueName="PfifoFastQueueDisc";
 }
 std::string set_dest = "python set_result_dir.py";
 system (set_dest.c_str());
 std::ifstream infile;
 infile.open("latest_dir.dat");
 std::string default_directory ;
 infile >> default_directory;
 uint32_t i=0,expt_num, num_Tcp;
  std::string transport_prot[] = {"TcpNewReno"} ;
  std::string run="";
  std::string gnuPlot = "";
  num_Tcp=1;
  // std::string gnuPlotR = "";
  for(expt_num =1;expt_num<=1;expt_num++)
   {
    gnuPlot = "gnuplot -e 'set terminal png size 640,640; set output \"ResultWith2TcpVariant-"+scenarioName+"-"+queueName+"\"; set xrange[]   reverse; plot ";
   // gnuPlotR = "gnuplot -e 'set terminal png size 640,640; set output \"TCPEvalOutput/"+ scenarioName +"/EXPT-"+std::to_string(expt_num)+"/  	     ResultR.png\"; set xrange[] reverse; plot ";        
   for(i=0; i<1;i++)
    {  	
      if(useAqm=="false")
       {
        run = "./waf --run \"drive-"+scenarioName+" "+"--useAqm="+useAqm+" "+"--tcp_variant="+transport_prot[i]+"\"";}
      else
       {
       run = "./waf --run \"drive-"+scenarioName+" "+"--useAqm="+useAqm+" "+"--tcp_variant="+transport_prot[i]+" "+"--Aqm_Name="+Aqm_Name+"\"";
       }
      
      system (run.c_str());	
      std::string proQdelThr = std::string ("python src/tcp-eval/generate-ellipseinput.py ") + scenarioName + " " + transport_prot[i] +" "+ 	  queueName +" "+ default_directory;
      std::string proEllipse = std::string ("python src/tcp-eval/ellipsemaker ") + scenarioName + " " + transport_prot[i] +" "+ queueName  	 +" "+default_directory;
     std::string Result = proQdelThr ;
     std::string Ellipse = proEllipse ;             
     //std::string ResultR = proQdelThr + " R";
     //std::string EllipseR = proEllipse + " R";
     system(Result.c_str());
     system(Ellipse.c_str());              
     //system(ResultR.c_str());
     //system(EllipseR.c_str());
     std::string graphName =std::string("\""+default_directory+"/"+scenarioName+"/ellipse/dat_files/"+transport_prot[i]+"-"+queueName 
     +"-ellipse.dat\" title \"")+transport_prot[i]+std::string("\" with lines "); 
     //std::string graphNameR =std::string("\"TCPEvalOutput/"+ scenarioName +"/EXPT-"+std::to_string(expt_num)+"/"+transport_prot[i]  	  	+"_ellipseR.dat\" title \"")+transport_prot[i]+std::string("\" with lines "); 
     if (i!=num_Tcp-1)
      {
       gnuPlot = gnuPlot + graphName + std::string(",");
       //gnuPlotR = gnuPlotR + graphNameR + std::string(",");
      }
     else
      {
       gnuPlot += graphName + std::string("'");
       //gnuPlotR += graphNameR + std::string("'");
      }
    }
    system(gnuPlot.c_str());
    //system(gnuPlotR.c_str());
  }    
  return(0);
}
