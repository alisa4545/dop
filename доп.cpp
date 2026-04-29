#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

struct Participant {
    string name;
    double spent = 0.0;
    double norm = 0.0;
};

string clean(const string& s) {
    if (s.empty()) return "";
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        string cleaned = clean(token);
        if (!cleaned.empty()) tokens.push_back(cleaned);
    }
    return tokens;
}

int main() {
    setlocale(LC_ALL, "Russian");
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");

    if (!inputFile.is_open()) {
        cerr << "Ошибка: не удалось открыть input.txt" << endl;
        return 1;
    }

    string firstLine;
    getline(inputFile, firstLine);

    vector<string> tokens = split(firstLine, ' ');
    if (tokens.empty()) {
        cerr << "Ошибка: пустая первая строка" << endl;
        return 1;
    }

    int numParticipants = stoi(tokens[0]);
    vector<string> participantNames;
    map<string, Participant> participants;

    for (int i = 1; i <= numParticipants && i < tokens.size(); ++i) {
        string name = tokens[i];
        participantNames.push_back(name);
        participants[name].name = name;
        participants[name].spent = 0.0;
        participants[name].norm = 0.0;
    }

    string line;

    while (getline(inputFile, line)) {
        if (clean(line).empty()) continue;

        size_t slashPos = line.find('/');

        string beforeSlash = (slashPos == string::npos) ? line : line.substr(0, slashPos);
        istringstream iss(beforeSlash);

        string payer;
        double amount;

        iss >> payer;
        payer = clean(payer);
        iss >> amount;

        participants[payer].spent += amount;

        vector<string> excluded;
        if (slashPos != string::npos) {
            string afterSlash = line.substr(slashPos + 1);
            excluded = split(afterSlash, ',');
        }

        vector<string> included;
        for (const auto& name : participantNames) {
            bool isExcluded = false;
            for (const auto& ex : excluded) {
                if (ex == name) {
                    isExcluded = true;
                    break;
                }
            }
            if (!isExcluded) {
                included.push_back(name);
            }
        }

        if (!included.empty()) {
            double share = amount / included.size();
            for (const auto& name : included) {
                participants[name].norm += share;
            }
        }
    }

    outputFile << fixed << setprecision(1);
    for (const auto& name : participantNames) {
        outputFile << name << " " << participants[name].spent << " " << participants[name].norm << endl;
    }
    outputFile << endl;

    vector<pair<string, double>> debtors;
    vector<pair<string, double>> creditors;

    for (const auto& name : participantNames) {
        double balance = participants[name].spent - participants[name].norm;
        if (balance > 0.009) {
            creditors.push_back({ name, balance });
        }
        else if (balance < -0.009) {
            debtors.push_back({ name, -balance });
        }
    }

    size_t i = 0, j = 0;
    while (i < debtors.size() && j < creditors.size()) {
        double transfer = min(debtors[i].second, creditors[j].second);

        if (transfer > 0.009) {
            outputFile << debtors[i].first << " " << transfer << " " << creditors[j].first << endl;
        }

        debtors[i].second -= transfer;
        creditors[j].second -= transfer;

        if (debtors[i].second < 0.009) i++;
        if (creditors[j].second < 0.009) j++;
    }

    inputFile.close();
    outputFile.close();
    cout << "Расчет завершен. Результаты в output.txt" << endl;

    return 0;
}