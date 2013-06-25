/*
Copyright (C) 2011 Baptiste Wicht

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>

#include "Timer.hpp"
#include "functions.hpp"

#define LOOPS 2000000//000
#define REPEAT 17
#define WARMUP 3

using std::cout;
using std::endl;
using inlining::Timer;

double benchFunctionCall() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke();
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call : " << mean << "ms " << endl;

    return mean;
}

// long parameters

double benchFunctionCallWithOnelongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  1 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithTwolongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  2 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithThreelongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  3 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithFourlongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  4 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithFivelongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  5 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithSixlongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5, 6);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  6 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithSevenlongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5, 6, 7);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  7 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithEightlongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5, 6, 7, 8);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  8 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithNinelongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5, 6, 7, 8, 9);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  9 long parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithTenlongParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokelong(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with 10 long parameters : " << mean << "ms " << endl;

    return mean;
}

//int parameters

double benchFunctionCallWithOneintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  1 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithTwointParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  2 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithThreeintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  3 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithFourintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  4 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithFiveintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  5 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithSixintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5, 6);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  6 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithSevenintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5, 6, 7);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  7 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithEightintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5, 6, 7, 8);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  8 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithNineintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5, 6, 7, 8, 9);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  9 int parameters : " << mean << "ms " << endl;

    return mean;
}

double benchFunctionCallWithTenintParameters() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invokeInt(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with 10 int parameters : " << mean << "ms " << endl;

    return mean;
}

//---

double benchLibraryCall() {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            libraryTest();
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Library function call : " << mean << "ms " << endl;

    return mean;
}

//---

double benchMemberFunction(Poo& poo) {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            poo.second();
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Member Function call : " << mean << "ms " << endl;

    return mean;
}

double benchVirtualMemberFunction(Poo& poo) {
    double mean = 0;

    for (int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            poo.first();
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Virtual Function call : " << mean << "ms " << endl;

    return mean;
}

double bench32() {
    double mean = 0;

    ThirtyTwo o1;
    for (unsigned int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke32(o1);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  32 bytes parameters : " << mean << "ms " << endl;

    return mean;
}

double bench64() {
    double mean = 0;

    SixtyFour o2;
    for (unsigned int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke64(o2);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with  64 bytes parameters : " << mean << "ms " << endl;

    return mean;
}

double bench128() {
    double mean = 0;

    HundredTwentyEight o3;
    for (unsigned int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke128(o3);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with 128 bytes parameters : " << mean << "ms " << endl;

    return mean;
}

double bench256() {
    double mean = 0;

    TwoHundredFiftySix o4;
    for (unsigned int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke256(o4);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with 256 bytes parameters : " << mean << "ms " << endl;

    return mean;
}

double bench512() {
    double mean = 0;

    FiveHundredTwelve o5;
    for (unsigned int a = 0; a < REPEAT + WARMUP; a++) {
        Timer start;
        for (unsigned long i = 0; i < LOOPS; i++) {
            invoke512(o5);
        }
        if (a >= WARMUP) {
            mean += start.elapsed();
        }
    }

    mean /= REPEAT;

    cout << "Function call with 512 bytes parameters : " << mean << "ms " << endl;

    return mean;
}

int main(int, const char*[]) {
    Pee* pee = new Pee();

    double baseCall = benchFunctionCall();
    double libraryCall = benchLibraryCall();
    double memberCall = benchMemberFunction(*pee);
    double virtualCall = benchVirtualMemberFunction(*pee);

    delete pee;

    double longCalls[10];

    longCalls[0] = benchFunctionCallWithOnelongParameters();
    longCalls[1] = benchFunctionCallWithTwolongParameters();
    longCalls[2] = benchFunctionCallWithThreelongParameters();
    longCalls[3] = benchFunctionCallWithFourlongParameters();
    longCalls[4] = benchFunctionCallWithFivelongParameters();
    longCalls[5] = benchFunctionCallWithSixlongParameters();
    longCalls[6] = benchFunctionCallWithSevenlongParameters();
    longCalls[7] = benchFunctionCallWithEightlongParameters();
    longCalls[8] = benchFunctionCallWithNinelongParameters();
    longCalls[9] = benchFunctionCallWithTenlongParameters();

    double intCalls[10];

    intCalls[0] = benchFunctionCallWithOneintParameters();
    intCalls[1] = benchFunctionCallWithTwointParameters();
    intCalls[2] = benchFunctionCallWithThreeintParameters();
    intCalls[3] = benchFunctionCallWithFourintParameters();
    intCalls[4] = benchFunctionCallWithFiveintParameters();
    intCalls[5] = benchFunctionCallWithSixintParameters();
    intCalls[6] = benchFunctionCallWithSevenintParameters();
    intCalls[7] = benchFunctionCallWithEightintParameters();
    intCalls[8] = benchFunctionCallWithNineintParameters();
    intCalls[9] = benchFunctionCallWithTenintParameters();

    double size32 = bench32();
    double size64 = bench64();
    double size128 = bench128();
    double size256 = bench256();
    double size512 = bench512();

    cout << endl << "Summary" << endl;

    cout << "\tOverhead of library call : " << ((libraryCall / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of virtual call : " << ((virtualCall / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of member call : " << ((memberCall / baseCall - 1) * 100) << "%" << endl;

    double longMean = 0.0;
    for (int p = 0; p < 10; ++p) {
        cout << "\tOverhead of " << (p+1) << " long parameter : " << ((longCalls[p] / baseCall - 1) * 100) << "%" << endl;
        longMean += ((longCalls[p] / baseCall - 1) / static_cast<double>(p+1));
    }

    cout << "\t=> Mean overhead for 1 long parameter : " << (longMean * 100 / 10) << "%" << endl;

    double intMean = 0.0;
    for (int p = 0; p < 10; ++p) {
        cout << "\tOverhead of " << (p+1) << " int parameter : " << ((intCalls[p] / baseCall - 1) * 100) << "%" << endl;
        intMean += ((intCalls[p] / baseCall - 1) / static_cast<double>(p+1));
    }

    cout << "\t=> Mean overhead for 1 int parameter : " << (intMean * 100 / 10) << "%" << endl;

    cout << "\tOverhead of 32 bit parameter : " << ((size32 / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of 64 bit parameter : " << ((size64 / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of 128 bit parameter : " << ((size128 / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of 256 bit parameter : " << ((size256 / baseCall - 1) * 100) << "%" << endl;
    cout << "\tOverhead of 512 bit parameter : " << ((size512 / baseCall - 1) * 100) << "%" << endl;

    return 0;
}
