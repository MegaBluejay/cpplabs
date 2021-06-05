#ifndef CUBE_H
#define CUBE_H

#include <array>

#include "util.h"

class Cube {
public:

    enum class COLOR : uint8_t { WHITE, BLUE, RED, GREEN, ORANGE, YELLOW};
    using Face = std::array<COLOR, 8>;

    Cube() {
        front_.fill(COLOR::RED);
        back_.fill(COLOR::ORANGE);
        left_.fill(COLOR::GREEN);
        right_.fill(COLOR::BLUE);
        up_.fill(COLOR::WHITE);
        down_.fill(COLOR::YELLOW);
    }

    void fr() {
        turnr(front_, up_, left_, down_, right_,
              4, 2, 0, 6);
    }

    void fl() {
        turnl(front_, up_, left_, down_, right_,
              4, 2, 0, 6);
    }

    void f2() {
        turn2(front_, up_, left_, down_, right_,
              4, 2, 0, 6);
    }

    void br() {
        turnr(back_, up_, right_, down_, left_,
              0, 2, 4, 6);
    }

    void bl() {
        turnl(back_, up_, right_, down_, left_,
              0, 2, 4, 6);
    }

    void b2() {
        turn2(back_, up_, right_, down_, left_,
              0, 2, 4, 6);
    }

    void lr() {
        turnr(left_, up_, back_, down_, front_,
              6, 2, 0, 6);
    }

    void ll() {
        turnl(left_, up_, back_, down_, front_,
              6, 2, 0, 6);
    }

    void l2() {
        turn2(left_, up_, back_, down_, front_,
              6, 2, 0, 6);
    }

    void rr() {
        turnr(right_, up_, front_, down_, back_,
              2, 2, 2, 6);
    }

    void rl() {
        turnl(right_, up_, front_, down_, back_,
              2, 2, 2, 6);
    }

    void r2() {
        turn2(right_, up_, front_, down_, back_,
              2, 2, 2, 6);
    }

    void ur() {
        turnr(up_, back_, left_, front_, right_,
              0, 0, 0, 0);
    }

    void ul() {
        turnl(up_, back_, left_, front_, right_,
              0, 0, 0, 0);
    }

    void u2() {
        turn2(up_, back_, left_, front_, right_,
              0, 0, 0, 0);
    }

    void dr() {
        turnr(down_, front_, left_, back_, right_,
              4, 4, 4, 4);
    }

    void dl() {
        turnl(down_, front_, left_, back_, right_,
              4, 4, 4, 4);
    }

    void d2() {
        turn2(down_, front_, left_, back_, right_,
              4, 4, 4, 4);
    }

protected:

     Face front_{}, back_{}, left_{}, right_{}, up_{}, down_{};

    static COLOR& geti(Face& face, int i) {
        return face[i%8];
    }

    static void turnr(Face& fm, Face& f1, Face& f2, Face& f3, Face& f4
            , int i1, int i2, int i3, int i4) {
        std::rotate(fm.begin(), &fm[6], fm.end());
        for (int i = 0; i < 3; ++i) {
            COLOR &c1 = geti(f1,i1+i)
            , &c2 = geti(f2,i2+i)
            , &c3 = geti(f3, i3+i)
            , &c4 = geti(f4, i4+i);
            COLOR save = c1;
            c1 = c2;
            c2 = c3;
            c3 = c4;
            c4 = save;
        }
    }

    static void turnl(Face& fm, Face& f1, Face& f2, Face& f3, Face& f4
            , int i1, int i2, int i3, int i4) {
        std::rotate(fm.begin(), &fm[2], fm.end());
        for (int i = 0; i < 3; ++i) {
            COLOR &c1 = geti(f1,i1+i)
            , &c2 = geti(f2,i2+i)
            , &c3 = geti(f3, i3+i)
            , &c4 = geti(f4, i4+i);
            COLOR save = c1;
            c1 = c4;
            c4 = c3;
            c3 = c2;
            c2 = save;
        }
    }

    static void turn2(Face& fm, Face& f1, Face& f2, Face& f3, Face& f4
            , int i1, int i2, int i3, int i4) {
        std::rotate(fm.begin(), &fm[4], fm.end());
        for (int i = 0; i < 3; ++i) {
            COLOR &c1 = geti(f1,i1+i)
            , &c2 = geti(f2,i2+i)
            , &c3 = geti(f3, i3+i)
            , &c4 = geti(f4, i4+i);
            std::swap(c1,c3);
            std::swap(c2,c4);
        }
    }
};

#endif
