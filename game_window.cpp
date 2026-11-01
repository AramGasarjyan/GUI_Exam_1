#include "game_window.h"
#include "clock_label.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>
#include <QRandomGenerator>

GameWindow::GameWindow(QWidget* parent)
    : QWidget(parent),
      m_score(0),
      m_bestScore(0),
      m_timeLeft(30),
      m_moleIndex(-1),
      m_running(false)
{
    createWidgets();
    createLayout();
    makeConnections();

    setWindowTitle("Whack-a-Mole");
    resize(760, 440);

    updateScoreLabel(m_score);
    m_timeLeft = m_lengthSpin->value();
    updateTimeLabel();
    resetBoard();
}

void GameWindow::createWidgets()
{
    m_scoreLabel = new QLabel;
    m_timeLabel  = new QLabel;
    m_bestLabel  = new QLabel("<b>Best:</b> 0");
    m_clock      = new ClockLabel;
    m_statusLabel = new QLabel;

    m_rulesButton = new QPushButton("&Rules");
    m_startButton = new QPushButton("&Start");
    m_stopButton  = new QPushButton("S&top");
    m_quitButton  = new QPushButton("&Quit");
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(false);

    m_moleTimer = new QTimer(this);
    m_countdownTimer = new QTimer(this);
    m_countdownTimer->setInterval(1000);
}

QGroupBox* GameWindow::createPlayerGroup()
{
    QGroupBox* group = new QGroupBox("Player");

    m_nameLabel = new QLabel("&Name:");
    m_nameEdit  = new QLineEdit;
    m_nameEdit->setMaxLength(20);
    m_nameLabel->setBuddy(m_nameEdit);

    QHBoxLayout* lyt = new QHBoxLayout;
    lyt->addWidget(m_nameLabel);
    lyt->addWidget(m_nameEdit);
    group->setLayout(lyt);
    return group;
}

QGroupBox* GameWindow::createDifficultyGroup()
{
    QGroupBox* group = new QGroupBox("Difficulty");

    m_easyRadio   = new QRadioButton("&Easy");
    m_mediumRadio = new QRadioButton("&Medium");
    m_hardRadio   = new QRadioButton("&Hard");
    m_mediumRadio->setChecked(true);

    QVBoxLayout* lyt = new QVBoxLayout;
    lyt->addWidget(m_easyRadio);
    lyt->addWidget(m_mediumRadio);
    lyt->addWidget(m_hardRadio);
    group->setLayout(lyt);
    return group;
}

QGroupBox* GameWindow::createLengthGroup()
{
    QGroupBox* group = new QGroupBox("Game length");

    m_lengthSpin = new QSpinBox;
    m_lengthSpin->setRange(10, 120);
    m_lengthSpin->setSuffix(" s");
    m_lengthSpin->setValue(30);

    m_lengthSlider = new QSlider(Qt::Horizontal);
    m_lengthSlider->setRange(10, 120);
    m_lengthSlider->setValue(30);

    QHBoxLayout* lyt = new QHBoxLayout;
    lyt->addWidget(m_lengthSpin);
    lyt->addWidget(m_lengthSlider);
    group->setLayout(lyt);
    return group;
}

QGroupBox* GameWindow::createOptionsGroup()
{
    QGroupBox* group = new QGroupBox("Options");

    m_penaltyCheck = new QCheckBox("&Lose a point when a mole escapes");
    m_speedUpCheck = new QCheckBox("S&peed up after every hit");
    m_speedUpCheck->setChecked(true);

    QVBoxLayout* lyt = new QVBoxLayout;
    lyt->addWidget(m_penaltyCheck);
    lyt->addWidget(m_speedUpCheck);
    group->setLayout(lyt);
    return group;
}

QGroupBox* GameWindow::createFieldGroup()
{
    QGroupBox* group = new QGroupBox("Field");
    QGridLayout* grid = new QGridLayout;

    for (int i = 0; i < HOLES; ++i) {
        m_holes[i] = new QPushButton;
        m_holes[i]->setMinimumSize(100, 70);
        m_holes[i]->setEnabled(false);
        grid->addWidget(m_holes[i], i / COLS, i % COLS);
    }

    group->setLayout(grid);
    return group;
}

