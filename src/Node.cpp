#include "Node.h"

using namespace qglviewer;

Node::Node(const Vec& initialPosition) : Node(initialPosition, 0.0) {}

Node::Node(const Vec& initialPosition, const double nodeMass)
    : position(initialPosition), mass(nodeMass) {}

void Node::draw() const {
    if (this->mass == 0) {
        glColor3f(1, 0, 0);
    } else {
        glColor3f(1, 1, 1);
    }
    glPointSize(24);
    glBegin(GL_POINTS);
    glVertex3fv(this->position);
    glEnd();
}
