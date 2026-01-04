//
// Created by HowXu on 2025/12/26.
//

#include "YggdrasilAuthStep.h"

#include "Application.h"
#include "Logging.h"
#include "net/NetUtils.h"

#include <utility>

YggdrasilAuthStep::YggdrasilAuthStep(AccountData* data, AuthFlow::Action action, QString password)
    : AuthStep(data), m_password(std::move(password)), m_action(action)
{}

void YggdrasilAuthStep::perform()
{
    const QUrl url(authUrl() + requestUrl());
    const QString requestData = fillRequest();

    m_response.reset(new QByteArray());
    m_request = Net::Upload::makeByteArray(url, m_response, requestData.toUtf8());

    m_task.reset(new NetJob(authType() + "AuthStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &YggdrasilAuthStep::onRequestDone);

    m_task->start();
    qDebug() << "Getting authorization token for " + authType() + " account";
}

QString YggdrasilAuthStep::requestUrl()
{
    return m_action == AuthFlow::Action::Login ? m_data->loginUrl : m_data->refreshUrl;
}

QString YggdrasilAuthStep::requestTemplate()
{
    if (m_action == AuthFlow::Action::Login) {
        return R"XXX(
{
    "username": "%1",
    "password": "%2",
    "clientToken": "%3",
    "requestUser": false,
    "agent":{
		"name":"Minecraft",
		"version":1
	}
}
)XXX";
    }
    return R"XXX(
{
    "accessToken": "%1",
    "clientToken": "%2",
    "requestUser": false
}
)XXX";
}

QString YggdrasilAuthStep::fillRequest()
{
    if (m_action == AuthFlow::Action::Login) {
        return requestTemplate().arg(m_data->accountLogin, m_password, clientID());
    } else {
        return requestTemplate().arg(m_data->yggdrasilToken.token, m_data->clientID);
    }
}

bool YggdrasilAuthStep::parseResponse()
{
    qCDebug(authCredentials()) << *m_response;
    if (m_request->error() != QNetworkReply::NoError) {
        qWarning() << "Reply error:" << m_request->error();
        return false;
    }

    auto jsonResponse = QJsonDocument::fromJson(*m_response);

    m_data->yggdrasilToken.token = jsonResponse["accessToken"].toString();

    m_data->clientID = jsonResponse["clientToken"].toString();

    // here process the profiles
    auto selectedProfile = jsonResponse["selectedProfile"].toObject();
    auto availableProfiles = jsonResponse["availableProfiles"].toArray();
    if (!selectedProfile.isEmpty())
    {
        m_data->minecraftProfile.id = selectedProfile["id"].toString();
        m_data->minecraftProfile.name = selectedProfile["name"].toString();
    }else
    {
        if (availableProfiles.size() > 0)
        {
            m_data->minecraftProfile.id = availableProfiles[0].toObject()["id"].toString();
            m_data->minecraftProfile.name = availableProfiles[0].toObject()["name"].toString();
        }else
        {
            qWarning() << "Parse Error: No profile id and name";
        }
    }
    

    return true;
}

void YggdrasilAuthStep::onRequestDone()
{
    if (!parseResponse()) {
        emit finished(AccountTaskState::STATE_OFFLINE,
                      tr("Failed to get authorization for %1 account: %2").arg(authType(), m_request->errorString()));
        return;
    }
    emit finished(AccountTaskState::STATE_WORKING, tr("Got authorization for %1 account").arg(authType()));
}