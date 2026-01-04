//
// Created by HowXu on 2025/12/26.
//

#pragma once

#include "BuildConfig.h"
#include "minecraft/auth/AuthFlow.h"
#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Upload.h"

class YggdrasilAuthStep : public AuthStep {
    Q_OBJECT

   public:
    YggdrasilAuthStep(AccountData* data, AuthFlow::Action action, QString password);
    virtual ~YggdrasilAuthStep() noexcept = default;

    void perform() override;

    QString describe() override { return tr("Yggdrasil account authentication"); }

protected:
    virtual QString authType() { return "Yggdrasil"; }

    virtual QString authUrl() { return m_data->authUrl; }

    virtual QString clientID() { return BuildConfig.LAUNCHER_NAME; }

    virtual QString requestUrl();

    QString requestTemplate();

    QString fillRequest();

    bool parseResponse();

protected slots:
 virtual void onRequestDone();

protected:
    std::shared_ptr<QByteArray> m_response;
    Net::Upload::Ptr m_request;
    NetJob::Ptr m_task;

    const QString m_password;
    const AuthFlow::Action m_action;
};