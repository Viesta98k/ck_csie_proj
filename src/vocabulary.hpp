#include<string>
#include<vector>
#include"csv.hpp"

using namespace std;

class vocabulary{
  public:
    double initDifficultyOf(string vocab);
  private:
    bool isVowel(char input);
    int countSyllable(string vocab);
};
