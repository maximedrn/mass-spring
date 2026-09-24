#include "MassSpring.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QMetaEnum>
#include <cstdlib>
#include <iostream>

int main(int argc, char** const argv) {
    QApplication application(argc, argv);

    const QMetaEnum integrationMethods =
        QMetaEnum::fromType<MassSpring::IntegrationMethod>();
    QStringList modeNames;
    for (int index = 0; index < integrationMethods.keyCount(); ++index) {
        modeNames.append(
            QString::fromLatin1(integrationMethods.key(index)).toLower()
        );
    }
    const QString defaultMode =
        QString::fromLatin1(
            integrationMethods.valueToKey(
                static_cast<int>(MassSpring::defaultIntegrationMethod)
            )
        )
            .toLower();

    QCommandLineParser parser;
    parser.setApplicationDescription("Mass spring simulation");
    parser.addHelpOption();
    const QCommandLineOption modeOption(
        "mode",
        QStringLiteral("Integration method: %1.").arg(modeNames.join(" or ")),
        "method",
        defaultMode
    );
    parser.addOption(modeOption);
    parser.process(application);

    const int modeIndex = modeNames.indexOf(parser.value(modeOption));
    if (modeIndex < 0 || !parser.positionalArguments().isEmpty()) {
        std::cerr << "Usage: massspring [--mode "
                  << modeNames.join('|').toStdString() << "]\n";
        return EXIT_FAILURE;
    }

    const MassSpring::IntegrationMethod integrationMethod =
        static_cast<MassSpring::IntegrationMethod>(
            integrationMethods.value(modeIndex)
        );
    MassSpring viewer(integrationMethod);

    viewer.setWindowTitle(QStringLiteral("Mass Spring Simulation (%1)")
                              .arg(modeNames.at(modeIndex).toUpper()));
    viewer.resize(900, 700);

    viewer.show();

    return application.exec();
}
