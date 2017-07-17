#ifndef DEF_H_INCLUDED
#define DEF_H_INCLUDED

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <time.h>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <chrono>
#include <unistd.h>
//header pour les threads
#include <random>
#include <thread>
#include <mutex>
//header gestion des dossiers
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#define ISCONFUSION true
#define NB_OF_INTENSITES 8//8
#define NB_OF_RUN 2//100
#define MAX_EPOCH 1000//1000
#define NB_EPOCHS_TEST 2
#define NB_OF_TEST_PATTERNS 9
#define NB_OF_PATTERNS 90
#define NB_OF_EMOTION 6
#define NB_OF_FACES 10
#define NB_OF_REINJECTIONS 0

#define VERBOSE 0
#define BIAS_UID 0

#define private public //trolololol

#endif // DEF_H_INCLUDED
