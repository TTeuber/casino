#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QRandomGenerator>
#include <iostream>
#include <QTimer>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->hitButton, SIGNAL (released()), this, SLOT (hit()));
    connect(ui->betButton, SIGNAL (released()), this, SLOT (changeBet()));
    connect(ui->playButton, SIGNAL (released()), this, SLOT (play()));
    connect(ui->standButton, SIGNAL (released()), this, SLOT (stand()));

    ui->betLabel->setText("Your Bet: 20");
    ui->moneyLabel->setText("Money: 1000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList cards =
{
    "A ♣", "2 ♣", "3 ♣", "4 ♣", "5 ♣", "6 ♣", "7 ♣", "8 ♣", "9 ♣", "10 ♣", "J ♣", "Q ♣", "K ♣",
    "A ♦", "2 ♦", "3 ♦", "4 ♦", "5 ♦", "6 ♦", "7 ♦", "8 ♦", "9 ♦", "10 ♦", "J ♦", "Q ♦", "K ♦",
    "A ♥", "2 ♥", "3 ♥", "4 ♥", "5 ♥", "6 ♥", "7 ♥", "8 ♥", "9 ♥", "10 ♥", "J ♥", "Q ♥", "K ♥",
    "A ♠", "2 ♠", "3 ♠", "4 ♠", "5 ♠", "6 ♠", "7 ♠", "8 ♠", "9 ♠", "10 ♠", "J ♠", "Q ♠", "K ♠",
};

QStringList used;
int playerAces = 0;
int dealerAces = 0;

QString chooseCard()
{
    int value = QRandomGenerator::global()->bounded(1, 53);
    QString card = cards.value(value);
    if (used.filter(card).length() > 0)
    {
        return chooseCard();
    }
    else
    {
        used << card;
        return card;
    }
}

int cardValue(QString card)
{
    QStringList cList = card.split(" ");
    return cList.value(0).toInt();
}

QString card(QString card)
{
    return card.remove(" ");
}

void MainWindow::hit()
{
    QString card = chooseCard();
    QStringList cardList = card.split(" ");
    int value;
    if (cardList.value(0).toInt() > 0)
    {
        value = cardList.value(0).toInt();
    }
    else if (cardList.value(0) == "A")
    {
        value = 11;
        playerAces++;
    }
    else
    {
        value = 10;
    }
    QString str = ui->playerCardLabel->text();
    ui->playerCardLabel->setText(str + " " + card.remove(" "));
    int total = ui->playerCounterLabel->text().toInt();
    ui->playerCounterLabel->setText(QString::number(total + value));
    if ((total + value) == 21)
    {
        stand();
    }
    if ((total + value) > 21)
    {
        while (playerAces > 0)
        {
            total = ui->playerCounterLabel->text().toInt();
            total -= 10;
            ui->playerCounterLabel->setText(QString::number(total));
            playerAces -= 1;
        }
        if ((total + value) > 21)
        {
            ui->hitButton->setEnabled(false);
            ui->standButton->setEnabled(false);
            ui->playerTitleLabel->setText("Bust");
            updateMoney(false);
            ui->betButton->setEnabled(true);
            ui->playButton->setEnabled(true);
        }
    }
}

void MainWindow::dealerHit()
{
    delay();
    QString card = chooseCard();
    QStringList cardList = card.split(" ");
    int value;
    if (cardList.value(0).toInt() > 0)
    {
        value = cardList.value(0).toInt();
    }
    else if (cardList.value(0) == "A")
    {
        value = 11;
        dealerAces++;
    }
    else
    {
        value = 10;
    }
    QString str = ui->dealerCardLabel->text();
    ui->dealerCardLabel->setText(str + " " + card.remove(" "));
    int total = ui->dealerCounterLabel->text().toInt();
    ui->dealerCounterLabel->setText(QString::number(total + value));
}



void MainWindow::clear()
{
    ui->playerCardLabel->setText("Card:");
    ui->dealerCardLabel->setText("Card:");
    ui->playerCounterLabel->setText("0");
    ui->dealerCounterLabel->setText("0");
    ui->playerTitleLabel->setText("Your Hand");
    ui->dealerTitleLabel->setText("Dealer's Hand");
    playerAces = 0;
    dealerAces = 0;
    used.clear();
    if (ui->hitButton->isEnabled() == false)
    {
        ui->hitButton->setEnabled(true);
    }
}

void MainWindow::play()
{
    clear();
    hit();
    delay();
    hit();
    dealerHit();
    dealerHit();
    ui->playButton->setEnabled(false);
    ui->betButton->setEnabled(false);
    ui->standButton->setEnabled(true);
    ui->hitButton->setEnabled(true);


}

void MainWindow::stand()
{
    ui->standButton->setEnabled(false);
    ui->hitButton->setEnabled(false);
    ui->playButton->setEnabled(true);
    ui->betButton->setEnabled(true);
    while (ui->dealerCounterLabel->text().toInt() < 17)
    {
        dealerHit();
    }
    if (ui->dealerCounterLabel->text().toInt() <= 21)
    {
        if (ui->playerCounterLabel->text().toInt() > ui->dealerCounterLabel->text().toInt())
        {
            ui->playerTitleLabel->setText("Win!");
            updateMoney(true);
        }
        else if (ui->playerCounterLabel->text().toInt() < ui->dealerCounterLabel->text().toInt())
        {
            ui->playerTitleLabel->setText("Lose");
            updateMoney(false);
        }
        else
        {
            ui->playerTitleLabel->setText("Draw");
        }
    }
    else
    {
        ui->dealerTitleLabel->setText("Dealer Bust");
        ui->playerTitleLabel->setText("Win!");
        updateMoney(true);
    }
}

void MainWindow::delay()
{
    QEventLoop loop;
    QTimer::singleShot(500, &loop, &QEventLoop::quit);
    loop.exec();
}

void MainWindow::changeBet()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        ui->betLabel->setText("Your Bet: " + text);
}

void MainWindow::updateMoney(bool win)
{
    QStringList str = ui->moneyLabel->text().split(" ");
    int total = str.value(1).toInt();
    QStringList str2 = ui->betLabel->text().split(" ");
    int bet = str2.value(2).toInt();
    if (win)
    {
        ui->moneyLabel->setText("Money: " + QString::number(total + bet));
    }
    else
    {
        ui->moneyLabel->setText("Money: " + QString::number(total - bet));
    }
}

