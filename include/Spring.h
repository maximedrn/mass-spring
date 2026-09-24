#pragma once

#include "Node.h"
#include <QGLViewer/vec.h>

using namespace qglviewer;

class Spring {
  public:
    Spring(
        Node* origin, Node* target, double springStiffness, double springDamping
    );

    Vec computeForce() const;
    void draw() const;

    Node* const startNode;
    Node* const endNode;
    const double stiffness;
    const double damping;
    const double restLength;
};
