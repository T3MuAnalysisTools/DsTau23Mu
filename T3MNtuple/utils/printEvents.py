from ROOT import *
import sys
import argparse
from branch_list import t3m_branch_list

DATA_TYPE = ['float','bool','int','double','string','unsigned int']
VECTOR_TYPE = ['vector<double>','vector<bool>','vector<float>','vector<int>','vector<string>','vector<unsigned int>']
DOUBLE_VECTOR_TYPE = ['vector<vector<bool> >','vector<vector<float> >','vector<vector<int> >','vector<vector<double> >','vector<vector<string> >','vector<vector<unsigned int> >']
TRIPLE_VECTOR_TYPE = ['vector<vector<vector<int> > >','vector<vector<vector<bool> > >','vector<vector<vector<float> > >','vector<vector<vector<double> > >','vector<vector<vector<string> > >','vector<vector<vector<unsigned int> > >']

def printBranchList(_tree):
   _branches = _tree.GetListOfBranches()    # Get list of branches
   for i in xrange(_branches.GetEntries()):
      print _branches.At(i).GetName()


#----- Argument parser --------
parser = argparse.ArgumentParser()

parser.add_argument("--inputFile", help="input root file")

args = parser.parse_args()
#------------------------------
FILENAME = args.inputFile

infile = TFile(FILENAME, "READ")

_tree = infile.Get('T3MTree/t3mtree')
nevents = _tree.GetEntriesFast()
#------------------------------

_tree.GetEntry(100)

for brname in t3m_branch_list:
   tmp = getattr(_tree, brname)
   print '--------------------------------------'
   print brname+" ("+type(tmp).__name__+")"
   print '--------------------------------------'


   if (type(tmp).__name__ in DATA_TYPE):
      print tmp
   elif (type(tmp).__name__ in VECTOR_TYPE):
      if (tmp.size()==0): sys.stdout.write("Empty!")
      else: sys.stdout.write("* ")
      for j in xrange(tmp.size()): sys.stdout.write(str(tmp.at(j))+' ')
      sys.stdout.write("\n")

   elif (type(tmp).__name__ in DOUBLE_VECTOR_TYPE):
      if (tmp.size()==0): sys.stdout.write("Empty!")
      else: sys.stdout.write("**\n")
      for j in xrange(tmp.size()):
         tmp_arr = tmp.at(j)
         if (tmp_arr.size()==0): sys.stdout.write("Empty!")
         else: sys.stdout.write("  * ")
         for k in xrange(tmp_arr.size()):
            sys.stdout.write(str(tmp_arr.at(k))+" ")
         sys.stdout.write("\n")
      sys.stdout.write("\n")

   elif (type(tmp).__name__ in TRIPLE_VECTOR_TYPE):
      if (tmp.size()==0): sys.stdout.write("Empty!")
      else: sys.stdout.write("***\n")
      for j in xrange(tmp.size()):
         tmp_arr = tmp.at(j)
         if (tmp_arr.size()==0): sys.stdout.write("  Empty!")
         else: sys.stdout.write("  **\n")
         for k in xrange(tmp_arr.size()):
            tmp_sec_arr = tmp_arr.at(k)
            if (tmp_sec_arr.size()==0): sys.stdout.write("    Empty!")
            else: sys.stdout.write("    * ")
            for l in xrange(tmp_sec_arr.size()):
               sys.stdout.write(str(tmp_sec_arr.at(l))+" ")
            sys.stdout.write("\n")
         sys.stdout.write("\n")
      sys.stdout.write("\n")
