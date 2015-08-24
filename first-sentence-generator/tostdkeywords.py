#!/usr/bin/python

import sys

ENCODE = 'utf-8'

# each line 
# kw1 kw2 kw3 senLen1 senLen2
# comments are allowed: after #

def uwrite(fout, text):
    fout.write(text.encode(ENCODE))

def tostdkeywords(infile, outfile):
    line_no = 0
    fout = open(outfile, 'w')

    for line in open(infile):
        line_no += 1
        ori_line = line
        line = line.decode(ENCODE)
        cm_pos = line.find('#')
        if cm_pos != -1:
            line = line[0:cm_pos]
        line = line.strip()
        if len(line) == 0:
            continue
        fields = line.split()
        keywords = []
        senLens = []
        for field in fields:
            if field.isdigit():
                senLens.append(field)
            else:
                keywords.append(field)

        if len(keywords) == 0 or len(senLens) == 0:
            print 'invalid line, %d' % line_no
            print ori_line
            continue

        keytext = '*'.join([' '.join(kw) for kw in keywords])
        for senLen in senLens:
            uwrite(fout, keytext + '\t' + senLen + '\n')

    fout.close()

if __name__ == '__main__':
    print 'convert human readable keywords and sentence lens to cpp readable keywords and sentence lens'
    infile = sys.argv[1]
    outfile = sys.argv[2]
    tostdkeywords(infile, outfile)


