//
// Created by Катя on 6/2/2025.
//

#ifndef TESTRESULT_H
#define TESTRESULT_H

struct Result {
    std::string graphType;
    std::string algorithmName;
    std::string testType;

    int vertices;
    int threads;
    double timeMs;
    double speedup;
    int visited;
};


#endif //TESTRESULT_H
