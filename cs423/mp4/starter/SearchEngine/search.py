'''
search.py

@author Max Docauer (docauer2), Jiageng "Nemo" Li (jli65), Dennis McWherter (dmcwhe2)
'''

import linecache, operator, sys

def substituteForId(docId, sub):
	docId += 1
	line = linecache.getline(sub, docId)
	toks = line.split()

	if len(toks) < 2:
		print "This is kind of a problem, ain't it"
		return "INVALID PAGE ENTRY"

	title = toks[1]
	i = 1

	while True:
		i += 1
		if toks[i] == ":":
			break
		title += " "
		title += toks[i]

	return title

def search(indexFile, key):
	file = open(indexFile)
	for line in file:
		split = line.split(None, 1)
    #if line.find(key) != -1:
		if split[0] == key:
			print line
			return line
	return None

def rank(line, articleFile):
  toks = line.split()
  retval = {}
  count = 0;
  for x in toks:
    x = x.strip()
    count += 1
    if count == 1:
      continue
    vals = x.split("-")
    name  = substituteForId(int(vals[0]), articleFile)
    frequency = vals[1]
    retval[name] = int(frequency) 
  #sorted_vals = sorted(retval.iteritems(), key=operator.itemgetter(1))
  #sorted_vals
  return retval.items()

def main():
  argv = sys.argv
  length = len(argv)
  if length < 4:
    print "Must provide at least one search key, an article file, and an index file."

  articleFile = argv[length - 2]
  indexFile = argv[length - 1]

  ranks = []

  for x in argv[1:-2]:
    line = search(indexFile, x)
    if line is not None:
      ranks.extend(rank(line, articleFile))

  #print ranks
  ranks.sort(key=operator.itemgetter(1), reverse=True)

  count = 0
  for x in ranks:
    if count >= 10:
      break
    print x
    count += 1

if __name__ == "__main__":
  main()

