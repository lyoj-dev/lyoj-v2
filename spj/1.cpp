#include<bits/stdc++.h>
using namespace std;
string trim(string str) {
    while (str.size() && str.back() == ' ') str.pop_back();
    return str;
}
int main(int argc,char** argv) {
    string input_file = argv[1];
    string output_file = argv[2];
    string answer_file = argv[3];
    int score = atoi(argv[4]);
    string score_file = argv[5];
    string info_file = argv[6];
    ifstream outin(output_file.c_str()), ansin(answer_file.c_str());
    ofstream scoreout(score_file.c_str()), infoout(info_file.c_str());
    int line = 0;
    while (!ansin.eof()) { 
        line++;
    	string a = "", b = "";
        getline(outin, a);
        getline(ansin, b);
        while (a.find("\r") != -1) a.replace(a.find("\r"), 1, "");
        while (b.find("\r") != -1) b.replace(b.find("\r"), 1, "");
        a = trim(a), b = trim(b);
        for (int i = 0; i < a.size(); i++) if (a[i] < 0) a.erase(i, i + 1), i--;
        if (a != b) {
            scoreout << -1;
            if (a.size() >= 100) a = a.substr(0, 100) + "...";
            if (b.size() >= 100) b = b.substr(0, 100) + "...";
            infoout << "In line #" << line << ": Read \"" << a << "\" but expected \"" << b << "\"";
            return 0;
        }
    } 
    infoout << "Accepted | OK!" << endl;
    scoreout << score << endl;
    return 0;
}
