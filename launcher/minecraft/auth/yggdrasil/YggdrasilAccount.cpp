//
// Created by HowXu on 2025/12/26.
//

#include "YggdrasilAccount.h"
#include <utility>

YggdrasilAccount::YggdrasilAccount(QObject* parent) : BaseAccount(parent) {}

YggdrasilAccountPtr YggdrasilAccount::createYggdrasil(const QString& login,
                                             const QString& authUrl,
                                             const QString& loginUrl,
                                             const QString& refreshUrl)
{
    YggdrasilAccountPtr account(new YggdrasilAccount);
    account->data.type = AccountType::Yggdrasil;
    account->data.accountLogin = login;
    account->data.authUrl = authUrl;
    account->data.loginUrl = loginUrl;
    account->data.refreshUrl = refreshUrl;
    return account;
}

shared_qobject_ptr<AuthFlow> YggdrasilAccount::login(QString password)
{
    Q_ASSERT(m_currentTask.get() == nullptr);

    m_currentTask.reset(new AuthFlow(&data, AuthFlow::Action::Login, std::move(password)));
    connect(m_currentTask.get(), &Task::succeeded, this, &YggdrasilAccount::authSucceeded);
    connect(m_currentTask.get(), &Task::failed, this, &YggdrasilAccount::authFailed);
    connect(m_currentTask.get(), &Task::aborted, this, [this] { authFailed(tr("Aborted")); });
    emit activityChanged(true);

    return m_currentTask;
}