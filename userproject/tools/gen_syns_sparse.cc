//--------------------------------------------------------------------------
/*! \file gen_syns_sparse.cc

  \brief This file generates the arrays needed for sparse connectivity. The connectivity is saved to a file for each variable and can then be read to fill the struct of connectivity.
*/ 
//--------------------------------------------------------------------------
//gdb -tui --args ./gen_syns_sparse 10 10 0.5 0.001 0.0001 include/g
//g++ -Wall -Winline -g -I../lib/include/numlib -o gen_syns_sparse gen_syns_sparse.cc


using namespace std;

#include <iostream>
#include <fstream>
#include <string.h>
#include "randomGen.h"
#include "gauss.h"
#include <vector>

randomGen R;
randomGauss RG;

int main(int argc, char *argv[])
{
  if (argc != 7)
    {
      cerr << "usage: gen_syns_sparse <n1> <n2> ";
      cerr << "<prob. of connection> ";
      cerr << "<mean strength> ";
      cerr << "<strength jitter> <outfile>" << endl;
      exit(1);
    }
  
  int n1= atoi(argv[1]);
  int n2= atoi(argv[2]);
  float psyn= atof(argv[3]);
  float meangsyn= atof(argv[4]);
  float jitter= atof(argv[5]);
  //ofstream os(argv[6], ios::binary);
  ofstream os(argv[6], ios::binary);
  char filename_index[100];
  char filename_postcount[100]; 
  char filename_nonopt[100];
  char filename_info[100];
  strcpy(filename_index,argv[6]);
  strcat(filename_index,"_postind");
  strcpy(filename_postcount,argv[6]);
  strcat(filename_postcount,"_postIndInG");
  strcpy(filename_nonopt,argv[6]);
  strcat(filename_nonopt,"_nonopt");
  strcpy(filename_info,argv[6]);
  strcat(filename_info,"_info");
  ofstream os_index(filename_index, ios::binary);
  ofstream os_postcount(filename_postcount, ios::binary);
  ofstream os_nonopt(filename_nonopt, ios::binary);
  ofstream os_info(filename_info);
  
  float gsyn;
  float *g_alltoall= new float[n1*n2];
  std::vector<float> g;

  cerr << "# call was: ";
  for (int i= 0; i < argc; i++) cerr << argv[i] << " ";
  cerr << endl;

  std::vector<unsigned int>PreIdx;
  std::vector<unsigned int>maxInColS;
       	
  unsigned long int ctr=0;
  std::vector<unsigned int> postIndInG;
  std::vector<unsigned int> postIndex;
  int maxInColI;
  postIndInG.push_back(0);  
  for (int i= 0; i < n1; i++) {
    maxInColI=0;
    for (int j= 0; j < n2; j++) {
      if (R.n() < psyn) {
	gsyn= meangsyn+jitter*RG.n();
	g.push_back(gsyn);
	postIndex.push_back(j);
	maxInColI++;
	g_alltoall[i*n2+j]= gsyn;
	//cout << "g for" << i << " to "<< j<< " is " << gsyn <<endl;
      }      
      else {
	g_alltoall[i*n2+j]= 0.0f;
      }
    }
    ctr=ctr+maxInColI;
    postIndInG.push_back(ctr);
  }
  
  size_t sz = g.size();
  cout << "vect.size: " << sz << endl;
  os_info.write((char *)&sz,sizeof(size_t));
  os.write(reinterpret_cast<const char*>(&g[0]), sz * sizeof(g[0]));
  sz = postIndex.size();
  cout << "ind size: " << sz << endl;
  os_index.write(reinterpret_cast<const char*>(&postIndex[0]), sz * sizeof(postIndex[0]));
  sz = postIndInG.size();
  cout << "count size: " << sz << endl;
  os_postcount.write(reinterpret_cast<const char*>(&postIndInG[0]), sz * sizeof(postIndInG[0]));

  os_nonopt.write((char *)g_alltoall, n1*n2*sizeof(float));
  os.close();
  os_index.close();
  os_postcount.close();
  os_nonopt.close();
  cout << "ctr: " << ctr << endl;
  os_info.close();
  delete[] g_alltoall;
  
  return 0;
}