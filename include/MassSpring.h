#pragma once

#include "Node.h"
#include "Spring.h"
#include <QGLViewer/qglviewer.h>
#include <vector>

using namespace std;
using namespace qglviewer;

class MassSpring : public QGLViewer {
    Q_OBJECT

  public:
    enum class IntegrationMethod { Euler, RK4 };
    Q_ENUM(IntegrationMethod)

    static constexpr IntegrationMethod defaultIntegrationMethod =
        IntegrationMethod::Euler;

    explicit MassSpring(
        IntegrationMethod integrationMethod = defaultIntegrationMethod
    );

  protected:
    void draw() override;
    void init() override;
    void animate() override;
    void keyPressEvent(QKeyEvent* event) override;

  private:
    struct State {
        Vec position;
        Vec velocity;
    };

    static constexpr double timeStep = 0.01;

    void computeAccelerations();
    void integrateEuler(double step);
    void integrateRK4(double step);
    vector<State> evaluateDerivatives(
        const vector<State>& initialState,
        const vector<State>& derivatives,
        double timeOffset
    );

    const IntegrationMethod integrationMethod;
    vector<Node*> masses;
    vector<Spring*> springs;
};