void GameWindow::createLayout()
{
    QHBoxLayout* topLyt = new QHBoxLayout;
    topLyt->addWidget(m_scoreLabel);
    topLyt->addSpacing(25);
    topLyt->addWidget(m_timeLabel);
    topLyt->addSpacing(25);
    topLyt->addWidget(m_bestLabel);
    topLyt->addStretch();
    topLyt->addWidget(m_clock);

    m_playerGroup     = createPlayerGroup();
    m_difficultyGroup = createDifficultyGroup();
    m_lengthGroup     = createLengthGroup();
    m_optionsGroup    = createOptionsGroup();

    QVBoxLayout* settingsLyt = new QVBoxLayout;
    settingsLyt->addWidget(m_playerGroup);
    settingsLyt->addWidget(m_difficultyGroup);
    settingsLyt->addWidget(m_lengthGroup);
    settingsLyt->addWidget(m_optionsGroup);
    settingsLyt->addStretch();

    QHBoxLayout* middleLyt = new QHBoxLayout;
    middleLyt->addLayout(settingsLyt);
    middleLyt->addWidget(createFieldGroup(), 1);

    QHBoxLayout* bottomLyt = new QHBoxLayout;
    bottomLyt->addWidget(m_statusLabel);
    bottomLyt->addStretch();
    bottomLyt->addWidget(m_rulesButton);
    bottomLyt->addWidget(m_startButton);
    bottomLyt->addWidget(m_stopButton);
    bottomLyt->addWidget(m_quitButton);

    QVBoxLayout* mainLyt = new QVBoxLayout;
    mainLyt->addLayout(topLyt);
    mainLyt->addLayout(middleLyt, 1);
    mainLyt->addLayout(bottomLyt);
    setLayout(mainLyt);
}

void GameWindow::makeConnections()
{
    connect(m_quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_nameEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(checkName(const QString&)));

    connect(m_lengthSpin, SIGNAL(valueChanged(int)), m_lengthSlider, SLOT(setValue(int)));
    connect(m_lengthSlider, SIGNAL(valueChanged(int)), m_lengthSpin, SLOT(setValue(int)));
    connect(m_lengthSpin, SIGNAL(valueChanged(int)), this, SLOT(onLengthChanged(int)));

    connect(m_startButton, &QPushButton::clicked, this, &GameWindow::startGame);
    connect(m_stopButton,  &QPushButton::clicked, this, &GameWindow::stopGame);
    connect(m_rulesButton, &QPushButton::clicked, this, &GameWindow::showRules);

    connect(m_moleTimer,      &QTimer::timeout, this, &GameWindow::onMoleTimeout);
    connect(m_countdownTimer, &QTimer::timeout, this, &GameWindow::onCountdownTick);

    connect(this, &GameWindow::scoreChanged, this, &GameWindow::updateScoreLabel);
    connect(this, &GameWindow::gameOver,     this, &GameWindow::showGameOverDialog);

    for (int i = 0; i < HOLES; ++i) {
        connect(m_holes[i], &QPushButton::clicked, this, [this, i]() {
            onHoleClicked(i);
        });
    }
}

void GameWindow::askPlayerName()
{
    bool ok = false;
    QString name = QInputDialog::getText(this,
                                         "Welcome to Whack-a-Mole",
                                         "Enter your name:",
                                         QLineEdit::Normal,
                                         "Player",
                                         &ok);
    if (ok && !name.trimmed().isEmpty()) {
        m_nameEdit->setText(name.trimmed());
    }
    m_nameEdit->setFocus();
}

void GameWindow::showRules()
{
    QMessageBox::information(this, "How to play",
        "1. Enter your name.\n"
        "2. Choose difficulty and game length.\n"
        "3. Press Start.\n\n"
        "A mole appears in a random hole. Click it as fast as you can!\n"
        "Every hit gives you 1 point. The mole runs away if you are too slow.\n\n"
        "Easy = slow mole, Hard = very fast mole.");
}

void GameWindow::showGameOverDialog(int finalScore)
{
    bool newRecord = finalScore > m_bestScore;
    if (newRecord) {
        m_bestScore = finalScore;
        m_bestLabel->setText(QString("<b>Best:</b> %1").arg(m_bestScore));
    }

    QString result = QString("Your score: %1\n").arg(finalScore);
    if (newRecord) {
        result += "That's a new record!\n";
    } else {
        result += QString("Best score: %1\n").arg(m_bestScore);
    }
    result += "\nDo you want to play again?";

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game over");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(QString("Time's up, %1!").arg(m_nameEdit->text().trimmed()));
    msgBox.setInformativeText(result);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        startGame();
        break;
    case QMessageBox::No:
    default:
        m_timeLeft = m_lengthSpin->value();
        updateTimeLabel();
        break;
    }
}

