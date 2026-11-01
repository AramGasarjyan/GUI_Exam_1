# Whack-a-Mole - Complete Technical README

A small but complete desktop game written in C++ with **Qt Widgets**. A mole pops up in a random hole of a 3x3 field, you click it, the score goes up, and the mole speeds up. Everything you see (window, buttons, dialogs, clock, timers) is built from standard Qt classes from your GUI course. There is no drawing code, no images and no external library.

This README explains **what every file, function, variable and Qt call does and why it is there**. Read sections 1-5 to understand the whole project, then use sections 6-11 as a reference while you look at the code.

> **Line numbers** (for example `game_window.cpp:309`) refer to the files as delivered. If you edit a file they will shift, but the function names stay valid.

---

## Contents

1. [What the project does](#1-what-the-project-does)
2. [Run it](#2-run-it)
3. [Project map](#3-project-map)
4. [The window, region by region](#4-the-window-region-by-region)
5. [The life of the program (timeline)](#5-the-life-of-the-program-timeline)
6. [Core Qt ideas used and why](#6-core-qt-ideas-used-and-why)
7. [File reference](#7-file-reference)
8. [Complete signal and slot map](#8-complete-signal-and-slot-map)
9. [State: variables, modes and what is enabled when](#9-state-variables-modes-and-what-is-enabled-when)
10. [Timers in depth](#10-timers-in-depth)
11. [API index: every Qt class and function used](#11-api-index-every-qt-class-and-function-used)
12. [Which lesson each part comes from](#12-which-lesson-each-part-comes-from)
13. [Design decisions (the "why")](#13-design-decisions-the-why)
14. [Known behaviours and limits](#14-known-behaviours-and-limits)
15. [Cookbook: how to change the game](#15-cookbook-how-to-change-the-game)
16. [Build system](#16-build-system)
17. [Troubleshooting](#17-troubleshooting)
18. [Questions your teacher may ask](#18-questions-your-teacher-may-ask)

---

## 1. What the project does

| Step | What the player sees | What the program does |
|---|---|---|
| 1 | A name dialog right after the window opens | `askPlayerName()` runs once, using a single-shot timer |
| 2 | Settings on the left: name, difficulty, game length, two options | Widgets built by the `create...Group()` functions |
| 3 | Press **Start** | Timers start, the first mole appears, the settings lock |
| 4 | Click the mole | +1 point, the mole jumps to another hole, optionally faster |
| 5 | Too slow | The mole moves anyway (optionally -1 point) |
| 6 | Time reaches 0 | Result window: score, best score, "Play again?" |

The whole project is **595 lines over 5 source files**; 440 of them are in `game_window.cpp`.

---

## 2. Run it

**Windows, easiest: double-click `WhackAMole_Launcher.bat`.** It shows a menu: 1 play the game (builds it first), 2 open the code in Qt Creator to read and analyse it, 3 open the PDF guide, 4 open this README, 5 install Qt and Qt Creator if they are missing (it downloads the official installer and starts it; you click through it and sign in with a free Qt account).

**Windows (Qt Creator, recommended):** unzip, then File > Open File or Project... > `WhackAMole.pro` > pick the *Desktop Qt ... MinGW 64-bit* kit > Configure Project > green Run button.
Do not open the `.zip` itself and do not just double-click the `.pro` file; use the menu so it opens as a **project**.

**Windows (double-click):** `run_windows.bat` finds Qt, builds in a `build` folder and starts the game. Add `--standalone` (from a command prompt) to get a folder that runs without Qt.

**Mac:** `./run_mac.command` (or double-click). Add `--standalone` for a `.app` that runs without Qt.

If Qt Creator complains about "in-source build files", delete `Makefile`, `.qmake.stash`, `*.o`, `moc_*` and `WhackAMole.app` from the folder (they are leftovers from a Mac build) and open the project again.

---

## 3. Project map

```
WhackAMole/
|-- WhackAMole.pro     build description for qmake (Qt Creator uses this)
|-- CMakeLists.txt     the same build description for CMake
|-- main.cpp           entry point: creates the app and the window
|-- game_window.h      class GameWindow: declaration (functions, signals, variables)
|-- game_window.cpp    class GameWindow: ALL the interface building and game logic
|-- clock_label.h      class ClockLabel: declaration
|-- clock_label.cpp    class ClockLabel: a label that shows the current time
|-- WhackAMole_Launcher.bat   menu for Windows: play, open in Qt Creator, guides, install Qt
|-- run_windows.bat    one-click build and run (Windows)
|-- WhackAMole_Guide.pdf   the plain-language guide with diagrams
|-- run_mac.command    one-click build and run (macOS)
`-- README.md          this file
```

| File | Lines | Role |
|---|---|---|
| `main.cpp` | 15 | Starts Qt, shows the window, schedules the name dialog, starts the event loop |
| `game_window.h` | 104 | Lists everything `GameWindow` can do and remember |
| `game_window.cpp` | 440 | Builds the window and implements the game |
| `clock_label.h` | 16 | Declares `ClockLabel` |
| `clock_label.cpp` | 20 | Implements the live clock |

### How the pieces are connected

```
main()
 |
 |-- QApplication app            owns the event loop (app.exec())
 |
 `-- GameWindow window           the main window (inherits QWidget)
       |
       |-- ClockLabel            inherits QLabel; owns its own 1-second QTimer
       |-- m_moleTimer           QTimer: "the mole was too slow"
       |-- m_countdownTimer      QTimer: "one more second has passed"
       |-- labels, buttons, spin box, slider, radio buttons, check boxes
       `-- 9 hole buttons        m_holes[0..8]
```

Only **one class does real work**: `GameWindow`. `ClockLabel` is a small decoration that reuses your Lesson 7 homework.

---

## 4. The window, region by region

```
+------------------------------------------------------------------+
| A  Score: 0    Time left: 30 s    Best: 0              07:45 PM  |  topLyt
+----------------------------+-------------------------------------+
| B  [Player]                | C  [Field]                          |
|     Name: [_________]      |      [hole0] [hole1] [hole2]        |
|    [Difficulty]            |      [hole3] [hole4] [hole5]        |
|     ( ) Easy               |      [hole6] [hole7] [hole8]        |
|     (o) Medium             |                                     |
|     ( ) Hard               |   QGridLayout, 3 rows x 3 columns   |
|    [Game length]           |                                     |
|     [30 s] ---o-------     |                                     |
|    [Options]               |                                     |
|     [ ] Lose a point ...   |                                     |
|     [x] Speed up ...       |                                     |
+----------------------------+-------------------------------------+
| D  status text            [Rules] [Start] [Stop] [Quit]          |  bottomLyt
+------------------------------------------------------------------+
```

| Region | Layout | Widgets inside |
|---|---|---|
| **A** top bar | `topLyt` (`QHBoxLayout`) | `m_scoreLabel`, `m_timeLabel`, `m_bestLabel`, stretch, `m_clock` |
| **B** settings | `settingsLyt` (`QVBoxLayout`) | four `QGroupBox`es: player, difficulty, length, options |
| **C** field | `QGridLayout` inside the "Field" group box | `m_holes[0]` ... `m_holes[8]` |
| **D** bottom bar | `bottomLyt` (`QHBoxLayout`) | `m_statusLabel`, stretch, Rules, Start, Stop, Quit buttons |

How they nest: `mainLyt` (vertical) holds A, then `middleLyt` (horizontal: B on the left, C on the right), then D. A layout may contain widgets **and other layouts**; that is the only trick behind the whole window.

### Field index map

The loop counter `i` (0 to 8) becomes a grid position with `row = i / 3` and `column = i % 3`:

| | column 0 | column 1 | column 2 |
|---|---|---|---|
| **row 0** | hole 0 | hole 1 | hole 2 |
| **row 1** | hole 3 | hole 4 | hole 5 |
| **row 2** | hole 6 | hole 7 | hole 8 |

`m_moleIndex` stores which of these nine holes has the mole (or -1 for none).

### Keyboard shortcuts (the `&` in button texts)

| Text in code | Shortcut | Does |
|---|---|---|
| `"&Rules"` | Alt+R | Rules |
| `"&Start"` | Alt+S | Start |
| `"S&top"` | Alt+T | Stop |
| `"&Quit"` | Alt+Q | Quit |
| `"&Name:"` (label with buddy) | Alt+N | Jumps into the name box |
| `"&Easy"` `"&Medium"` `"&Hard"` | Alt+E / M / H | Selects difficulty |
| `"&Lose a point ..."` | Alt+L | Toggles the penalty |
| `"S&peed up ..."` | Alt+P | Toggles speed-up |

---

## 5. The life of the program (timeline)

A GUI program does not run top to bottom. It sets everything up, then **waits for events** and reacts to each one. Here is one complete game in order, with the exact function that handles each moment.

| # | Moment | What happens | Function |
|---|---|---|---|
| 1 | Program starts | `QApplication` is created, then `GameWindow` | `main()`, `GameWindow::GameWindow` |
| 2 | Window is built | Widgets created, arranged in layouts, signals connected | `createWidgets`, `createLayout`, `makeConnections` |
| 3 | Window is shown | `show()`, then the event loop starts with `app.exec()` | `main()` |
| 4 | Event loop begins | The single-shot timer (0 ms) fires once | `QTimer::singleShot` in `main()` |
| 5 | Name dialog | Modal text dialog; OK puts the name in the Name box | `askPlayerName()` |
| 6 | Name box changes | `textChanged` signal; Start is enabled when the name is not empty | `checkName()` |
| 7 | Player changes length | Spin box and slider follow each other; "Time left" updates | `onLengthChanged()` and Qt's `setValue` |
| 8 | Player presses Start | Score 0, time set, settings locked, both timers start, first mole appears | `startGame()` |
| 9 | Every 1000 ms | One second less; at 0 the game ends | `onCountdownTick()` |
| 10 | Player clicks the mole | +1 point, timer restarted (maybe faster), new mole | `onHoleClicked()` |
| 11 | Mole timer fires | The player was too slow; (optional -1 point); new mole | `onMoleTimeout()` |
| 12 | Time hits 0 | Board reset, `gameOver` signal sent | `endGame()` |
| 13 | Result window | Best score updated; "Play again?" Yes starts a new game, No returns to idle | `showGameOverDialog()` |
| 14 | Quit | `close()` closes the only window, so `app.exec()` returns and the program ends | Quit button, `main()` |

Steps 9, 10 and 11 interleave freely while the game runs: the countdown ticks once a second, and clicks and mole timeouts happen whenever they happen. Qt handles each event one at a time, so they never run at the same instant and no locking is needed.

---

## 6. Core Qt ideas used and why

| Idea | In plain words | Where | Why it is used here |
|---|---|---|---|
| **Widget** | Any visible element; a window is also a widget | everywhere; `GameWindow : public QWidget` | The whole UI is widgets inside one widget |
| **Parent and child** | A child is shown inside its parent and deleted with it | layouts reparent widgets in `setLayout`; timers get `this` | No manual `delete` needed |
| **Layout** | Places widgets and resizes them with the window | `QHBoxLayout`, `QVBoxLayout`, `QGridLayout` | The window stays tidy at any size |
| **Group box** | Framed container with a title | four settings blocks, the field | Groups radio buttons and lets one call disable a whole block |
| **Signal and slot** | "When X happens, call Y" | all of `makeConnections()` | Connects clicks, timers and value changes to your functions |
| **Own signals** | Signals you declare yourself | `scoreChanged`, `gameOver` | The sender does not need to know who listens |
| **Event loop** | The waiting loop started by `exec()` | `app.exec()` in `main()` | Nothing runs unless an event happens |
| **Meta-object system / moc** | Qt generates code for `Q_OBJECT` classes | `Q_OBJECT` in both headers | Makes `signals`, `slots` and `connect` work |
| **Modal dialog** | Window that blocks the rest until closed | `QInputDialog`, `QMessageBox` | Ask or tell the player something |
| **Single-shot timer** | Fires once | `QTimer::singleShot(0, ...)` | Opens the name dialog after the window is visible |
| **Repeating timer** | Fires again and again | `m_moleTimer`, `m_countdownTimer`, the clock's timer | All the timing in the game |
| **Lambda** | A tiny unnamed function written in place | the 9 hole connections | Carries each hole's own number into the slot |

---

## 7. File reference

### 7.1 `main.cpp` (15 lines)

```cpp
 1  #include <QApplication>
 2  #include <QTimer>
 3  #include "game_window.h"
 4
 5  int main(int argc, char* argv[])
 6  {
 7      QApplication app(argc, argv);
 8
 9      GameWindow* window = new GameWindow;
10      window->show();
11
12      QTimer::singleShot(0, window, &GameWindow::askPlayerName);
13
14      return app.exec();
15  }
```

| Line | Meaning |
|---|---|
| 7 | Creates the Qt application object. One is required before any widget exists. It reads the command-line arguments and owns the event loop. |
| 9 | Creates the main window. Its constructor builds everything (section 7.4). |
| 10 | Windows are invisible until `show()` is called. |
| 12 | Schedules `askPlayerName()` to run once, **as soon as the event loop starts**. If we called it directly before `exec()`, the name dialog would appear without the main window behind it. |
| 14 | Starts the event loop. It returns only when the last window closes. Its return value becomes the program's exit code. |

### 7.2 `clock_label.h` and `clock_label.cpp`

`ClockLabel` **is a** `QLabel` (it inherits from it) that updates its own text every second.

```cpp
ClockLabel::ClockLabel(QWidget* p) : QLabel(p)
{
    QTimer* tmr = new QTimer(this);        // parent = this: deleted together with the label
    tmr->setInterval(1000);                // 1000 ms = 1 second
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime()));
    tmr->start();
    updateTime();                          // show the time immediately, not after 1 s
}

void ClockLabel::updateTime()
{
    QTime t = QTime::currentTime();        // current system time
    setText(t.toString("hh:mm:ss AP"));    // for example 07:45:35 PM
}
```

| Piece | Purpose |
|---|---|
| `Q_OBJECT` and `private slots:` in the header | Needed so `updateTime()` can be a slot |
| `new QTimer(this)` | Gives the timer a parent so Qt deletes it automatically |
| `setInterval(1000)` + `start()` | Repeating timer, one `timeout()` per second |
| `QTime::currentTime()` | A `QTime` holding the current clock time |
| `toString("hh:mm:ss AP")` | `hh` hours with leading zero, `mm` minutes, `ss` seconds, `AP` adds AM/PM |
| `updateTime()` in the constructor | Without it the label would be empty for the first second |

### 7.3 `game_window.h` (104 lines): the class declaration

**Forward declarations (lines 6-15)** such as `class QLabel;` tell the compiler "this class exists" without including its whole header. The pointers in the class only need the name, so compile times stay short. The real `#include <QLabel>` lines are in the `.cpp` file.

**Access sections:**

| Section | Contents | Meaning |
|---|---|---|
| `public:` | the constructor | Used by `main()` |
| `public slots:` | `askPlayerName()` | Must be reachable from `main()` (it is passed to `singleShot`) |
| `signals:` | `scoreChanged(int)`, `gameOver(int)` | Our own messages; Qt writes their bodies via moc |
| `private slots:` | 10 functions | Reachable by `connect` inside the class, hidden from outside |
| `private:` | helpers, data | Internal only |

**The enum (line 62):** `enum { ROWS = 3, COLS = 3, HOLES = ROWS * COLS };` gives named constants. Change `ROWS` and `COLS` and the whole field resizes (see the cookbook).

**All data members:**

| Variable | Type | Role | Written by | Read by |
|---|---|---|---|---|
| `m_scoreLabel` | `QLabel*` | Shows "Score: n" | `updateScoreLabel` | layout |
| `m_timeLabel` | `QLabel*` | Shows "Time left: n s" | `updateTimeLabel` | layout |
| `m_bestLabel` | `QLabel*` | Shows "Best: n" | `showGameOverDialog` | layout |
| `m_clock` | `ClockLabel*` | Live clock, top right | its own timer | layout |
| `m_statusLabel` | `QLabel*` | Status sentence at the bottom | `startGame`, `stopGame`, `endGame`, `resetBoard` | - |
| `m_playerGroup` | `QGroupBox*` | Group holding the name box | `createPlayerGroup` | `setSettingsEnabled` |
| `m_nameLabel` | `QLabel*` | "Name:" label with buddy | `createPlayerGroup` | - |
| `m_nameEdit` | `QLineEdit*` | The name text box | user, `askPlayerName` | `checkName`, `setSettingsEnabled`, `showGameOverDialog` |
| `m_difficultyGroup` | `QGroupBox*` | Holds the 3 radio buttons | `createDifficultyGroup` | `setSettingsEnabled` |
| `m_easyRadio`, `m_mediumRadio`, `m_hardRadio` | `QRadioButton*` | Difficulty choice | user | `difficultyInterval` |
| `m_lengthGroup` | `QGroupBox*` | Holds spin box and slider | `createLengthGroup` | `setSettingsEnabled` |
| `m_lengthSpin` | `QSpinBox*` | Game length in seconds (10-120) | user, slider | `startGame`, `stopGame`, `showGameOverDialog`, constructor |
| `m_lengthSlider` | `QSlider*` | Same value as a slider | user, spin box | - |
| `m_optionsGroup` | `QGroupBox*` | Holds the two check boxes | `createOptionsGroup` | `setSettingsEnabled` |
| `m_penaltyCheck` | `QCheckBox*` | "Lose a point when a mole escapes" | user | `onMoleTimeout` |
| `m_speedUpCheck` | `QCheckBox*` | "Speed up after every hit" | user | `onHoleClicked` |
| `m_holes[9]` | `QPushButton*[9]` | The 9 buttons of the field | `createFieldGroup` | `moveMole`, `hideMole` |
| `m_rulesButton`, `m_startButton`, `m_stopButton`, `m_quitButton` | `QPushButton*` | Bottom buttons | `createWidgets` | `setSettingsEnabled`, `checkName` |
| `m_moleTimer` | `QTimer*` | Fires when the mole was too slow | `startGame`, `onHoleClicked`, `stopGame` | - |
| `m_countdownTimer` | `QTimer*` | Fires every 1000 ms | `startGame`, `stopGame`, `resetBoard` | - |
| `m_score` | `int` | Points in the current game | `startGame`, `onHoleClicked`, `onMoleTimeout` | `endGame`, `onMoleTimeout` |
| `m_bestScore` | `int` | Best score since the program started | `showGameOverDialog` | `showGameOverDialog` |
| `m_timeLeft` | `int` | Seconds remaining | `startGame`, `onCountdownTick`, `onLengthChanged`, dialogs | `updateTimeLabel`, `onCountdownTick` |
| `m_moleIndex` | `int` | Hole with the mole, -1 = none | `moveMole`, `hideMole` | `onHoleClicked` |
| `m_running` | `bool` | Is a game in progress? | `startGame`, `resetBoard` | `stopGame`, `onHoleClicked`, `onMoleTimeout`, `onLengthChanged`, `checkName` |

The `m_` prefix is only a naming habit meaning "member variable".

### 7.4 `game_window.cpp` (440 lines): every function

#### A. Construction

**`GameWindow::GameWindow(QWidget* parent)`** (line 20)
- *Called by:* `main()`.
- *Does:* initialises the state variables (score 0, best 0, time 30, no mole, not running) in the **initializer list**, then builds the window in a strict order: `createWidgets()`, `createLayout()`, `makeConnections()`. Sets the title and the size (760 x 440). Finally shows the starting values (`updateScoreLabel`, `updateTimeLabel` with the spin box value) and calls `resetBoard()` so the window starts in the idle state.
- *Why the order matters:* `makeConnections()` needs every widget to exist. The name box, radio buttons, spin box and holes are created inside `createLayout()` (through the `create...Group()` helpers), so connections must come after it.

**`createWidgets()`** (line 41): creates the widgets that are **not** inside a group box: the score, time, best, clock and status labels, the four bottom buttons, and the two game timers. Start and Stop begin disabled. `m_countdownTimer` gets its 1000 ms interval here; `m_moleTimer`'s interval is set later in `startGame()` because it depends on the difficulty. Both timers get `this` as parent.

**The five group builders**, each returns a finished `QGroupBox*`:

| Function (line) | Builds | Key calls |
|---|---|---|
| `createPlayerGroup()` (61) | "Name:" label + text box in a horizontal layout | `setMaxLength(20)`, `setBuddy` so Alt+N focuses the box |
| `createDifficultyGroup()` (77) | Three radio buttons in a vertical layout | `setChecked(true)` on Medium; radios in one parent are mutually exclusive automatically |
| `createLengthGroup()` (94) | Spin box and slider side by side | both `setRange(10, 120)` and `setValue(30)`; spin box `setSuffix(" s")` |
| `createOptionsGroup()` (114) | Two check boxes in a vertical layout | speed-up starts checked, penalty unchecked; check boxes are independent |
| `createFieldGroup()` (129) | Nine buttons in a `QGridLayout` | loop `i = 0..8`, `addWidget(btn, i / COLS, i % COLS)`, `setMinimumSize(100, 70)`, `setEnabled(false)` |

A hole is just a `QPushButton`. **Empty hole = disabled button with no text. Mole = enabled button with text.** No painting is involved.

**`createLayout()`** (line 145): builds the four bars/columns and nests them:
`topLyt` (labels, spacing 25, stretch, clock), `settingsLyt` (the four groups, stretch at the bottom so they stay at the top), `middleLyt` (settings left, field right with stretch factor 1 so the field takes the extra space), `bottomLyt` (status, stretch, four buttons), and finally `mainLyt` stacks top, middle (stretch factor 1), bottom and is installed with `setLayout(mainLyt)`.
`addStretch()` is an invisible springy gap; `addSpacing(25)` is a fixed gap.

**`makeConnections()`** (line 187): the list of every "when X, call Y" rule. Full table in section 8. It uses all three syntaxes from Lesson 2: the old `SIGNAL()/SLOT()` macros (lines 189-195), function pointers (197-205) and a lambda inside a loop (207-211).

#### B. Dialogs

**`askPlayerName()`** (line 214), public slot.
- *Called by:* the single-shot timer from `main()`.
- *Does:* `QInputDialog::getText(...)` shows a modal text dialog with the default text "Player" and returns the entered `QString`; `ok` becomes `true` if OK was pressed. If OK **and** the trimmed text is not empty, `m_nameEdit->setText(...)`. That triggers `textChanged`, so `checkName()` runs and enables Start. If the player cancels, the name stays empty and Start stays disabled. In both cases focus goes to the name box.

**`showRules()`** (line 229): `QMessageBox::information(...)` is a static convenience function: one call shows a window with an information icon, the text, and an OK button, and waits until it is closed.

**`showGameOverDialog(int finalScore)`** (line 240), private slot.
- *Called by:* the `gameOver` signal.
- *Does:* decides whether this is a new record (`finalScore > m_bestScore`) and, if so, stores it and updates the Best label. Builds the message text with `QString::arg`. Then uses the **property-based** `QMessageBox` API: create the box, set title, icon, text, informative text, buttons (`Yes | No`) and default button, then `exec()`.
- `exec()` blocks inside the function until a button is pressed and returns **which** button; a `switch` reacts: **Yes** calls `startGame()` (new game, same settings), **No** just resets "Time left" to the chosen length.

#### C. Small slots and helpers

| Function (line) | Called by | What it does |
|---|---|---|
| `checkName(const QString& text)` (277) | `m_nameEdit` `textChanged` | Start is enabled only if the trimmed text is not empty **and** no game is running |
| `onLengthChanged(int seconds)` (282) | `m_lengthSpin` `valueChanged` | When idle, copies the chosen length into `m_timeLeft` and refreshes "Time left" |
| `updateScoreLabel(int score)` (290) | `scoreChanged` signal; constructor | `setText("<b>Score:</b> n")`; the `<b>` tag works because `QLabel` understands simple HTML |
| `updateTimeLabel()` (295) | many | Writes `m_timeLeft` into the Time label |
| `difficultyInterval() const` (300) | `startGame` | Returns 1200 (Easy), 550 (Hard) or 850 (Medium) milliseconds by asking the radio buttons `isChecked()`. It is `const` because it only reads. |

#### D. Game logic

**`startGame()`** (line 309)
- *Called by:* Start button; "Yes" in the game over dialog.
- *Does, in order:* `m_running = true`; score to 0 and `emit scoreChanged(0)`; `m_timeLeft` from the spin box; `setSettingsEnabled(false)` (locks the settings, enables Stop); status text; `m_moleTimer->start(difficultyInterval())`; `m_countdownTimer->start()`; `moveMole()` so the first mole appears immediately.

**`stopGame()`** (line 326)
- *Called by:* Stop button.
- *Does:* if no game is running, returns. Otherwise **first stops both timers**, then asks `QMessageBox::question(... Yes | No, default No)`. **Yes:** `resetBoard()`, restore the time label, status "Game stopped...". **No:** start both timers again and the game continues.
- *Why stop the timers first:* a modal dialog blocks mouse and keyboard input to the window, but **timers keep firing in the background**. Without stopping them the clock would count down while the player reads the question.

**`onHoleClicked(int index)`** (line 349)
- *Called by:* the lambda of each hole button, with that hole's number.
- *Does:* ignores the click unless a game is running **and** `index == m_moleIndex`. On a real hit: `++m_score` and `emit scoreChanged`. Then the timer is **restarted** so the new mole gets a full fresh interval: with speed-up on, the new interval is `current * 93 / 100` (7 percent faster) but never below 300 ms; with speed-up off, `start()` just restarts with the same interval. Finally `moveMole()`.

**`onMoleTimeout()`** (line 369)
- *Called by:* `m_moleTimer` `timeout()`.
- *Does:* if running, and the penalty option is on and the score is above 0, subtract one point (the score never goes below 0). Then `moveMole()`.

**`onCountdownTick()`** (line 381)
- *Called by:* `m_countdownTimer` `timeout()`, once per second.
- *Does:* `--m_timeLeft`, refresh the label, and when it reaches 0 call `endGame()`. A game of N seconds therefore ends after exactly N ticks.

**`moveMole()`** (line 390)
- *Called by:* `startGame`, `onHoleClicked`, `onMoleTimeout`.
- *Does:* remembers the old hole, empties it via `hideMole()`, then picks a random hole with `QRandomGenerator::global()->bounded(9)` in a `while` loop **until it differs from the previous hole** (so the mole never appears twice in the same place). Stores the new index, sets the button text to `"(o_o)\nMOLE!"` and enables it. `static_cast<int>(HOLES)` is needed because `HOLES` is an unnamed enum value and `bounded` wants a real `int`.

**`hideMole()`** (line 405): if a mole exists, empties that button's text and disables it; sets `m_moleIndex = -1`.

**`endGame()`** (line 414): `resetBoard()`, status "Game over...", then `emit gameOver(m_score)`. Because sender and receiver live in the same thread, the signal acts like an ordinary function call: `showGameOverDialog` runs right here, and its `exec()` waits for the player.

**`resetBoard()`** (line 421): returns the window to idle: `m_running = false`, both timers stopped, mole hidden, settings re-enabled, status "Press Start to play."

**`setSettingsEnabled(bool enabled)`** (line 431): the **master switch between idle and playing**. Enables or disables the four group boxes (disabling a group box also disables everything inside it), enables Start only if idle **and** a name exists, and sets Stop to the opposite of `enabled`.

---

## 8. Complete signal and slot map

| # | Sender | Signal | Receiver | Slot | Syntax | Effect |
|---|---|---|---|---|---|---|
| 1 | Quit button | `clicked()` | window | `close()` | old macros | Window closes, program ends |
| 2 | Name box | `textChanged(QString)` | window | `checkName` | old macros | Start enabled when a name exists |
| 3 | Spin box | `valueChanged(int)` | slider | `setValue(int)` | old macros | Slider follows spin box |
| 4 | Slider | `valueChanged(int)` | spin box | `setValue(int)` | old macros | Spin box follows slider |
| 5 | Spin box | `valueChanged(int)` | window | `onLengthChanged` | old macros | "Time left" shows the chosen length |
| 6 | Start button | `clicked` | window | `startGame` | function pointer | Game begins |
| 7 | Stop button | `clicked` | window | `stopGame` | function pointer | Stop question |
| 8 | Rules button | `clicked` | window | `showRules` | function pointer | Rules window |
| 9 | `m_moleTimer` | `timeout` | window | `onMoleTimeout` | function pointer | Mole was too slow |
| 10 | `m_countdownTimer` | `timeout` | window | `onCountdownTick` | function pointer | One second passed |
| 11 | window | `scoreChanged(int)` | window | `updateScoreLabel` | function pointer | Score label updates |
| 12 | window | `gameOver(int)` | window | `showGameOverDialog` | function pointer | Result window |
| 13-21 | each `m_holes[i]` | `clicked` | window | `onHoleClicked(i)` | **lambda** `[this, i]` | Hit check for hole `i` |
| - | `ClockLabel`'s timer | `timeout()` | the label | `updateTime` | old macros | Clock text updates |
| - | single-shot timer (`main`) | `timeout` | window | `askPlayerName` | function pointer | Name dialog opens once |

**Who emits our own signals**

| Signal | Emitted by | Meaning |
|---|---|---|
| `scoreChanged(int)` | `startGame` (0), `onHoleClicked` (+1), `onMoleTimeout` (-1) | The score has a new value |
| `gameOver(int)` | `endGame` | The time ran out; carries the final score |

**Notes**
- **Rows 3 and 4 link both ways without looping forever.** Qt sends `valueChanged` only if the value really changes, so after one step both widgets hold the same value and the chain stops.
- **Row 5:** one signal (`valueChanged` of the spin box) is connected to **two** slots (rows 3 and 5). One signal can drive many slots.
- **Why a lambda for the holes:** `clicked()` does not say which button was clicked. Each lambda remembers its own `i` (the capture `[this, i]` copies `i` and gives access to the window) and calls `onHoleClicked(i)`.

---

## 9. State: variables, modes and what is enabled when

The game has two modes, controlled by `m_running`:

| Widget | Idle, no name | Idle, name entered | Playing |
|---|---|---|---|
| Name, Difficulty, Length, Options groups | enabled | enabled | **disabled** |
| Start button | **disabled** | enabled | disabled |
| Stop button | disabled | disabled | **enabled** |
| Rules and Quit buttons | enabled | enabled | enabled |
| Hole buttons | all disabled | all disabled | only the mole's hole enabled |

**Transitions**

```
 IDLE --startGame()--> PLAYING --time reaches 0--> endGame() --> resetBoard() --> IDLE
                          |                                           |
                          |                          then the result window:
                          |                          Yes = startGame() again, No = stay IDLE
                          |
                          '--Stop pressed, answer Yes--> resetBoard() --> IDLE
```

**Invariants** that always hold:
- `m_moleIndex` is -1 whenever no game runs, otherwise a number from 0 to 8.
- At most one hole button is enabled at any time.
- `m_running` is `true` exactly between `startGame()` and `resetBoard()`.
- `m_score` never goes below 0.

---

## 10. Timers in depth

The program uses **four** timers:

| Timer | Created in | Interval | Repeats | Job |
|---|---|---|---|---|
| `m_moleTimer` | `createWidgets` | Easy 1200, Medium 850, Hard 550 ms (set in `startGame`) | yes | If it fires, the mole was too slow |
| `m_countdownTimer` | `createWidgets` | 1000 ms | yes | Counts the game time down |
| `ClockLabel`'s timer | `ClockLabel` constructor | 1000 ms | yes | Updates the clock text |
| single-shot timer | `main()` | 0 ms | **once** | Opens the name dialog after the window is up |

**Using a `QTimer` always follows three steps** (Lesson 7): create it, connect `timeout()` to a slot, call `start()`.

**`start()` on a running timer restarts it.** That is why `onHoleClicked()` calls `m_moleTimer->start(...)`: after a hit the new mole gets a **full** interval instead of the leftover of the old one. `start(int msec)` also **changes** the interval; plain `start()` reuses the current interval.

**The two game timers are independent.** The mole timer can fire many times between two countdown ticks, and a hit restarts only the mole timer, never the countdown.

### Speed-up sequences

With speed-up on, each hit does `interval = interval * 93 / 100` (integer arithmetic) with a floor of 300 ms. These are the exact values:

| Difficulty | Interval after each hit (ms) | Hits to reach the 300 ms floor |
|---|---|---|
| Easy | 1200, 1116, 1037, 964, 896, 833, 774, 719, 668, 621, 577, 536, 498, 463, 430, 399, 371, 345, 320, **300** | 19 |
| Medium | 850, 790, 734, 682, 634, 589, 547, 508, 472, 438, 407, 378, 351, 326, 303, **300** | 15 |
| Hard | 550, 511, 475, 441, 410, 381, 354, 329, 305, **300** | 9 |

The speed-up exists only for the current game: `startGame()` always begins with `difficultyInterval()` again.

---

## 11. API index: every Qt class and function used

| Class or function | Used in | What it is used for |
|---|---|---|
| `QApplication(argc, argv)` | `main` | The application object |
| `QApplication::exec()` | `main` | Runs the event loop |
| `QWidget` (base class) | `GameWindow` | The main window |
| `QWidget::show()` | `main` | Makes the window visible |
| `QWidget::setLayout()` | `createLayout`, group builders | Installs a layout on a widget |
| `QWidget::setWindowTitle()` | constructor | Title bar text |
| `QWidget::resize(w, h)` | constructor | Initial window size |
| `QWidget::setEnabled(bool)` | many | Greys widgets in or out (also for a group box with all its children) |
| `QWidget::setFocus()` | `askPlayerName` | Puts the cursor in the name box |
| `QWidget::close()` | Quit button | Closes the window |
| `QLabel(text)` | `createWidgets` | Text display; understands simple HTML like `<b>` |
| `QLabel::setText()` | several | Changes the text |
| `QLabel::setBuddy()` | `createPlayerGroup` | Alt+N jumps to the name box |
| `QLineEdit` | `createPlayerGroup` | One-line text input |
| `QLineEdit::setMaxLength()` | `createPlayerGroup` | Limits the name to 20 characters |
| `QLineEdit::text()`, `setText()` | several | Read and write the name |
| `textChanged(QString)` signal | connection 2 | Fires on every keystroke |
| `QPushButton` | buttons, holes | Clickable button |
| `QPushButton::setText()` | `moveMole`, `hideMole` | Shows or clears the mole text |
| `clicked()` signal | many | Fires when a button is pressed |
| `QRadioButton`, `setChecked`, `isChecked` | difficulty | Exclusive choice inside one parent |
| `QCheckBox`, `setChecked`, `isChecked` | options | Independent on/off options |
| `QSpinBox`, `setRange`, `setSuffix`, `setValue`, `value` | game length | Numeric input with arrows |
| `QSlider(Qt::Horizontal)`, `setRange`, `setValue` | game length | Slider for the same number |
| `valueChanged(int)` signal | spin box, slider | Fires when the number changes |
| `QGroupBox(title)`, `setLayout`, `setEnabled` | settings, field | Titled frame / container |
| `QHBoxLayout`, `QVBoxLayout` | all bars and columns | Row / column of widgets |
| `addWidget`, `addLayout(layout, stretch)` | `createLayout` | Add children; stretch factor 1 = take the extra space |
| `addStretch()`, `addSpacing(n)` | `createLayout` | Springy gap / fixed gap |
| `QGridLayout::addWidget(w, row, col)` | `createFieldGroup` | Position in a grid |
| `QTimer(parent)`, `setInterval`, `start()`, `start(ms)`, `stop`, `interval()` | game timers, clock | Repeating timing |
| `timeout()` signal | timers | Fires when the interval elapses |
| `QTimer::singleShot(ms, receiver, slot)` | `main` | Calls a slot once after a delay |
| `QTime::currentTime()`, `toString(format)` | `ClockLabel` | Current time as text |
| `QMessageBox::information(...)` | `showRules` | One-call info window |
| `QMessageBox::question(...)` | `stopGame` | One-call Yes/No question, returns the button |
| `QMessageBox` object: `setWindowTitle`, `setIcon`, `setText`, `setInformativeText`, `setStandardButtons`, `setDefaultButton`, `exec` | `showGameOverDialog` | Fully customised dialog |
| `QMessageBox::Yes`, `No`, `Information` | dialogs | Standard buttons and icon |
| `QInputDialog::getText(...)` | `askPlayerName` | Modal text prompt |
| `QString::arg()`, `trimmed()`, `isEmpty()`, `+=` | several | Build and clean text; `%1` is replaced by the argument |
| `QRandomGenerator::global()->bounded(n)` | `moveMole` | Random integer from 0 to n-1 (Qt 5.10 or newer) |
| `Q_OBJECT`, `signals`, `slots`, `emit`, `SIGNAL()`, `SLOT()` | headers, `.cpp` | Qt's signal and slot machinery |

**C++ features used:** inheritance (`: public QWidget`), constructor initializer lists, header guards (`#ifndef`), forward declarations, `nullptr`, a `const` member function, an unnamed `enum` for constants, arrays of pointers (`m_holes[9]`), a `for` loop, a `while` loop, `switch`, `static_cast`, and a lambda with a capture list.

---

## 12. Which lesson each part comes from

| Feature | Lesson |
|---|---|
| `.pro` file, `QApplication`, `exec()`, showing a widget | Lesson 1 |
| Quit button with `SIGNAL/SLOT`; Start/Stop with `&QPushButton::clicked`; holes with a lambda; `<b>` in labels | Lesson 2 |
| Spin box and slider linked in both directions; `QHBoxLayout`; `setLayout`; `Q_OBJECT` and moc | Lesson 3 |
| Radio buttons, check boxes, group boxes, `QGridLayout` | Lesson 5 |
| `QInputDialog::getText`, `QMessageBox::information`, `question`, property-based `QMessageBox` with `exec()` and `switch` | Lesson 6 |
| `QTimer`, `timeout()`, `start()`, `stop()`, `QTime`, the `ClockLabel` homework | Lesson 7 |
| `createWidgets / createLayout / makeConnections` structure, `setBuddy`, `&` shortcuts, `addStretch`, enable/disable, own signals with `emit` | Find Dialog files |

Slightly beyond the lessons (small helpers, be ready to explain them): `QTimer::singleShot`, `QRandomGenerator`, `QString::arg` and `trimmed`, `setSuffix`, `setMaxLength`, `setMinimumSize`, `addSpacing`.

---

## 13. Design decisions (the "why")

| Decision | Reason |
|---|---|
| One big class `GameWindow` | The game is small; one class keeps every connection visible in one file |
| Setup split into `createWidgets / createLayout / makeConnections` | Short, readable constructor; same structure as the Find Dialog |
| Holes are plain buttons, enabled = mole | Needs no custom painting, and a disabled button cannot be clicked by mistake |
| Own signals `scoreChanged` and `gameOver` | The sender does not need to know who reacts; this is the Qt way |
| One `setSettingsEnabled(bool)` for locking | One place decides how the window looks in idle versus playing mode |
| Timers stopped before the Stop question | Modal dialogs do not pause timers |
| `start()` after every hit | The next mole always gets a full interval |
| Mole never repeats the same hole | A repeat would feel like a missed mole and look like a bug |
| Spin box and slider both exist | Demonstrates two-way signal and slot linking from Lesson 3 |
| `QTimer::singleShot(0, ...)` for the name dialog | The dialog appears over a visible window |
| All three `connect` syntaxes | Shows the old, the function-pointer and the lambda way, each where it fits |

---

## 14. Known behaviours and limits

Honest list of things that are **by design or simply not handled**. Good to know before you present.

| Behaviour | Explanation |
|---|---|
| **Pressing Rules while a game runs does not pause it** | The Rules dialog is modal, but the timers keep running behind it (Stop pauses them; Rules does not). Fix in the cookbook, one line. |
| The best score is lost when the program closes | It lives only in `m_bestScore`; nothing is saved to disk |
| After "Stop, No" the pause is approximate | `start()` restarts both timers, so up to about one second may be added |
| Closing the window mid-game just ends the program | There is no confirmation |
| Settings cannot change during a game | The group boxes are disabled on purpose |
| Choosing "No" in the game over dialog keeps the status text "Game over..." | Only the time label is reset |
| Needs Qt 5.10 or newer | `QRandomGenerator` was added in 5.10; the project was also built with Qt 5.15 and 6.4 |
| The name is only used in the result window | It is not stored or shown elsewhere |

---

## 15. Cookbook: how to change the game

| Goal | Where | Edit |
|---|---|---|
| Faster or slower difficulties | `difficultyInterval()` | Change `1200`, `850`, `550` |
| Stronger or weaker speed-up | `onHoleClicked()` | `93 / 100` is 7 percent faster per hit; use `90 / 100` for 10 percent |
| Lower speed limit | `onHoleClicked()` | Change `300` (twice) |
| Longer games allowed | `createLengthGroup()` | `setRange(10, 120)` on **both** spin box and slider |
| Different default length | `createLengthGroup()` | `setValue(30)` on both; also `m_timeLeft(30)` in the constructor |
| Different mole text | `moveMole()` | Change `"(o_o)\nMOLE!"` |
| Bigger field | `game_window.h` line 62 | `ROWS = 4, COLS = 4`; everything else adapts (tested: it builds) |
| **Pause-proof Rules** | `setSettingsEnabled()` | Add `m_rulesButton->setEnabled(enabled);` so Rules is disabled during a game |
| Longer names | `createPlayerGroup()` | Change `setMaxLength(20)` |

**Adding a fourth difficulty ("Insane")** takes four small edits:

```cpp
// 1. game_window.h, next to the other radio buttons:
QRadioButton* m_insaneRadio;

// 2. createDifficultyGroup(), after creating m_hardRadio:
m_insaneRadio = new QRadioButton("&Insane");
// ... and after lyt->addWidget(m_hardRadio):
lyt->addWidget(m_insaneRadio);

// 3. difficultyInterval(), as the first test:
if (m_insaneRadio->isChecked())
    return 350;
```

Nothing else needs to change: the radio buttons share one parent, so they exclude each other automatically.

---

## 16. Build system

**`WhackAMole.pro`** (qmake, used by Qt Creator):

| Line | Meaning |
|---|---|
| `TEMPLATE = app` | Build an executable, not a library |
| `TARGET = WhackAMole` | Name of the program |
| `QT = core gui` | Use the Qt core and GUI modules |
| `greaterThan(QT_MAJOR_VERSION, 4): QT += widgets` | Qt 5 and 6 keep widgets in a separate module |
| `CONFIG += c++11` | Language standard |
| `SOURCES += ...` | The `.cpp` files to compile |
| `HEADERS += ...` | The headers; they contain `Q_OBJECT`, so moc must process them |

**What the build does:** qmake writes a Makefile; `moc` reads each header with `Q_OBJECT` and generates `moc_*.cpp` (the real code behind signals and slots); the compiler builds all `.cpp` files; the linker joins them with the Qt libraries.

**`CMakeLists.txt`** does the same for CMake. `CMAKE_AUTOMOC ON` runs moc automatically, and `find_package(... Qt6 Qt5 ...)` accepts either Qt version.

**`run_windows.bat` / `run_mac.command`** find your Qt installation, run qmake, build and start the game (details in section 2). On Windows the script matches the compiler to the Qt kit (a Qt 5 kit named `mingw81_64` uses `Tools\mingw810_64`; a Qt 6 kit named `mingw_64` uses the newest MinGW in `Tools`). `WhackAMole_Launcher.bat` is a menu around it.

---

## 17. Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| Qt Creator shows a wall of numbers | You opened the `.zip` | Unzip first, then File > Open File or Project > `WhackAMole.pro` |
| The `.pro` file opens as plain text; Projects icon is grey | Opened as a file, not as a project | Use File > Open File or Project... |
| "Configure Project" shows no kit | Qt libraries or MinGW not installed | Qt Maintenance Tool: add Qt (MinGW 64-bit) and the MinGW compiler |
| Complaint about in-source build files | Leftovers from another build in the folder | Delete `Makefile`, `.qmake.stash`, `*.o`, `moc_*`, `WhackAMole.app` |
| Start button is grey | The name box is empty | Type a name |
| The settings are grey | A game is running | Press Stop or wait for the end |
| Red underlines in an editor, but the build works | The editor cannot find the Qt headers | Cosmetic; run `run_mac.command` once on a Mac to generate `.clangd` |
| Build error mentioning `QRandomGenerator` | Qt older than 5.10 | Use a newer Qt |

---

## 18. Questions your teacher may ask

**What is a signal and what is a slot?**
A signal is a message a widget sends when something happens (`clicked()`). A slot is an ordinary function that runs when a connected signal is sent. `connect` links them.

**What does `Q_OBJECT` do?**
It switches on signals and slots (the meta-object system) for the class. The `moc` tool reads it and generates the extra code.

**What does `app.exec()` do?**
It starts the event loop: the program waits for events (clicks, timers) and calls our functions. It returns when the last window closes.

**How does the program know which hole was clicked?**
Each of the 9 buttons is connected through a lambda that remembers its own number `i` and calls `onHoleClicked(i)`. The function compares `i` with `m_moleIndex`.

**Why two timers?**
They do different jobs at different speeds: the mole timer (550 to 1200 ms depending on difficulty) moves the mole, the countdown timer (1000 ms) counts game seconds.

**Why do you stop the timers before the Stop question?**
A modal dialog does not stop timers. Without stopping them the game would keep running behind the question.

**Why `new` without `delete`?**
Widgets placed in a layout become children of the window, and a parent deletes its children automatically. Timers get `this` as parent. The main window lives until the program ends.

**Why are empty holes disabled buttons?**
A disabled button cannot be clicked and looks greyed out, so it acts as an empty hole without any drawing code.

**Why does linking the spin box and the slider not loop forever?**
`valueChanged` is sent only when the value really changes, so after one round both hold the same value.

**Why `QTimer::singleShot(0, ...)` for the name dialog?**
It runs the dialog right after the event loop starts, so the main window is already visible behind it.

**Where could the game break?**
Mainly at the edges listed in section 14, for example pressing Rules during a game, which does not pause the timers.

---

*Built with Qt Widgets. Works with Qt 5.15 and Qt 6 (tested: 5.15, 6.4; running on 6.11).*
