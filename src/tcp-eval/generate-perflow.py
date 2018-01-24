import sys
import os
import shutil

scenario_name = sys.argv[1]
dir_name = sys.argv[2]

l=[]
path=dir_name+"/"+scenario_name+"/ellipse/ellipse_dat/"
ellipse_png_dir = dir_name+"/"+scenario_name+"/png_files/throughput/"

for queue_delay_filename in os.listdir(path):
 filename=queue_delay_filename[-34:] 
 flag=0 
 gnuplot = "gnuplot -e 'set terminal png size 740,740;set output "+ "\""+ellipse_png_dir+"Result-"+filename[:-4]+".png"+"\""+";set xrange[] reverse;plot "
 for fn in os.listdir(path):  
  if fn.endswith(filename):
    if fn not in l:
     flag = 1
     l.append (fn)
     gnuplot+= "\""+path+"/"+fn+"\" with lines title\" "+fn+"\", " 
 gnuplot=gnuplot[:-3]+"\'"
 if flag == 0:
   gnuplot="" 
 print gnuplot
 os.system(gnuplot)
 
     
      
     
