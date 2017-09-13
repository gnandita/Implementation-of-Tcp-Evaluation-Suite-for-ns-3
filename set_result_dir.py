import sys
import os
import time

cwd = os.getcwd()
main_op_dir = "tcp-eval-output"
sub_op_dir = ""
dir_list = ["dumbbell","parking-lot"]
argv_list = []
argv_list = sys.argv

if len(argv_list) == 2:
	main_op_dir = argv_list[1]
else:
	print "Warning:Default values used."
#print cwd
#print os.path.isdir(main_op_dir)

if not os.path.isdir(main_op_dir):
	os.mkdir(main_op_dir)


os.chdir(cwd + '/' + main_op_dir)
old_cwd = cwd
cwd = os.getcwd()
#print cwd

#dir_list = os.listdir(cwd)
#print dir_list
folder_name = time.strftime("%d_%m_%Y") +'_'+ time.strftime("%H_%M_%S")
#print folder_name
os.mkdir(folder_name)
for item in dir_list:
	os.makedirs(folder_name +'/'+item+ "/dat_files/enqueue_drop")
	os.makedirs(folder_name +'/'+item+ "/dat_files/queue_delay")
	os.makedirs(folder_name +'/'+item+ "/dat_files/throughput")
	os.makedirs(folder_name +'/'+item+ "/png_files/queue_delay")
	os.makedirs(folder_name +'/'+item+ "/png_files/throughput")
	os.makedirs(folder_name +'/'+item+ "/ellipse/dat_files")
	os.makedirs(folder_name +'/'+item+ "/ellipse/png_files")




readme_file = open(cwd + '/' + folder_name + '/readme',"w" )
buff = "Created on:\nDate-> "+ time.strftime("%d-%m-%Y") +"\nTime-> " + time.strftime("%H:%M:%S")
readme_file.write(buff)
readme_file.close()

folder_name_file = open(old_cwd+'/latest_dir.dat','w')
folder_name_file.write(main_op_dir+'/'+folder_name)
folder_name_file.close()
