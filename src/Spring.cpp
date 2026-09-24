#include "Spring.h"

using namespace qglviewer;

Spring::Spring(
    Node* const origin,
    Node* const target,
    const double springStiffness,
    const double springDamping
)
    : startNode(origin), endNode(target), stiffness(springStiffness),
      damping(springDamping),
      restLength((target->position - origin->position).norm()) {}

Vec Spring::computeForce() const {
    const Vec displacement =
        this->endNode->position - this->startNode->position;
    const double length = displacement.norm();
    if (length == 0.0) {
        return {};
    }

    const Vec direction = displacement / length;
    const double relativeSpeed =
        (this->endNode->velocity - this->startNode->velocity) * direction;
    return (this->stiffness * (length - this->restLength) +
            this->damping * relativeSpeed) *
        direction;
}

void Spring::draw() const {
    glColor3f(1, 1, 0);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex3fv(this->startNode->position);
    glVertex3fv(this->endNode->position);
    glEnd();
}
