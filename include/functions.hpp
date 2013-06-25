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

/*! \file */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//Defined in library
long libraryTest();

class ThirtyTwo {
    public:
        long longs[4];
};

class SixtyFour {
    public:
        long longs[8];
};

class HundredTwentyEight {
    public:
        long longs[16];
};

class TwoHundredFiftySix {
    public:
        long longs[32];
};

class FiveHundredTwelve {
    public:
        long longs[64];
};

class Fat {
        long a;
        long b;
        long c;
        long d;
        long e;
};

class Obese {
        Fat a, b, c, d, e;
};

class Linker {
        Obese a, b, c, d, e;
};

class Poo {
    public:
        virtual long first();
        long second();

        virtual long first(long i);
        long second(long i);
};

class Pee : public Poo {
    public:
        long first();
        long first(long i);
};

long invoke();

long invokelong(long a);
long invokelong(long a, long b);
long invokelong(long a, long b, long c);
long invokelong(long a, long b, long c, long d);
long invokelong(long a, long b, long c, long d, long e);
long invokelong(long a, long b, long c, long d, long e, long f);
long invokelong(long a, long b, long c, long d, long e, long f, long g);
long invokelong(long a, long b, long c, long d, long e, long f, long g, long h);
long invokelong(long a, long b, long c, long d, long e, long f, long g, long h, long i);
long invokelong(long a, long b, long c, long d, long e, long f, long g, long h, long i, long j);

long invokeInt(int a);
long invokeInt(int a, int b);
long invokeInt(int a, int b, int c);
long invokeInt(int a, int b, int c, int d);
long invokeInt(int a, int b, int c, int d, int e);
long invokeInt(int a, int b, int c, int d, int e, int f);
long invokeInt(int a, int b, int c, int d, int e, int f, int g);
long invokeInt(int a, int b, int c, int d, int e, int f, int g, int h);
long invokeInt(int a, int b, int c, int d, int e, int f, int g, int h, int i);
long invokeInt(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j);

long invoke32(ThirtyTwo a);
long invoke64(SixtyFour a);
long invoke128(HundredTwentyEight o);
long invoke256(TwoHundredFiftySix o);
long invoke512(FiveHundredTwelve o);

//.1

long invokeFat(Fat a);
long invokeObese(Obese a);
long invokeLinker(Linker a);

long invokeFatPointer(Fat* a);
long invokeObesePointer(Obese* a);
long invokeLinkerPointer(Linker* a);

long invokeFatRef(Fat& a);
long invokeObeseRef(Obese& a);
long invokeLinkerRef(Linker& a);

#endif
