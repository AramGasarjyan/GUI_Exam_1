#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QSpinBox;
class QSlider;
class QGroupBox;
class QTimer;
class ClockLabel;

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);

public slots:
    void askPlayerName();

signals:
    void scoreChanged(int score);
    void gameOver(int finalScore);

private slots:
    void checkName(const QString& text);
    void onLengthChanged(int seconds);
    void startGame();
    void stopGame();
    void showRules();
    void onHoleClicked(int index);
    void onMoleTimeout();
    void onCountdownTick();
    void updateScoreLabel(int score);
    void showGameOverDialog(int finalScore);

private:
    void createWidgets();
    void createLayout();
    void makeConnections();
    QGroupBox* createPlayerGroup();
    QGroupBox* createDifficultyGroup();
    QGroupBox* createLengthGroup();
    QGroupBox* createOptionsGroup();
    QGroupBox* createFieldGroup();

    void moveMole();
    void hideMole();
    void resetBoard();
    void endGame();
    void setSettingsEnabled(bool enabled);
    void updateTimeLabel();
    int  difficultyInterval() const;

private:
    enum { ROWS = 3, COLS = 3, HOLES = ROWS * COLS };

    QLabel*     m_scoreLabel;
    QLabel*     m_timeLabel;
    QLabel*     m_bestLabel;
    ClockLabel* m_clock;

    QGroupBox*    m_playerGroup;
    QLabel*       m_nameLabel;
    QLineEdit*    m_nameEdit;

    QGroupBox*    m_difficultyGroup;
    QRadioButton* m_easyRadio;
    QRadioButton* m_mediumRadio;
    QRadioButton* m_hardRadio;

    QGroupBox*    m_lengthGroup;
    QSpinBox*     m_lengthSpin;
    QSlider*      m_lengthSlider;

    QGroupBox*    m_optionsGroup;
    QCheckBox*    m_penaltyCheck;
    QCheckBox*    m_speedUpCheck;

    QPushButton* m_holes[HOLES];

    QLabel*      m_statusLabel;
    QPushButton* m_rulesButton;
    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QPushButton* m_quitButton;

    QTimer* m_moleTimer;
    QTimer* m_countdownTimer;

    int  m_score;
    int  m_bestScore;
    int  m_timeLeft;
    int  m_moleIndex;
    bool m_running;
};

#endif // GAME_WINDOW_H
