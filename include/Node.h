#pragma once

#include <QGLViewer/vec.h>

using namespace qglviewer;

class Node {
  public:
    Node() = default;
    explicit Node(const Vec& initialPosition);
    Node(const Vec& initialPosition, double nodeMass);

    void draw() const;

    Vec position;
    Vec velocity;
    Vec acceleration;
    Vec force;
    const double mass = 0.0;
};
