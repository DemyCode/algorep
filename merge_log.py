import os
import glob
import sys

if __name__ == '__main__':
    debug_files = glob.glob("debug/debug_*.txt")
    lines = []
    for file in debug_files:
        fstream = open(file, 'r')
        lines.append(fstream.readlines())
    lines = [item for sublist in lines for item in sublist]
    lines = [(int(x.split(': ')[0]), x) for x in lines]
    lines.sort()
    output = open('debug/merged.txt', 'w')
    for line in lines:
        print(line[1], end='')
        output.write(line[1])
    output.close()
