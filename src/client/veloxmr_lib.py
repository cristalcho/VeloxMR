#!/usr/bin/python
import code
import sys
import inspect 
from subprocess import call

sys.ps1 ='VeloxMR>'

def put(file_name): 
	call(["veloxdfs", "put", file_name])

def rm(file_name): 
	call(["veloxdfs", "rm", file_name])

def cat(file_name): 
	call(["veloxdfs", "cat", file_name])

def ls(): 
	call(["veloxdfs", "ls"])

def format(): 
	call(["veloxdfs", "format"])

def show(file_name): 
	call(["veloxdfs", "show", file_name])

def mapreduce(file_name, mapf, redf, output, pmap = "", amap = ""): 
	map_src = inspect.getsource(mapf)
	red_src = inspect.getsource(redf)
	map_src.replace(' ', '|')
	map_src.replace('\n', '~')

	red_src.replace(' ', '|')
	red_src.replace('\n', '~')

	pmap_src = "NULL"
	amap_src = "NULL"
	if pmap is not "":
		pmap_src = inspect.getsource(pmap)
		pmap_src.replace(' ', '|')
		pmap_src.replace('\n', '~')

	if amap is not "":
		amap_src = inspect.getsource(amap)
		amap_src.replace(' ', '|')
		amap_src.replace('\n', '~')

	call(["__velox_mapreduce", file_name, map_src, red_src, output, pmap_src, amap_src])
