#include "play_window.h"
#include "ui_play_window.h"

play_window::play_window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::play_window)
{
    ui->setupUi(this);
}

play_window::~play_window()
{
    delete ui;
}
