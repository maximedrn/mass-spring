#include "MassSpring.h"
#include <QKeyEvent>

using namespace qglviewer;
using namespace std;

MassSpring::MassSpring(const IntegrationMethod integrationMethod)
    : integrationMethod(integrationMethod) {}

void MassSpring::init() {
    glDisable(GL_LIGHTING);
    this->setGridIsDrawn();

    this->masses.clear();
    this->springs.clear();

    // A zero mass marks the fixed anchor; the other node is free to move.
    Node* const fixedNode = new Node(Vec(2, 0, 4));
    Node* const movingNode = new Node(Vec(2, 0, 2), 5);
    this->masses.push_back(fixedNode);
    this->masses.push_back(movingNode);
    Spring* const spring = new Spring(fixedNode, movingNode, 5, 0.5);
    this->springs.push_back(spring);

    const Vec center = (fixedNode->position + movingNode->position) / 2.0;
    this->setSceneCenter(center);
    this->setSceneRadius(4.0);
    this->camera()->setViewDirection(Vec(0, 1, 0));
    this->camera()->setUpVector(Vec(0, 0, 1));
    this->showEntireScene();

    // Keep the default camera controls and use Space for the simulation.
    this->setShortcut(CAMERA_MODE, 0);
    this->setShortcut(ANIMATION, 0);
    this->setShortcut(EXIT_VIEWER, Qt::Key_Escape);
    this->setKeyDescription(Qt::Key_Space, "Start animation");
    this->setAnimationPeriod(10);
}

void MassSpring::draw() {
    for (const Node* const node : this->masses) {
        node->draw();
    }

    for (const Spring* const spring : this->springs) {
        spring->draw();
    }
}

void MassSpring::animate() {
    if (this->integrationMethod == IntegrationMethod::RK4) {
        this->integrateRK4(MassSpring::timeStep);
    } else {
        this->integrateEuler(MassSpring::timeStep);
    }

    // Use the updated state to check whether the system has settled.
    this->computeAccelerations();
    constexpr double speedTolerance = 1e-3;
    constexpr double accelerationTolerance = 1e-3;

    /*
     * Stop only when every moving node is close to rest:
     *     ||v|| <= speedTolerance and ||a|| <= accelerationTolerance
     * Checking acceleration avoids stopping at a turning point,
     * where velocity is zero but the spring still pulls the mass back.
     */
    for (const Node* const node : this->masses) {
        if (node->mass > 0.0 &&
            (node->velocity.norm() > speedTolerance ||
             node->acceleration.norm() > accelerationTolerance)) {
            return;
        }
    }

    // Remove the remaining motion before stopping the animation.
    for (Node* const node : this->masses) {
        node->velocity = Vec();
    }
    this->stopAnimation();
}

void MassSpring::computeAccelerations() {
    // Rebuild the total force from scratch at each evaluation.
    for (Node* const node : this->masses) {
        node->force = Vec();
    }

    /*
     * Add the elastic and damping forces from each spring.
     * L is its length, L0 its rest length, k its stiffness and c its damping.
     *     u = (x_end - x_start) / L
     *     F_start = [k * (L - L0) + c * ((v_end - v_start) dot u)] * u
     *     F_end = -F_start
     * The damping force opposes relative motion along the spring.
     */
    for (const Spring* const spring : this->springs) {
        const Vec force = spring->computeForce();
        spring->startNode->force += force;
        spring->endNode->force -= force;
    }

    /*
     * Convert the total force to acceleration using Newton's second law:
     *     a = sum(F) / m
     * Fixed nodes use a = 0 and are skipped by both integrators.
     */
    for (Node* const node : this->masses) {
        node->acceleration =
            node->mass > 0.0 ? node->force / node->mass : Vec();
    }
}

void MassSpring::integrateEuler(const double step) {
    /*
     * Semi-implicit Euler, with h = step:
     *     v_(n+1) = v_n + h * a_n
     *     x_(n+1) = x_n + h * v_(n+1)
     * Update velocity first, then use it to advance the position.
     */
    this->computeAccelerations();
    for (Node* const node : this->masses) {
        if (node->mass <= 0.0) {
            continue;
        }
        node->velocity += node->acceleration * step;
        node->position += node->velocity * step;
    }
}

void MassSpring::integrateRK4(const double step) {
    // Save one starting state for all four stages and the final update.
    vector<State> initialState;
    initialState.reserve(this->masses.size());
    for (const Node* const node : this->masses) {
        initialState.push_back({node->position, node->velocity});
    }

    /*
     * RK4 samples four derivatives of y = (x, v), with f(y) = (v, a).
     * For h = step:
     *     k1 = f(y_n)
     *     k2 = f(y_n + h * k1 / 2)
     *     k3 = f(y_n + h * k2 / 2)
     *     k4 = f(y_n + h * k3)
     *     y_(n+1) = y_n + h * (k1 + 2*k2 + 2*k3 + k4) / 6
     * Each k contains a velocity and an acceleration, without the factor h.
     */
    const vector<State> zeroDerivatives(this->masses.size());
    const vector<State> first =
        this->evaluateDerivatives(initialState, zeroDerivatives, 0.0);
    const vector<State> second =
        this->evaluateDerivatives(initialState, first, step / 2.0);
    const vector<State> third =
        this->evaluateDerivatives(initialState, second, step / 2.0);
    const vector<State> fourth =
        this->evaluateDerivatives(initialState, third, step);

    // Apply the weighted average to both position and velocity.
    for (size_t index = 0; index < this->masses.size(); ++index) {
        Node* const node = this->masses[index];
        if (node->mass <= 0.0) {
            continue;
        }
        node->position = initialState[index].position +
            step / 6.0 *
                (first[index].position + 2.0 * second[index].position +
                 2.0 * third[index].position + fourth[index].position);
        node->velocity = initialState[index].velocity +
            step / 6.0 *
                (first[index].velocity + 2.0 * second[index].velocity +
                 2.0 * third[index].velocity + fourth[index].velocity);
    }
}

vector<MassSpring::State> MassSpring::evaluateDerivatives(
    const vector<State>& initialState,
    const vector<State>& derivatives,
    const double timeOffset
) {
    /*
     * Evaluate the derivative at a trial state, with tau = timeOffset:
     *     y_trial = y_n + tau * k
     *     f(y_trial) = (v_trial, a(x_trial, v_trial))
     * In the returned State, .position stores dx/dt and .velocity stores dv/dt.
     */
    for (size_t index = 0; index < this->masses.size(); ++index) {
        Node* const node = this->masses[index];
        if (node->mass <= 0.0) {
            continue;
        }
        node->position = initialState[index].position +
            derivatives[index].position * timeOffset;
        node->velocity = initialState[index].velocity +
            derivatives[index].velocity * timeOffset;
    }

    // Update all nodes first so spring forces use a consistent trial state.
    this->computeAccelerations();
    vector<State> result(this->masses.size());
    for (size_t index = 0; index < this->masses.size(); ++index) {
        const Node* const node = this->masses[index];
        if (node->mass > 0.0) {
            result[index] = {
                .position = node->velocity, .velocity = node->acceleration
            };
        }
    }
    return result;
}

void MassSpring::keyPressEvent(QKeyEvent* const event) {
    if (event->key() == Qt::Key_Space && event->modifiers() == Qt::NoModifier) {
        if (!event->isAutoRepeat() && !this->animationIsStarted()) {
            // Give the mass a downward push without changing its position.
            this->masses[1]->velocity += Vec(0, 0, -0.5);
            this->startAnimation();
            this->update();
        }
        event->accept();
        return;
    }

    QGLViewer::keyPressEvent(event);
}
