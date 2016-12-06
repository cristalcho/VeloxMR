#!/usr/bin/python
import code
import sys
import inspect 
from subprocess import call

sys.ps1 ='VeloxMR>'

def put(file_name): 
	call(["dfs", "put", file_name])

def rm(file_name): 
	call(["dfs", "rm", file_name])

def cat(file_name): 
	call(["dfs", "cat", file_name])

def ls(): 
	call(["dfs", "ls"])

def formating(): 
	call(["dfs", "format"])

def mapreduce(file_name, mapf, redf, output): 
	map_src = inspect.getsource(mapf)
	red_src = inspect.getsource(redf)
	map_src.replace(' ', '|')
	map_src.replace('\n', '~')
	call(["__velox_mapreduce", file_name, map_src, red_src, output])
