#ifndef PLAY_WINDOW_H
#define PLAY_WINDOW_H

#include <QWidget>

namespace Ui {
class play_window;
}

class play_window : public QWidget
{
    Q_OBJECT

public:
    explicit play_window(QWidget *parent = nullptr);
    ~play_window();

private:
    Ui::play_window *ui;
};

#endif // PLAY_WINDOW_H
