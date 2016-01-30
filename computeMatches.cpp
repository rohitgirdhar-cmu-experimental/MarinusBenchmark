#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>

#define MAX_MATCHES 100

using namespace std;

template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {
  // ref: http://stackoverflow.com/a/12399290/1492614
  // initialize original index locations
  vector<size_t> idx(v.size());
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

int main(int argc, char* argv[]) {
  
  string hashfpath = argv[1];
  string outfpath = argv[2];

  ifstream fin(hashfpath);
  string hash;
  vector<boost::dynamic_bitset<> > hashes;
  while (fin >> hash) {
    hashes.push_back(boost::dynamic_bitset<>(hash));
  }
  cout << "Read all hashes" << endl;
  cout.flush();
  fin.close();

  int qid = 0;

  ofstream fout(outfpath);
  for (auto hash : hashes) {
    vector<int> hamdists(hashes.size(), 0);
    #pragma omp parallel for
    for (size_t i = 0; i < hashes.size(); i++) {
      hamdists[i] = (hashes[i] ^ hash).count();
    }
    vector<size_t> order = sort_indexes(hamdists);
    for (int i = 0; i < MAX_MATCHES; i++) {
      fout << order[i] << " ";      
    }
    fout << endl;

    cout << "Done for " <<  ++qid << " / " << hashes.size() << endl;
  }
  fout.close();
}