void GameWindow::checkName(const QString& text)
{
    m_startButton->setEnabled(!text.trimmed().isEmpty() && !m_running);
}

void GameWindow::onLengthChanged(int seconds)
{
    if (!m_running) {
        m_timeLeft = seconds;
        updateTimeLabel();
    }
}

void GameWindow::updateScoreLabel(int score)
{
    m_scoreLabel->setText(QString("<b>Score:</b> %1").arg(score));
}

void GameWindow::updateTimeLabel()
{
    m_timeLabel->setText(QString("<b>Time left:</b> %1 s").arg(m_timeLeft));
}

int GameWindow::difficultyInterval() const
{
    if (m_easyRadio->isChecked())
        return 1200;
    if (m_hardRadio->isChecked())
        return 550;
    return 850;
}

void GameWindow::startGame()
{
    m_running = true;
    m_score = 0;
    emit scoreChanged(m_score);

    m_timeLeft = m_lengthSpin->value();
    updateTimeLabel();

    setSettingsEnabled(false);
    m_statusLabel->setText("Game is running - hit the moles!");

    m_moleTimer->start(difficultyInterval());
    m_countdownTimer->start();
    moveMole();
}

void GameWindow::stopGame()
{
    if (!m_running)
        return;

    m_moleTimer->stop();
    m_countdownTimer->stop();

    int answer = QMessageBox::question(this, "Stop game",
                                       "Do you really want to stop the current game?",
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        resetBoard();
        m_timeLeft = m_lengthSpin->value();
        updateTimeLabel();
        m_statusLabel->setText("Game stopped. Press Start to play again.");
    } else {
        m_moleTimer->start();
        m_countdownTimer->start();
    }
}

void GameWindow::onHoleClicked(int index)
{
    if (!m_running || index != m_moleIndex)
        return;

    ++m_score;
    emit scoreChanged(m_score);

    if (m_speedUpCheck->isChecked()) {
        int faster = m_moleTimer->interval() * 93 / 100;
        if (faster < 300)
            faster = 300;
        m_moleTimer->start(faster);
    } else {
        m_moleTimer->start();
    }

    moveMole();
}

void GameWindow::onMoleTimeout()
{
    if (!m_running)
        return;

    if (m_penaltyCheck->isChecked() && m_score > 0) {
        --m_score;
        emit scoreChanged(m_score);
    }
    moveMole();
}

void GameWindow::onCountdownTick()
{
    --m_timeLeft;
    updateTimeLabel();

    if (m_timeLeft <= 0)
        endGame();
}

void GameWindow::moveMole()
{
    int oldIndex = m_moleIndex;
    hideMole();

    int newIndex = oldIndex;
    while (newIndex == oldIndex) {
        newIndex = QRandomGenerator::global()->bounded(static_cast<int>(HOLES));
    }

    m_moleIndex = newIndex;
    m_holes[m_moleIndex]->setText("(o_o)\nMOLE!");
    m_holes[m_moleIndex]->setEnabled(true);
}

void GameWindow::hideMole()
{
    if (m_moleIndex >= 0) {
        m_holes[m_moleIndex]->setText("");
        m_holes[m_moleIndex]->setEnabled(false);
    }
    m_moleIndex = -1;
}

void GameWindow::endGame()
{
    resetBoard();
    m_statusLabel->setText("Game over. Press Start to play again.");
    emit gameOver(m_score);
}

void GameWindow::resetBoard()
{
    m_running = false;
    m_moleTimer->stop();
    m_countdownTimer->stop();
    hideMole();
    setSettingsEnabled(true);
    m_statusLabel->setText("Press Start to play.");
}

void GameWindow::setSettingsEnabled(bool enabled)
{
    m_playerGroup->setEnabled(enabled);
    m_difficultyGroup->setEnabled(enabled);
    m_lengthGroup->setEnabled(enabled);
    m_optionsGroup->setEnabled(enabled);

    m_startButton->setEnabled(enabled && !m_nameEdit->text().trimmed().isEmpty());
    m_stopButton->setEnabled(!enabled);
}
