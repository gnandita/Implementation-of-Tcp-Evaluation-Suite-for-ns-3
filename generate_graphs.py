import sys
import os

cwd = os.getcwd()

if len(sys.argv) == 3:
	dir_name = sys.argv[1]
	scenarioName = sys.argv[2]
elif len(sys.argv) == 2:
	file = open('latest_dir.dat','r')
	dir_name = file.read()
	file.close()
	scenarioName = sys.argv[1]
else:
	print "minimum 1 cmdline input required"
	sys.exit(0)

try:
	os.chdir(dir_name+'/'+scenarioName)
except:
	print "Invalid output folder"
	sys.exit(0)

qdel_dat_dir = os.getcwd() +"/dat_files/queue_delay/"
throughput_dat_dir = os.getcwd() +"/dat_files/throughput/"
ellipse_dat_dir = os.getcwd() +"/ellipse/dat_files/"

qdel_files_list = os.listdir(qdel_dat_dir)
throughput_files_list = os.listdir(throughput_dat_dir)


qdel_len = len(qdel_files_list)
throughput_len = len(throughput_files_list)

qdel_png_dir = os.getcwd() +"/png_files/queue_delay/"
throughput_png_dir = os.getcwd() +"/png_files/throughput/"
ellipse_png_dir = os.getcwd() +"/ellipse/png_files/"

gnuPlotE = "gnuplot -e 'set terminal png size 640,640; set output"+ "\""+ellipse_png_dir+"Result"+".png"+"\""+"; set xrange[]; plot "

for item in qdel_files_list:
	qdel_len = qdel_len - 1
	split = item.split("-")
	tcp_name = split[0]
	aqm_name = split[1]
	
	gnuPlot = "gnuplot -e 'set terminal png size 640,640; set output "+ "\""+qdel_png_dir+tcp_name+"-"+aqm_name+".png"+"\""+"; set xrange[]; plot "+"\""+qdel_dat_dir+tcp_name+"-"+aqm_name+"-qdel.dat"+"\""+" title "+"\""+tcp_name+"-"+aqm_name+"\""+" with lines '";
	os.system(gnuPlot)

	os.system("python "+cwd+"/src/tcp-eval/generate-ellipseinput.py "+scenarioName +" "+tcp_name+" "+aqm_name+" "+os.getcwd())
	os.system("python "+cwd+"/src/tcp-eval/ellipsemaker "+scenarioName +" "+tcp_name+" "+aqm_name+" "+os.getcwd())
	
	graph_name = "\""+ellipse_dat_dir+tcp_name+"-"+aqm_name+"-ellipse.dat"+"\""+" title "+"\""+tcp_name+"-"+aqm_name+"\""+" with lines"
	
	if qdel_len ==0 :
		gnuPlotE = gnuPlotE + graph_name + "'"
	else:
		gnuPlotE = gnuPlotE + graph_name + ","

os.system(gnuPlotE)

for item in throughput_files_list:
	split = item.split("-")
	tcp_name = split[0]
	aqm_name = split[1]
	gnuPlot = "gnuplot -e 'set terminal png size 640,640; set output "+ "\""+throughput_png_dir+tcp_name+"-"+aqm_name+".png"+"\""+"; set xrange[]; plot "+"\""+throughput_dat_dir+tcp_name+"-"+aqm_name+"-throughput.dat"+"\""+" title "+"\""+tcp_name+"-"+aqm_name+"\""+" with lines '";
	os.system(gnuPlot)





