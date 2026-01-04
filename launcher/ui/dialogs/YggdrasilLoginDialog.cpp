//
// Created by HowXu on 2025/12/26.
//

#include "YggdrasilLoginDialog.h"
#include "ui_YggdrasilLoginDialog.h"

#include <QPushButton>
#include <QUrl>

YggdrasilLoginDialog::YggdrasilLoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::YggdrasilLoginDialog)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ui->userTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onTextBoxesChanged);
    connect(ui->passTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onTextBoxesChanged);
    connect(ui->authUrlTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onTextBoxesChanged);
    connect(ui->loginUrlTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onTextBoxesChanged);
    connect(ui->refreshUrlTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onTextBoxesChanged);

    connect(ui->authUrlTextBox, &QLineEdit::textChanged, this, &YggdrasilLoginDialog::onAuthUrlTextBoxChanged);
}

YggdrasilLoginDialog::~YggdrasilLoginDialog()
{
    delete ui;
}

// Stage 1: User interaction
void YggdrasilLoginDialog::accept()
{
    QUrl url = ui->authUrlTextBox->text();
    if (!url.isValid()) {
        emit onTaskFailed(tr("Provided URL isn't valid"));
        return;
    }
    if (url.scheme() != "https") {
        emit onTaskFailed(tr("Use encrypted HTTP protocol (https://)"));
        return;
    }

    setUserInputsEnabled(false);
    ui->progressBar->setVisible(true);

    // Setup the login task and start it
    m_account = YggdrasilAccount::createYggdrasil(ui->userTextBox->text(), url.toString(QUrl::StripTrailingSlash), ui->loginUrlTextBox->text(),
                                            ui->refreshUrlTextBox->text());
    m_loginTask = m_account->login(ui->passTextBox->text());
    connect(m_loginTask.get(), &Task::failed, this, &YggdrasilLoginDialog::onTaskFailed);
    connect(m_loginTask.get(), &Task::succeeded, this, &YggdrasilLoginDialog::onTaskSucceeded);
    connect(m_loginTask.get(), &Task::status, this, &YggdrasilLoginDialog::onTaskStatus);
    connect(m_loginTask.get(), &Task::progress, this, &YggdrasilLoginDialog::onTaskProgress);
    m_loginTask->start();
}

void YggdrasilLoginDialog::setUserInputsEnabled(bool enable)
{
    ui->authUrlTextBox->setEnabled(enable);
    ui->loginUrlTextBox->setEnabled(enable);
    ui->refreshUrlTextBox->setEnabled(enable);
    ui->userTextBox->setEnabled(enable);
    ui->passTextBox->setEnabled(enable);
    ui->buttonBox->setEnabled(enable);
}

// Enable the OK button only when both textboxes contain something.
void YggdrasilLoginDialog::onTextBoxesChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)
        ->setEnabled(!ui->userTextBox->text().isEmpty() && !ui->passTextBox->text().isEmpty() && !ui->authUrlTextBox->text().isEmpty() &&
                     !ui->loginUrlTextBox->text().isEmpty() && !ui->refreshUrlTextBox->text().isEmpty());
}

void YggdrasilLoginDialog::onTaskFailed(const QString& reason)
{
    // Set message
    auto lines = reason.split('\n');
    QString processed;
    for (auto line : lines) {
        if (line.size()) {
            processed += "<font color='red'>" + line + "</font><br />";
        } else {
            processed += "<br />";
        }
    }
    ui->label->setText(processed);

    // Re-enable user-interaction
    setUserInputsEnabled(true);
    ui->progressBar->setVisible(false);
}

void YggdrasilLoginDialog::onTaskSucceeded()
{
    QDialog::accept();
}

void YggdrasilLoginDialog::onTaskStatus(const QString& status)
{
    ui->label->setText(status);
}

void YggdrasilLoginDialog::onTaskProgress(qint64 current, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(current);
}

void YggdrasilLoginDialog::onAuthUrlTextBoxChanged()
{
    ui->loginUrlTextBox->setText("/authserver/authenticate");
    ui->refreshUrlTextBox->setText("/authserver/refresh");
}

// Public interface
YggdrasilAccountPtr YggdrasilLoginDialog::newAccount(QWidget* parent, QString msg)
{
    YggdrasilLoginDialog dlg(parent);
    dlg.ui->label->setText(msg);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.m_account;
    }
    return nullptr;
}